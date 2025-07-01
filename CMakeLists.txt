cmake_minimum_required(VERSION 3.20)

# Set policies for modern CMake behavior
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW) # MSVC debug information format
endif()

# Enable compile commands export for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project definition
project(
    100CommitsStrategyGame
    VERSION 0.1.0
    DESCRIPTION "A strategy game built with C++ and Raylib"
    LANGUAGES CXX
)

# Only set these properties if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # C++ Standard
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Position independent code
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # Use folders in IDE
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Set default build type if not specified
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    endif()

    # Configure MSVC runtime library
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        # Set startup project for Visual Studio
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
    endif()
endif()

# Create interface libraries for common settings
add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion
    )

    # Debug/Release specific flags
    target_compile_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )

elseif(MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4 /permissive- /utf-8
    )

    # Debug/Release specific flags
    target_compile_options(project_options INTERFACE
        $<$<CONFIG:Debug>:/Od /Zi /RTC1 /DDEBUG>
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(project_options INTERFACE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Options
option(ENABLE_SANITIZERS "Enable sanitizers (Debug builds only)" OFF)

# Sanitizers (Debug builds only)
if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>
    )
    target_link_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

# Add subdirectories
add_subdirectory(external)
add_subdirectory(src)

# Summary
message(STATUS "")
message(STATUS "=================== Configuration ===================")
message(STATUS "Project:      ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:     ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Sanitizers:   ${ENABLE_SANITIZERS}")
message(STATUS "=====================================================")
message(STATUS "")
