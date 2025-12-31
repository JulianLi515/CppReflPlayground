cmake_minimum_required(VERSION 3.20)
project(ReflectSystem)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE LIB_SOURCES
        "CppReflPlayground/include/*.h"
        "CppReflPlayground/src/*.cpp"
)

add_library(my_reflect STATIC ${LIB_SOURCES}
        CppReflPlayground/src/dynamic_refl/Vector.cpp
        CppReflPlayground/include/dynamic_refl/Vector.h)

target_include_directories(my_reflect PUBLIC CppReflPlayground/include)

add_executable(my_test CppReflPlayground/tests/main.cpp)

target_link_libraries(my_test PRIVATE my_reflect)

target_include_directories(my_test PRIVATE CppReflPlayground/include)