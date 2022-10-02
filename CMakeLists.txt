cmake_minimum_required(VERSION 3.14...3.24)

project(GRay VERSION 0.0.1
             DESCRIPTION "A Ray Tracing Engine which will conquer the world"
             LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(CTest)

    find_package(Doxygen)
    if(doxygen_FOUND)
        message(STATUS "Doxygen found, would build docs here")#add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

include(FetchContent)

#set(CMAKE_CXX_FLAGS "-Wall -Wextra") #Enable before publishing
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(src)
add_subdirectory(apps)

#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR GRAY_BUILD_TESTING) AND BUILD_TESTING)
#    add_subdirectory(tests)
#endif()