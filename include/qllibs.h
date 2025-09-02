//
// Created by ZekeXiao on 2025/4/10.
//

#ifndef QLLIBS_H
#define QLLIBS_H

#include "lua.hpp"
#include "qluaglobal.h"

#define QLUA_POINT_METATABLE_NAME "QPointFMetaTable"

// QPointF
QLUA_EXPORT void qlpoint_openlib(lua_State *L);

#define QLUA_VECTOR3D_METATABLE_NAME "QVector3DMetaTable"

// QVector3D
QLUA_EXPORT void qlvector3d_openlib(lua_State *L);

#endif //QLLIBS_H
