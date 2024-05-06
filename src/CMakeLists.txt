set(EXEC_NAME "main")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")

add_executable(${EXEC_NAME} main.cpp camera.cpp terrain.cpp drawing.cpp
                            systems.cpp minion.cpp tasks.cpp assets_loader.cpp
                            homeless_functions.cpp)

                        target_compile_definitions(${EXEC_NAME} PRIVATE RESOURCES_DIR="${RESOURCES_DIR}")

set(MAIN_FLAGS ${COMPILE_FLAGS})

add_custom_command(TARGET ${EXEC_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${EXEC_NAME}>/resources)

if(SANITIZER_AVAILABLE_AND_SET)
  set(MAIN_FLAGS ${MAIN_FLAGS} ${SANITIZER_FLAGS})
  target_link_libraries(${EXEC_NAME} ${SANITIZER_FLAGS})
endif()

target_compile_options(${EXEC_NAME} PRIVATE ${MAIN_FLAGS})

target_link_libraries(${EXEC_NAME} Entt raylib Simplex Raygui)

# raylib OSX dependencies
if(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
