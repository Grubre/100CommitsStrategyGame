# External Dependencies

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# =============================================================================
# RAYLIB
# =============================================================================

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)

if(NOT raylib_FOUND)
    message(STATUS "Fetching Raylib ${RAYLIB_VERSION}...")

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_VERSION}
        GIT_SHALLOW TRUE
    )

    # Configure raylib
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(raylib)

    # Suppress warnings from raylib
    set_target_properties(raylib PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()

# =============================================================================
# ENTT
# =============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/entt/include/entt.hpp")
    add_library(EnTT INTERFACE)
    target_include_directories(EnTT INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/entt/include")

    # Suppress warnings
    set_target_properties(EnTT PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/entt/include"
    )

    add_library(EnTT::EnTT ALIAS EnTT)
else()
    message(FATAL_ERROR "EnTT submodule not found. Run: git submodule update --init --recursive")
endif()

# =============================================================================
# SIMPLEX NOISE
# =============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/SimplexNoise.cpp")
    add_library(SimplexNoise STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/SimplexNoise.cpp"
    )

    target_include_directories(SimplexNoise PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include"
    )

    # Suppress warnings
    set_target_properties(SimplexNoise PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include"
    )

    target_link_libraries(SimplexNoise PRIVATE project_options project_warnings)
else()
    message(FATAL_ERROR "SimplexNoise submodule not found. Run: git submodule update --init --recursive")
endif()

# =============================================================================
# RAYGUI
# =============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/raygui/include/raygui.h")
    add_library(raygui INTERFACE)

    target_include_directories(raygui INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/raygui/include"
    )

    # Suppress warnings
    set_target_properties(raygui PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/raygui/include"
    )

    target_link_libraries(raygui INTERFACE raylib)
else()
    message(FATAL_ERROR "Raygui submodule not found. Run: git submodule update --init --recursive")
endif()

# =============================================================================
# IMGUI
# =============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp" AND
   EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rlimgui/rlImGui.cpp")

    add_library(imgui STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/rlimgui/rlImGui.cpp"
    )

    target_include_directories(imgui PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/rlimgui"
    )

    # Suppress warnings
    set_target_properties(imgui PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui;${CMAKE_CURRENT_SOURCE_DIR}/rlimgui"
    )

    target_link_libraries(imgui PUBLIC raylib)
    target_link_libraries(imgui PRIVATE project_options)
else()
    message(FATAL_ERROR "ImGui submodules not found. Run: git submodule update --init --recursive")
endif()

# =============================================================================
# ALIASES FOR COMPATIBILITY
# =============================================================================

if(NOT TARGET Entt)
    add_library(Entt ALIAS EnTT)
endif()

if(NOT TARGET Simplex)
    add_library(Simplex ALIAS SimplexNoise)
endif()

if(NOT TARGET Raygui)
    add_library(Raygui ALIAS raygui)
endif()

if(NOT TARGET Imgui)
    add_library(Imgui ALIAS imgui)
endif()

message(STATUS "External dependencies configured")
