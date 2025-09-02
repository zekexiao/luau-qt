cmake_minimum_required(VERSION 3.16)

project(qlua VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QLUA_BUILD_TEST "Build Luau Qt unit tests" ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(LUA_BUILD_AS_CXX OFF)
set(LUA_ENABLE_TESTING OFF)
set(LUA_ENABLE_SHARED OFF)
add_subdirectory(deps/cmake-lua)

add_library(qlua STATIC
        include/qlua.h src/qlua.cpp
        src/qlpoint.cpp
        src/qlvector3d.cpp
        include/qllibs.h
        include/qluaglobal.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(qlua PRIVATE -DQLUA_LIBRARY)
target_link_libraries(qlua PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Lua::Library
)

if (QLUA_BUILD_TEST)
    qt_add_executable(qlua_test
            test/test.h test/test.cpp
    )
    target_link_libraries(qlua_test PRIVATE
            qlua
            Qt${QT_VERSION_MAJOR}::Test
    )
    add_test(NAME qlua_test COMMAND qlua_test)
endif ()
