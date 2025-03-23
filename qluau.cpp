#include "qluau.h"

#include <Luau/Compiler.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

#include <QDebug>
#include <QVector3D>

class QLuauPrivate
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
            } else if (lua_isvector(L, i)) {
                auto v = lua_tovector(L, i);
                output += QString("(%1, %2, %3)").arg(v[0]).arg(v[1]).arg(v[2]);
            } else if (lua_isnil(L, i)) {
                output += "nil";
            } else {
                output += QString("<%1>").arg(lua_typename(L, lua_type(L, i)));
            }
        }

        qDebug() << "[Lua]" << output;
        return 0;
    }

    void pushValue(const QVariant &value)
    {
        switch (value.typeId()) {
        case QMetaType::Int:
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
            auto vec = value.value<QVector3D>();
            lua_pushvector(L, vec.x(), vec.y(), vec.z());
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
        default: {
            qWarning() << "unkonwn qt type" << value.typeName();
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
        case LUA_TVECTOR: {
            auto v = lua_tovector(L, index);
            result = QVector3D(v[0], v[1], v[2]);
        } break;
        case LUA_TNIL: {
            result = QVariant();
        } break;
        case LUA_TTABLE: {
            if (index < 0) {
                index = lua_gettop(L) + index + 1;
            }

            auto len = lua_objlen(L, index);

            bool isList = true;
            lua_pushnil(L); // 第一个 key
            while (lua_next(L, index) != 0) {
                // 检查 key 是否为数字
                if (lua_type(L, -2) != LUA_TNUMBER) {
                    isList = false;
                    lua_pop(L, 2); // 弹出 key 和 value
                    break;
                }

                // 获取数字 key
                int key = lua_tointeger(L, -2);

                // 检查 key 是否为正整数且不大于数组长度
                if (key <= 0 || key > len || key != (int) lua_tonumber(L, -2)) {
                    isList = false;
                    lua_pop(L, 2); // 弹出 key 和 value
                    break;
                }

                lua_pop(L, 1); // 只弹出 value，保留 key 用于下一次迭代
            }

            if (isList) {
                QVariantList values;
                for (int i = 1; i <= len; i++) {
                    lua_rawgeti(L, index, i);
                    values.push_back(toValue(-1));
                    lua_pop(L, 1);
                }
                result = values;
            } else {
                // 处理字典情况
                QVariantMap dict;

                // 遍历表中的所有键值对
                lua_pushnil(L); // 第一个 key
                while (lua_next(L, index) != 0) {
                    // 复制 key，避免 lua_tostring 可能修改原始值导致 lua_next 行为异常
                    lua_pushvalue(L, -2);

                    // 获取键（转换为字符串作为 QVariantMap 的键）
                    QString key;
                    switch (lua_type(L, -1)) {
                    case LUA_TSTRING:
                        key = QString::fromUtf8(lua_tostring(L, -1));
                        break;
                    case LUA_TNUMBER:
                        key = QString::number(lua_tonumber(L, -1));
                        break;
                    case LUA_TBOOLEAN:
                        key = lua_toboolean(L, -1) ? "true" : "false";
                        break;
                    default: {
                        auto type = lua_typename(L, lua_type(L, -1));
                        qWarning() << "unkonwn lua table key" << type;
                        key = QString("<%s: %p>")
                                  .arg(QString::fromUtf8(type), *(ptrdiff_t *) lua_topointer(L, -1));

                    } break;
                    }

                    // 弹出复制的 key
                    lua_pop(L, 1);

                    // 获取值并添加到字典
                    QVariant value = toValue(-1);
                    dict[key] = value;

                    // 弹出值，保留键用于下一次迭代
                    lua_pop(L, 1);
                }
                result = dict;
            }
        } break;
        default: {
            qWarning() << "unkonwn lua type" << lua_typename(L, type);
        } break;
        }
        return result;
    }
    lua_State *L;
};

QLuau::QLuau(QObject *parent)
    : QObject{parent}
    , d_ptr(new QLuauPrivate())
{
    Q_D(QLuau);
    d->L = luaL_newstate();
    luaL_openlibs(d->L);
    lua_pushcfunction(d->L, QLuauPrivate::luaPrint, "print");
    lua_setglobal(d->L, "print");
}

QLuau::~QLuau()
{
    Q_D(QLuau);
    lua_close(d->L);
}

void QLuau::load(const QString &code)
{
    Q_D(QLuau);

    // 将QString转换为UTF-8编码
    QByteArray codeUtf8 = code.toUtf8();

    // 编译代码
    Luau::CompileOptions options;
    std::string bytecode = Luau::compile(codeUtf8.constData(), options);

    // 加载编译后的代码
    int status = luau_load(d->L, "=code", bytecode.data(), bytecode.size(), 0);

    if (status != 0) {
        // 如果加载失败，获取错误信息
        QString errorMessage = QString::fromUtf8(lua_tostring(d->L, -1));
        qWarning() << "Failed to load Luau code:" << errorMessage;
        lua_pop(d->L, 1); // 弹出错误信息
        return;
    }

    // 执行加载的代码
    status = lua_pcall(d->L, 0, 0, 0);
    if (status != 0) {
        // 如果执行失败，获取错误信息
        QString errorMessage = QString::fromUtf8(lua_tostring(d->L, -1));
        qWarning() << "Failed to execute Luau code:" << errorMessage;
        lua_pop(d->L, 1); // 弹出错误信息
    }
}

QVariantList QLuau::call(const QString &funcName, const QVariantList &args)
{
    Q_D(QLuau);

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
    if (numResults > 0) {
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
