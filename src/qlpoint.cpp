//
// Created by ZekeXiao on 2025/4/9.
//

#include <QPointF>

#include <qllibs.h>

#include <lua.hpp>

int qlpoint_create(lua_State *L)
{
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);

    auto udata = (QPointF **) lua_newuserdata(L, sizeof(QPointF *));
    *udata = new QPointF(x, y);

    luaL_getmetatable(L, QLUA_POINT_METATABLE_NAME);
    lua_setmetatable(L, -2);

    return 1;
}

int qlpoint_gc(lua_State *L)
{
    auto udata = (QPointF **) luaL_checkudata(L, 1, QLUA_POINT_METATABLE_NAME);
    delete *udata;
    return 0;
}

int qlpoint_index(lua_State *L)
{
    auto udata = (QPointF **) luaL_checkudata(L, 1, QLUA_POINT_METATABLE_NAME);
    const char *field = luaL_checkstring(L, 2);

    if (strcmp(field, "x") == 0) {
        lua_pushnumber(L, (*udata)->x());
    } else if (strcmp(field, "y") == 0) {
        lua_pushnumber(L, (*udata)->y());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int qlpoint_newindex(lua_State *L)
{
    auto udata = (QPointF **) luaL_checkudata(L, 1, QLUA_POINT_METATABLE_NAME);
    const char *field = luaL_checkstring(L, 2);
    double value = luaL_checknumber(L, 3);

    if (strcmp(field, "x") == 0) {
        (*udata)->setX(value);
    } else if (strcmp(field, "y") == 0) {
        (*udata)->setY(value);
    }
    return 0;
}

void qlpoint_openlib(lua_State *L)
{
    luaL_newmetatable(L, QLUA_POINT_METATABLE_NAME);

    lua_pushstring(L, QLUA_POINT_METATABLE_NAME);
    lua_setfield(L, -2, "__name");

    lua_pushcfunction(L, qlpoint_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, qlpoint_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, qlpoint_newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, qlpoint_create);
    lua_setfield(L, -2, "create");
    lua_setglobal(L, "point");
}
