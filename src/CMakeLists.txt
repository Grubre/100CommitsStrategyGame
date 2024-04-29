set(EXEC_NAME "main")

find_package(raylib 4.5 REQUIRED)

add_executable(${EXEC_NAME} main.cpp camera.cpp terrain.cpp drawing.cpp
                            systems.cpp minion.cpp tasks.cpp)

set(MAIN_FLAGS ${COMPILE_FLAGS})

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
