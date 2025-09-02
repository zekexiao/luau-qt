#include "qlua.h"

#include <lua.hpp>

#include <QDebug>
#include <QVector3D>

#include "qllibs.h"

class QLuaPrivate
{
public:
    static int luaPrint(lua_State *L)
    {
        int nargs = lua_gettop(L);
        QString output;

        for (int i = 1; i <= nargs; i++) {
            if (i > 1)
                output += " "; // 参数之间添加空格

            // 尝试将任何类型转换为字符串
            if (lua_isstring(L, i)) {
                output += QString::fromUtf8(lua_tostring(L, i));
            } else if (lua_isnumber(L, i)) {
                output += QString::number(lua_tonumber(L, i));
            } else if (lua_isboolean(L, i)) {
                output += lua_toboolean(L, i) ? "true" : "false";
            } else if (lua_isnil(L, i)) {
                output += "nil";
            } else {
                output += QString("<%1>").arg(lua_typename(L, lua_type(L, i)));
            }
        }

        qDebug() << "[Lua]" << output;
        return 0;
    }

    QVariant parseTable(int index)
    {
        // 确保索引处是一个表
        if (!lua_istable(L, index)) {
            return QVariant();
        }
        // 规范化索引（如果是负数）
        if (index < 0) {
            index = lua_gettop(L) + index + 1;
        }
        // 检查表是否为数组形式（第一个元素没有键）
        bool isArray = true;
        // 将表复制到栈顶以便遍历
        lua_pushvalue(L, index);
        lua_pushnil(L); // 第一个键
        if (lua_next(L, -2)) {
            // 检查第一个键是否为数字1
            isArray = lua_isnumber(L, -2) && (lua_tonumber(L, -2) == 1);
            // 弹出值和键，保留表
            lua_pop(L, 2);
        } else {
            // 空表，默认当作数组处理
            lua_pop(L, 1); // 弹出nil
        }
        if (isArray) {
            // 解析为QList<QVariant>
            QList<QVariant> result;
            // 遍历数组
            lua_pushvalue(L, index); // 再次将表压入栈
            int len = lua_rawlen(L, -1);
            for (int i = 1; i <= len; i++) {
                lua_rawgeti(L, -1, i);
                result.append(toValue(-1)); // 使用您已实现的toValue函数
                lua_pop(L, 1);
            }
            lua_pop(L, 1); // 弹出表
            return QVariant::fromValue(result);
        } else {
            // 解析为QVariantMap
            QVariantMap result;
            // 遍历表
            lua_pushvalue(L, index); // 再次将表压入栈
            lua_pushnil(L);          // 第一个键
            while (lua_next(L, -2)) {
                // 键在-2，值在-1
                QVariant key;
                if (lua_isstring(L, -2)) {
                    key = QString::fromUtf8(lua_tostring(L, -2));
                } else if (lua_isnumber(L, -2)) {
                    key = lua_tonumber(L, -2);
                } else {
                    // 其他类型的键，跳过
                    qWarning() << "unsupported key type" << lua_typename(L, lua_type(L, -2));
                    lua_pop(L, 1);
                    continue;
                }
                QVariant value = toValue(-1);
                result[key.toString()] = value;

                lua_pop(L, 1); // 弹出值，保留键用于下一次迭代
            }

            lua_pop(L, 1); // 弹出表
            return result;
        }
    }
    void pushValue(const QVariant &value)
    {
        switch (value.typeId()) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Double:
        case QMetaType::Float: {
            lua_pushnumber(L, value.toDouble());
        } break;
        case QMetaType::Bool: {
            lua_pushboolean(L, value.toBool());
        } break;
        case QMetaType::QString: {
            auto str = value.toString();
            lua_pushstring(L, str.toUtf8());
        } break;
        case QMetaType::QVector3D: {
            auto point = value.value<QVector3D>();
            auto udata = (QVector3D **) lua_newuserdata(L, sizeof(QVector3D *));
            *udata = new QVector3D(point);
            luaL_getmetatable(L, QLUA_VECTOR3D_METATABLE_NAME);
            lua_setmetatable(L, -2);
            break;
        }
        case QMetaType::QVariantList: {
            QVariantList list = value.toList();
            lua_createtable(L, list.size(), 0);
            for (int i = 0; i < list.size(); i++) {
                pushValue(list[i]);
                lua_rawseti(L, -2, i + 1);
            }
            break;
        }
        case QMetaType::QPointF: {
            auto point = value.toPointF();
            auto udata = (QPointF **) lua_newuserdata(L, sizeof(QPointF *));
            *udata = new QPointF(point);
            luaL_getmetatable(L, QLUA_POINT_METATABLE_NAME);
            lua_setmetatable(L, -2);
            break;
        }
        case QMetaType::QPoint: {
            auto point = value.toPoint();
            auto udata = (QPointF **) lua_newuserdata(L, sizeof(QPointF *));
            *udata = new QPointF(point);
            luaL_getmetatable(L, QLUA_POINT_METATABLE_NAME);
            lua_setmetatable(L, -2);
            break;
        }
        default: {
            qWarning() << "unknown qt type" << value.typeName();
        } break;
        }
    }

    QVariant toValue(int index)
    {
        if (L == nullptr || index == 0) {
            return QVariant();
        }
        QVariant result;
        int type = lua_type(L, index);
        switch (type) {
        case LUA_TNUMBER: {
            result = lua_tonumber(L, index);
        } break;
        case LUA_TSTRING: {
            size_t len;
            auto bytes = lua_tolstring(L, index, &len);
            result = QString::fromUtf8(bytes, len);
        } break;
        case LUA_TBOOLEAN: {
            result = (bool) lua_toboolean(L, index);
        } break;
        case LUA_TNIL: {
            result = QVariant();
        } break;
        case LUA_TTABLE: {
            result = parseTable(index);
        } break;
        case LUA_TUSERDATA: {
            // 获取 userdata 的元表
            if (lua_getmetatable(L, index)) { // 获取 userdata 的元表
                // 从元表中获取 __name 字段
                lua_getfield(L, -1, "__name");
                if (lua_isstring(L, -1)) {
                    auto typeName = QString::fromUtf8(lua_tostring(L, -1));
                    lua_pop(L, 1); // 弹出 __name 值

                    if (typeName == QLUA_POINT_METATABLE_NAME) {
                        // 根据您的实际实现调整指针类型
                        QPointF *pointData = *(QPointF **) lua_touserdata(L, index);
                        // 或者如果是直接存储的对象：QPointF *pointData = (QPointF*) lua_touserdata(L, index);
                        result = QVariant::fromValue(*pointData);
                    } else if (typeName == QLUA_VECTOR3D_METATABLE_NAME) {
                        QVector3D *vectorData = *(QVector3D **) lua_touserdata(L, index);
                        // 或者：QVector3D *vectorData = (QVector3D*) lua_touserdata(L, index);
                        result = QVariant::fromValue(*vectorData);
                    }
                } else {
                    lua_pop(L, 1); // 弹出 nil 或非字符串值
                }
                lua_pop(L, 1); // 弹出元表
            }
            break;
        }
        default: {
            qWarning() << "unknown lua type" << lua_typename(L, type);
        } break;
        }
        return result;
    }

    lua_State *L;
};

