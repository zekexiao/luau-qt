//
// Created by ZekeXiao on 2025/5/8.
//

#ifndef QLUAGLOBAL_H
#define QLUAGLOBAL_H

#ifdef QLUA_LIBRARY
#define QLUA_EXPORT Q_DECL_EXPORT
#else
#define QLUA_EXPORT Q_DECL_IMPORT
#endif

#endif //QLUAGLOBAL_H
