set(EXEC_NAME "main")

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
	include(FetchContent)
	FetchContent_Declare(
		raylib
		URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
	)
	FetchContent_GetProperties(raylib)
	if (NOT raylib_POPULATED)
		set(FETCHCONTENT_QUIET NO)
		FetchContent_Populate(raylib)
		set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
	endif()
endif()

add_executable(${EXEC_NAME} main.cpp camera.cpp terrain.cpp drawing.cpp
                            systems.cpp minion.cpp tasks.cpp)

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
