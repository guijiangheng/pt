set(ILMBASE_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
set(OPENEXR_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
set(OPENEXR_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
set(ILMBASE_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)

add_subdirectory(openexr)

set(OPENEXR_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/openexr/IlmBase/Imath"
    "${CMAKE_CURRENT_SOURCE_DIR}/openexr/IlmBase/Half"
    "${CMAKE_CURRENT_SOURCE_DIR}/openexr/IlmBase/Iex"
    "${CMAKE_CURRENT_SOURCE_DIR}/openexr/OpenEXR/IlmImf"
    "${CMAKE_CURRENT_BINARY_DIR}/openexr/IlmBase/config"
    "${CMAKE_CURRENT_BINARY_DIR}/openexr/OpenEXR/config"
    PARENT_SCOPE
)

set(OPENEXR_LIBS IlmImf Imath Half PARENT_SCOPE)

add_library(lodepng STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/lodepng.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/lodepng.cpp"
)

target_compile_features(lodepng PRIVATE cxx_std_17)
set(LODEPNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
