cmake_minimum_required(VERSION 3.0)
project(pt CXX)

set(CMAKE_CXX_STANDARD 17)

set(PT_CORE_HEADERS
    include/pt/pt.h
    include/pt/core/ray.h
    include/pt/core/interaction.h
    include/pt/core/shape.h
    include/pt/math/math.h
    include/pt/math/vector3.h
    include/pt/math/matrix4.h
    include/pt/math/frame.h
)

set(PT_CORE_SRCS
    src/math/matrix4.cpp
)

include_directories(include)

add_library(pt STATIC
    ${PT_CORE_HEADERS}
    ${PT_CORE_SRCS}
)

add_executable(main src/main/main.cpp)
target_link_libraries(main pt)