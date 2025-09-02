//
// Created by ZekeXiao on 2025/4/9.
//

#include <QVector3D>

#include <qllibs.h>

#include "lua.hpp"

int qlvecotr3d_create(lua_State *L)
{
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    double z = luaL_checknumber(L, 3);

    auto udata = (QVector3D **) lua_newuserdata(L, sizeof(QVector3D *));
    *udata = new QVector3D(x, y, z);

    luaL_getmetatable(L, QLUA_VECTOR3D_METATABLE_NAME);
    lua_setmetatable(L, -2);

    return 1;
}

int qlvector3d_gc(lua_State *L)
{
    auto udata = (QVector3D **) luaL_checkudata(L, 1, QLUA_VECTOR3D_METATABLE_NAME);
    delete *udata;
    return 0;
}

int qlvector3d_index(lua_State *L)
{
    auto udata = (QVector3D **) luaL_checkudata(L, 1, QLUA_VECTOR3D_METATABLE_NAME);
    const char *field = luaL_checkstring(L, 2);

    if (strcmp(field, "x") == 0) {
        lua_pushnumber(L, (*udata)->x());
    } else if (strcmp(field, "y") == 0) {
        lua_pushnumber(L, (*udata)->y());
    } else if (strcmp(field, "z") == 0) {
        lua_pushnumber(L, (*udata)->z());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int qlvector3d_newindex(lua_State *L)
{
    auto udata = (QVector3D **) luaL_checkudata(L, 1, QLUA_VECTOR3D_METATABLE_NAME);
    const char *field = luaL_checkstring(L, 2);
    double value = luaL_checknumber(L, 3);

    if (strcmp(field, "x") == 0) {
        (*udata)->setX(value);
    } else if (strcmp(field, "y") == 0) {
        (*udata)->setY(value);
    } else if (strcmp(field, "z") == 0) {
        (*udata)->setZ(value);
    }
    return 0;
}

void qlvector3d_openlib(lua_State *L)
{
    luaL_newmetatable(L, QLUA_VECTOR3D_METATABLE_NAME);

    lua_pushstring(L, QLUA_VECTOR3D_METATABLE_NAME);
    lua_setfield(L, -2, "__name");

    lua_pushcfunction(L, qlvector3d_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, qlvector3d_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, qlvector3d_newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, qlvecotr3d_create);
    lua_setfield(L, -2, "create");
    lua_setglobal(L, "vector");
}
