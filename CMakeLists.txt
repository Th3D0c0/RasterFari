cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Set the project name and specify C as the language
project(RasterFari C)

# 1. Add the include folder so headers (.h) can be found
include_directories(include)

# 2. Gather all source files (.c) from the src folder
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

set(OpenGL_GL_Preference GLVND)
if(WIN32)
    # Windows: Needs GDI for windows and OpenGL32 for graphics
    target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 opengl32 winmm)

elseif(APPLE)
    # macOS: Uses Cocoa framework and native OpenGL/CoreVideo
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(OPENGL_FRAMEWORK OpenGL)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${COCOA_FRAMEWORK} 
        ${OPENGL_FRAMEWORK} 
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )

elseif(UNIX AND NOT APPLE)
    # Linux/BSD: Needs X11 and GL
    # find_package is the "clean" way to find these on Linux
    find_package(X11 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_compile_options(${PROJECT_NAME} PRIVATE -fgnu89-inline)
    
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        X11 
        ${OPENGL_LIBRARIES} 
        m    # Math library
        dl   # Dynamic linking library
    )
    
    # If using Xrandr (for resolution switching/multi-monitor)
    target_link_libraries(${PROJECT_NAME} PRIVATE Xrandr)
endif()
