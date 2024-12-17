cmake_minimum_required(VERSION 3.10)
project(VersaUI)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Check platform
if(WIN32)
    message(STATUS "Using Win32 for window creation")
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    message(STATUS "Using Cocoa for window creation")
    add_definitions(-DPLATFORM_MACOS)
elseif(UNIX)
    message(STATUS "Using X11 for window creation")
    add_definitions(-DPLATFORM_LINUX)
endif()

# Add GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/GLFW/glfw-3.3.8)

# Add glad
add_library(glad STATIC
    ThirdParty/glad/src/gl.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Layout
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Platform
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Widget
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Core/Public/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Controls
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Window
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Docking
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Layout
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Property
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/UI/Public/Theme
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Editor
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/glfw-3.3.8/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/freetype/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include
)

# Disable FreeType dependencies we don't need
set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_DISABLE_BROTLI TRUE)

# Add third-party dependencies
add_subdirectory(ThirdParty/freetype)
add_subdirectory(ThirdParty/glm)

# Find required packages
find_package(OpenGL REQUIRED)

# Core library source files
file(GLOB_RECURSE CORE_SOURCES
    "Source/Runtime/Core/Private/*.cpp"
    "Source/Runtime/Core/Public/*.h"
)

# UI library source files
file(GLOB_RECURSE UI_SOURCES
    "Source/Runtime/UI/Private/*.cpp"
    "Source/Runtime/UI/Public/*.h"
)

# Framework library source files
file(GLOB_RECURSE FRAMEWORK_SOURCES
    "Source/Runtime/Framework/Private/*.cpp"
    "Source/Runtime/Framework/Public/*.h"
)

# If Framework sources are empty, add a dummy source file
if(NOT FRAMEWORK_SOURCES)
    set(DUMMY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime/Framework/Private/dummy.cpp")
    file(WRITE "${DUMMY_FILE}" "")
    list(APPEND FRAMEWORK_SOURCES "${DUMMY_FILE}")
endif()

# Editor source files
file(GLOB_RECURSE EDITOR_SOURCES
    "Source/Editor/VersaEditor/Private/*.cpp"
    "Source/Editor/VersaEditor/Public/*.h"
)

# Launch source files
file(GLOB_RECURSE LAUNCH_SOURCES
    "Source/Launch/Private/*.cpp"
    "Source/Launch/Public/*.h"
)

# Source groups for better organization in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${CORE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${UI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${FRAMEWORK_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${EDITOR_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${LAUNCH_SOURCES})

# Create Core library
add_library(VersaUI_Core SHARED ${CORE_SOURCES})
target_include_directories(VersaUI_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime
)
target_link_libraries(VersaUI_Core PUBLIC
    glad
    glfw
    OpenGL::GL
    freetype
)
target_compile_definitions(VersaUI_Core PRIVATE 
    UI_EXPORTS
    GLFW_INCLUDE_NONE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# Create Framework library
add_library(VersaUI_Framework SHARED ${FRAMEWORK_SOURCES})
target_include_directories(VersaUI_Framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime
)
target_link_libraries(VersaUI_Framework PUBLIC
    VersaUI_Core
    freetype
    ${OPENGL_LIBRARIES}
)
target_compile_definitions(VersaUI_Framework PRIVATE UI_EXPORTS)

# Create UI library
add_library(VersaUI_UI STATIC ${UI_SOURCES})
target_include_directories(VersaUI_UI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime
)
target_link_libraries(VersaUI_UI PUBLIC
    VersaUI_Framework
)

# Create editor library
add_library(VersaUI_Editor STATIC ${EDITOR_SOURCES})
target_include_directories(VersaUI_Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Editor
)
target_link_libraries(VersaUI_Editor PUBLIC
    VersaUI_UI
)

# Create main executable
add_executable(${PROJECT_NAME} ${LAUNCH_SOURCES})
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Launch
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        VersaUI_Editor
)
