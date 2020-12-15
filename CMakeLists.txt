# Modar Nasser - 9/10/2020

# This CMakeLists.txt will automatically generate build
# solution for a NasNas game. 

# Note: Everytime you add a new file to the project or 
#       update an asset, be sure to rerun `cmake ..`
#       to update the project.



###### User Configuration #####################################################
# Project Name, which is also the ouput binary file name
set(PROJECT_NAME "NasNas_Game_Template")
# Assets directory path, will be copied in bin when cmake is called
set(ASSETS_DIR_NAME "")
###############################################################################


cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

project(${PROJECT_NAME})


###### Load NasNas  ###########################################################
add_subdirectory(NasNas)
###############################################################################


###### Find Game source files #################################################
file(GLOB_RECURSE Game_Src src/*.cpp src/*.hpp)
file(GLOB_RECURSE Game_Inc include/*.hpp)
# copy assets directory near binary output
if (ASSETS_DIR_NAME)
    file(COPY ${ASSETS_DIR_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
###############################################################################


###### Create the executabe target and link it ################################
add_executable(${PROJECT_NAME}
    ${Game_Src} ${Game_Inc}
    main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src include NasNas/include)
target_link_libraries(${PROJECT_NAME} NasNas sfml-graphics sfml-audio)
###############################################################################
