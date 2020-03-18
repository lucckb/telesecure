set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)
project(telesecure VERSION 1.0 LANGUAGES CXX)
find_package(Td 1.6.0 REQUIRED)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
find_package(Readline REQUIRED)

#Boost header only
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


#Compiler definiton
add_compile_definitions(_XOPEN_SOURCE_EXTENDED)

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
target_include_directories( ${PROJECT_NAME} PRIVATE ${NCURSES_INCLUDE_DIRS})

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Td::TdStatic)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Readline_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_SYSTEM_LIBRARY})
