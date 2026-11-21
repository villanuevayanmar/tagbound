cmake_minimum_required(VERSION 3.20)
project(tagbound LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(PLATFORM Web CACHE STRING "" FORCE)
set(OPENGL_VERSION "ES 2.0" CACHE STRING "" FORCE)

include(FetchContent)
FetchContent_Declare(raylib URL https://codeload.github.com/raysan5/raylib/zip/refs/tags/5.5)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp src/systems.cpp src/game.cpp src/render.cpp)
target_include_directories(game PRIVATE src)
target_link_libraries(game PRIVATE raylib)
target_compile_options(game PRIVATE -O3 -Wall -Wextra)
set_target_properties(game PROPERTIES SUFFIX ".js" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

if(EMSCRIPTEN)
 target_link_options(game PRIVATE
  "-sUSE_GLFW=3" "-sMIN_WEBGL_VERSION=1" "-sMAX_WEBGL_VERSION=2"
  "-sALLOW_MEMORY_GROWTH=1" "-sINITIAL_MEMORY=134217728" "-sSTACK_SIZE=2097152"
  "-sEXPORTED_RUNTIME_METHODS=ccall,cwrap" "-sEXPORTED_FUNCTIONS=_main"
  "-sENVIRONMENT=web,worker" "-sASSERTIONS=0" "-O3")
endif()
