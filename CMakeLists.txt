cmake_minimum_required(VERSION 2.8.8)
project(bitstream)
include(GNUInstallDirs)

add_definitions(-std=${CXX_STD})
file(GLOB sources src/*.cc)
include_directories(PUBLIC include PRIVATE src)
add_library(lib${PROJECT_NAME}_object OBJECT ${sources})
set_target_properties(lib${PROJECT_NAME}_object PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(lib${PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:lib${PROJECT_NAME}_object>)
add_library(lib${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:lib${PROJECT_NAME}_object>)
set_target_properties(lib${PROJECT_NAME}_shared lib${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(TARGETS lib${PROJECT_NAME}_shared lib${PROJECT_NAME}_static
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(test)
endif()

