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
    include/pt/core/parallel.h
    include/pt/core/light.h
    include/pt/core/sampling.h
    include/pt/core/bxdf.h
    include/pt/core/bsdf.h
    include/pt/core/material.h
    include/pt/core/coordinate.h
    include/pt/core/visibilitytester.h

    include/pt/math/math.h
    include/pt/math/vector2.h
    include/pt/math/vector3.h
    include/pt/math/matrix4.h
    include/pt/math/bounds2.h
    include/pt/math/bounds3.h
    
    include/pt/lights/diffuse.h
    include/pt/lights/infinite.h
    include/pt/lights/point.h

    include/pt/utils/objloader.h
    include/pt/utils/plyloader.h
)

set(PT_CORE_SRCS
    src/core/parallel.cpp
    src/core/fresnel.cpp
    src/core/interaction.cpp
    src/core/visibilitytester.cpp
    src/math/matrix4.cpp
    src/integrators/path.cpp
    src/accelerators/bvh.cpp
)

include_directories(include)

add_library(pt STATIC
    ${PT_CORE_HEADERS}
    ${PT_CORE_SRCS}
)

add_executable(bunny src/main/bunny.cpp)
target_link_libraries(bunny pt pthread)

add_executable(ajax src/main/ajax.cpp)
target_link_libraries(ajax pt pthread)

add_executable(point src/main/point.cpp)
target_link_libraries(point pt pthread)

add_executable(cbox src/main/cbox.cpp)
target_link_libraries(cbox pt pthread)

add_executable(glass src/main/glass.cpp)
target_link_libraries(glass pt pthread)

add_executable(table src/main/table.cpp)
target_link_libraries(table pt pthread)
