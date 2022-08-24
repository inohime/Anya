cmake_minimum_required(VERSION 3.20)
project(Anya VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
# platform flags
if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_EXE_LINKER_FLAGS /SUBSYSTEM:CONSOLE)
        message("debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_EXE_LINKER_FLAGS /SUBSYSTEM:WINDOWS)
        message("release")
    endif()
endif()
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resource.rc)

file(GLOB SOURCES 
    "src/*.cpp" 
    "src/*.hpp"
)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${APP_ICON})

add_subdirectory("deps/NFD")

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    SDL2::SDL2 
    SDL2::SDL2main 
    SDL2::SDL2_image 
    SDL2::SDL2_ttf 
    dwmapi
    nfd
)