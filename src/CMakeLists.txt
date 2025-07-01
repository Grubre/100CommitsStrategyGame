# Main Executable

set(EXEC_NAME "main")

# Source files
set(SOURCES
    main.cpp
    camera.cpp
    terrain.cpp
    drawing.cpp
    systems.cpp
    minion.cpp
    tasks.cpp
    assets_loader.cpp
    homeless_functions.cpp
)

# Header files (for IDE support)
set(HEADERS
    camera.hpp
    terrain.hpp
    drawing.hpp
    systems.hpp
    minion.hpp
    tasks.hpp
    assets_loader.hpp
    homeless_functions.hpp
    common.hpp
    common_components.hpp
    models.hpp
    error.hpp
)

# Create executable
add_executable(${EXEC_NAME} ${SOURCES} ${HEADERS})

# Set target properties
set_target_properties(${EXEC_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "${PROJECT_NAME}"
)

# Link libraries
target_link_libraries(${EXEC_NAME} PRIVATE
    project_options
    project_warnings
    raylib
    EnTT::EnTT
    SimplexNoise
    raygui
    imgui
)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(${EXEC_NAME} PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${EXEC_NAME} PRIVATE
        pthread
        dl
        m
    )
elseif(WIN32)
    target_link_libraries(${EXEC_NAME} PRIVATE
        winmm
        gdi32
        opengl32
    )
endif()

# Compile definitions
target_compile_definitions(${EXEC_NAME} PRIVATE
    RESOURCES_DIR="${CMAKE_SOURCE_DIR}/resources"
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
)

# Copy resources to build directory
add_custom_command(TARGET ${EXEC_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:${EXEC_NAME}>/resources"
    COMMENT "Copying resources"
)

# Visual Studio specific settings
if(MSVC)
    # Set working directory for debugging
    set_target_properties(${EXEC_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${EXEC_NAME}>"
    )

    # Enable multiprocessor compilation
    target_compile_options(${EXEC_NAME} PRIVATE /MP)
endif()
