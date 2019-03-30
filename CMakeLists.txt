cmake_minimum_required(VERSION 3.0)
project(pt LANGUAGES CXX)

if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/openexr")
    message(FATAL_ERROR "The OpenEXR submodule directory is missing! "
        "You probably did not clone the project with --recursive. It is possible to recover by running "
        "\"git submodule update --init --recursive\"")
endif()

add_subdirectory(ext)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;MinSzieRel;RelWithDebInfo")
endif()

add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter -Werror)

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
    include/pt/core/distrib.h
    include/pt/core/sampling.h
    include/pt/core/filter.h
    include/pt/core/bxdf.h
    include/pt/core/bsdf.h
    include/pt/core/material.h
    include/pt/core/coordinate.h
    include/pt/core/visibilitytester.h
    include/pt/core/texture.h

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

    include/pt/filters/box.h
    include/pt/filters/triangle.h
    include/pt/filters/gaussian.h

    include/pt/textures/mix.h
    include/pt/textures/scale.h
    include/pt/textures/image.h
    include/pt/textures/constant.h
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

find_package(Threads)

add_library(imageio STATIC
    include/pt/utils/imageio.h
    src/utils/imageio.cpp
)

# OpenEXR doesn't support c++17, compile it alone use c++14 and -Wno-deprecated flags
target_include_directories(
    imageio PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${OPENEXR_INCLUDE_DIRS}
)

target_compile_features(imageio PRIVATE cxx_std_14)
target_compile_options(imageio PRIVATE -Wno-deprecated)
target_link_libraries(imageio PRIVATE ${OPENEXR_LIBS} stdc++fs)

add_library(pt STATIC
    ${PT_CORE_HEADERS}
    ${PT_CORE_SRCS}
)

target_compile_features(pt PRIVATE cxx_std_17)
target_include_directories(pt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(pt PRIVATE ${CMAKE_THREAD_LIBS_INIT} imageio)
    
add_executable(bunny src/main/bunny.cpp)
add_executable(ajax src/main/ajax.cpp)
add_executable(point src/main/point.cpp)
add_executable(cbox src/main/cbox.cpp)
add_executable(glass src/main/glass.cpp)
add_executable(table src/main/table.cpp)
add_executable(dragon src/main/dragon.cpp)
add_executable(distribtest src/main/distribtest.cpp)

set(PT_ALL_EXES bunny ajax point cbox glass table dragon distribtest)
foreach(target ${PT_ALL_EXES})
    target_link_libraries(${target} PRIVATE pt)
    target_compile_features(${target} PRIVATE cxx_std_17)
endforeach()
