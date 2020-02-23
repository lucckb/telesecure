set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(telesecure VERSION 1.0 LANGUAGES CXX)
find_package(Td 1.6.0 REQUIRED)
find_package(Curses REQUIRED)
find_package(Readline REQUIRED)


# Sources list
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

# Executable
add_executable( ${PROJECT_NAME} ${SOURCES} )


# Options
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON) 


# Includes
target_include_directories( ${PROJECT_NAME} PRIVATE include )
target_include_directories( ${PROJECT_NAME} PRIVATE ${Readline_INCLUDE_DIR})

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Td::TdStatic)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Readline_LIBRARY})

