cmake_minimum_required(VERSION 3.28)

project(NBT LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Useful options for local dev and CI
option(NBT_BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_WARNINGS "Enable warnings" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" ON)


include(FetchContent)

FetchContent_Declare(
    BinaryStream
    GIT_REPOSITORY https://github.com/Cubix-Engine/BinaryStream.git
    GIT_TAG main
)

FetchContent_MakeAvailable(BinaryStream)


# Library
# Find source files. We keep the recursive glob for convenience but configure-depends
# ensures CMake will rescan when files are added/removed under `src/`.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_library(NBT STATIC ${SOURCES})
add_library(NBT::nbt ALIAS NBT)

target_link_libraries(NBT PUBLIC BinaryStream::BinaryStream)

target_compile_features(NBT PUBLIC cxx_std_23)
target_compile_options(NBT PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /Zc:preprocessor>
)

target_compile_definitions(NBT PRIVATE
    $<$<PLATFORM_ID:Windows>:NOMINMAX WIN32_LEAN_AND_MEAN>
    $<$<CONFIG:Debug>:NBT_DEBUG=1>
)

target_include_directories(NBT PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Compiler warnings
if (PROJECT_IS_TOP_LEVEL AND ENABLE_WARNINGS)
    include(cmake/CompilerWarnings.cmake)
    add_library(NBT_warnings INTERFACE)
    set_project_warnings(NBT_warnings ${WARNINGS_AS_ERRORS})
endif()


# Examples
if (NBT_BUILD_EXAMPLES)
    add_subdirectory(src-example)
endif()


# Install
install(TARGETS NBT EXPORT NBTTargets)

install(DIRECTORY src/nbt/
    DESTINATION include/nbt
)

install(EXPORT NBTTargets
    FILE NBTTargets.cmake
    NAMESPACE NBT::
    DESTINATION lib/cmake/NBT
)

# Tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)