cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  100Commits
  VERSION 0.1
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(COMPILE_FLAGS -Wall -Wextra -Wpedantic -Wconversion)
endif()

if(MSVC)
  add_compile_options(/W4)
  add_compile_options(/permissive-)
endif()

option(ENABLE_SANITIZERS "Enable sanitizers." OFF)
if(ENABLE_SANITIZER)
  message("- SANITIZERS ENABLED")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(SANITIZER_AVAILABLE_AND_SET
        TRUE
        CACHE BOOL "" FORCE)
    mark_as_advanced(SANITIZER_AVAILABLE_AND_SET)
  endif()

  if(NOT SANITIZER_AVAILABLE_AND_SET)
    message(VERBOSE
            "The option ENABLE_SANITIZER is set but sanitizers not found.")
  else()
    set(SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
  endif()
endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
  message("- CLANG-TIDY ENABLED")
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;--use-color"
    )
  else()
    message(
      WARNING
        "The option ENABLE_CLANG_TIDY is set but clang-tidy executable is not found."
    )
  endif()
endif()

option(ENABLE_CPPCHECK "Enable cppcheck" OFF)
if(ENABLE_CPPCHECK)
  message("- CPPCHECK ENABLED")
  find_program(CPPCHECK_EXE NAMES "cppcheck")
  if(CPPCHECK_EXE)
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--inconclusive")
  else()
    message(
      WARNING
        "The option ENABLE_CPPCHECK is set but cppcheck executable is not found."
    )
  endif()
endif()

# add_subdirectory(external)
add_subdirectory(src)

# option(ENABLE_TESTS "Enable tests" Off)
# if(ENABLE_TESTS)
#   include(CTest)
#   enable_testing()
#   add_subdirectory(tests)
# endif()
