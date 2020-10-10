cmake_minimum_required(VERSION 3.12)
project(lens)

if (APPLE)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)

include_directories(include
                    include/common
                    ${SDL2_INCLUDE_DIR})

file(GLOB PROJECT_HEADERS include/*.hpp
                          include/*.h)
file(GLOB PROJECT_SOURCES src/*.cpp)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          README.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

if(WIN32)
file(COPY "${SDL2_INCLUDE_DIR}/../../bin/SDL2.dll"
            DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
endif()

source_group("include" FILES ${PROJECT_HEADERS})
source_group("src" FILES ${PROJECT_SOURCES})

add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} SDL2::Main)
if (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} 
        "-framework CoreAudio" 
        "-framework AudioToolbox"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreVideo"
        "-framework ForceFeedback"
        "-framework IOKit"
        "-framework CarBon"
        "-framework AppKit"
        iconv
    )
endif()

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )


