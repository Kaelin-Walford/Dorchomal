project(dorchomal)
cmake_minimum_required(VERSION 3.11)
# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#### Setup Directories ####
#Main output directory
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Ouput all DLLs from all libs into main build folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

#### Add External Dependencies ####
add_subdirectory("lib/SFML")
set(SFML_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/SFML/lib")

add_subdirectory("lib/box2d")
set(B2D_INCS "lib/box2d/include")
link_directories("${CMAKE_BINARY_DIR}/lib/box2d/lib")

#### level system ####
add_library(tile_level STATIC tile_level_loader/level_system.cpp)
target_include_directories(tile_level INTERFACE tile_level_loader)
target_link_libraries(tile_level sfml-graphics)

#### Engine ####
add_library(engine STATIC engine/game_system.cpp 
                        engine/renderer.cpp 
                        engine/ecm.cpp 
                        engine/physics.cpp
                        engine/audio_system.cpp)
target_include_directories(engine INTERFACE engine ${SFML_INCS} ${B2D_INCS} tile_level_loader)
target_link_libraries(engine sfml-graphics sfml-audio sfml-system box2d tile_level)

#### executable ####
add_executable(dorchomal 
    main.cpp
    components.cpp
    scenes.cpp
    b2_utils.cpp
    physics_cmps.cpp
    graphics_cmps.cpp
    collisions.cpp
    UI/ui_button.cpp
    UI/menu_scene.cpp
    UI/game_settings.cpp
    UI/ui_slider.cpp
    UI/ui_toggle.cpp
    UI/settings_scene.cpp
    UI/credits_scene.cpp
)
target_include_directories(dorchomal PRIVATE ${SFML_INCS} ${B2D_INCS} tile_level_loader engine)
target_link_libraries(dorchomal sfml-graphics sfml-audio sfml-system box2d tile_level engine)
set_target_properties(dorchomal 
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
)

add_custom_command(TARGET dorchomal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:box2d>
    $<TARGET_FILE_DIR:dorchomal>
)

add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} 
  -E copy_directory
    "${PROJECT_SOURCE_DIR}/resources"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
)

add_custom_command(TARGET dorchomal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/lib/SFML/extlibs/bin/x64"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

add_dependencies(dorchomal copy_resources)