QLua::QLua(QObject *parent)
    : QObject{parent}
    , d_ptr(new QLuaPrivate())
{
    Q_D(QLua);
    d->L = luaL_newstate();
    luaL_openlibs(d->L);
    qlpoint_openlib(d->L);
    qlvector3d_openlib(d->L);
    lua_pushcfunction(d->L, QLuaPrivate::luaPrint);
    lua_setglobal(d->L, "print");
}

QLua::~QLua()
{
    Q_D(QLua);
    lua_close(d->L);
}

void QLua::load(const QString &code)
{
    Q_D(QLua);

    // 将QString转换为UTF-8编码
    QByteArray codeUtf8 = code.toUtf8();

    auto err = luaL_loadstring(d->L, codeUtf8.data());
    if (err) {
        qWarning() << "failed load code with err: " << err;
        return;
    }
    lua_pcall(d->L, 0, 0, 0);
}

QVariant QLua::call(const QString &funcName, const QVariantList &args)
{
    Q_D(QLua);

    // 获取全局函数
    lua_getglobal(d->L, funcName.toUtf8().constData());

    // 检查是否是函数
    if (!lua_isfunction(d->L, -1)) {
        qWarning() << "Function" << funcName << "not found or not a function";
        lua_pop(d->L, 1); // 弹出非函数值
        return {};
    }

    // 将参数压入栈
    int numArgs = 0;
    for (const QVariant &arg : args) {
        d->pushValue(arg);
        numArgs++;
    }

    // 调用函数
    int status = lua_pcall(d->L, numArgs, LUA_MULTRET, 0);

    if (status != 0) {
        // 如果调用失败，获取错误信息
        QString errorMessage = QString::fromUtf8(lua_tostring(d->L, -1));
        qWarning() << "Failed to call function" << funcName << ":" << errorMessage;
        lua_pop(d->L, 1); // 弹出错误信息
        return {};
    }

    // 处理返回值
    int numResults = lua_gettop(d->L);
    if (numResults == 1) {
        auto result = d->toValue(-1);
        lua_pop(d->L, numResults);
        return result;
    }

    if (numResults > 1) {
        QVariantList results;
        for (int i = 1; i <= numResults; i++) {
            results.append(d->toValue(-numResults + i - 1));
        }
        // 弹出所有返回值
        lua_pop(d->L, numResults);
        return results;
    }
    return {};
}
