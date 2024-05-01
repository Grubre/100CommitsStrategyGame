set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# this magic formula silences warnings from raylib
set_target_properties(raylib PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

add_library(Entt INTERFACE)
target_include_directories(Entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/entt/include)
set_target_properties(Entt PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Entt,INTERFACE_INCLUDE_DIRECTORIES>)
# Turns off warnings for Entt /\


add_library(Simplex STATIC ${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/SimplexNoise.cpp)
target_include_directories(Simplex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include)
set_target_properties(Simplex PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Simplex,INTERFACE_INCLUDE_DIRECTORIES>)


add_library(Raygui INTERFACE)
target_include_directories(Raygui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/raygui/include)
set_target_properties(Raygui PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Raygui,INTERFACE_INCLUDE_DIRECTORIES>)


