# Modar Nasser - 9/10/2020

# This CMakeLists.txt will automatically generate build
# solution for a NasNas game. 

# Note: Everytime you add a new file to the project or 
#       update an asset, be sure to rerun `cmake ..`
#       to update the project.



###### User Configuration #####################################################
# Project Name, which is also the ouput binary file name
set(PROJECT_NAME "CyberSlayer")
# Assets directory path, will be copied in bin when cmake is called
set(ASSETS_DIR_NAME "assets")
###############################################################################


cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin)

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
target_link_libraries(${PROJECT_NAME} NasNas)
###############################################################################

if(NASNAS_BUILD_RESLIB)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DNS_RESLIB)
endif()

if(NASNAS_BUILD_ECS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DNS_ECS)
endif()

if(NASNAS_BUILD_TILEMAPPING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DNS_TILEMAPPING)
endif()

if(NASNAS_BUILD_UI)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DNS_UI)
endif()
