cmake_minimum_required(VERSION 3.0)
project(pt CXX)

set(CMAKE_CXX_STANDARD 17)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

set(PT_CORE_HEADERS
    include/pt/pt.h
    
    include/pt/core/ray.h
    include/pt/core/frame.h
    include/pt/core/interaction.h
    include/pt/core/film.h
    include/pt/core/camera.h
    include/pt/core/shape.h
    include/pt/core/sampler.h
    include/pt/core/scene.h
    include/pt/core/camera.h
    include/pt/core/scene.h
    include/pt/core/integrator.h

    include/pt/math/math.h
    include/pt/math/vector2.h
    include/pt/math/vector3.h
    include/pt/math/matrix4.h
    include/pt/math/bounds2.h
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