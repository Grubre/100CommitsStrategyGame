add_library(Entt INTERFACE)

target_include_directories(Entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/entt/include)

# Turns off warnings for Entt
set_target_properties(Entt PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Entt,INTERFACE_INCLUDE_DIRECTORIES>)

add_library(Simplex STATIC ${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/SimplexNoise.cpp)

target_include_directories(Simplex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include)

set_target_properties(Simplex PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Simplex,INTERFACE_INCLUDE_DIRECTORIES>)
