cmake_minimum_required(VERSION 3.16)

project(luau-qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QLUAU_BUILD_TEST "Build Luau Qt unit tests" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

set(LUAU_BUILD_CLI OFF)
set(LUAU_BUILD_TESTS OFF)
add_subdirectory(deps/luau)

add_library(qluau SHARED
        include/qluau.h src/qluau.cpp
        src/qlpoint.cpp
        include/qllibs.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(qluau PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Luau.Compiler
        Luau.VM
)

if (QLUAU_BUILD_TEST)
    qt_add_executable(qluau_test
            test/test.h test/test.cpp
    )
    target_link_libraries(qluau_test PRIVATE
            qluau
            Qt${QT_VERSION_MAJOR}::Test
    )
    add_test(NAME qluau_test COMMAND qluau_test)
endif ()
