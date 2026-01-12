cmake_minimum_required(VERSION 3.10)

project(learn_cpp_ranges)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_AR "/opt/homebrew/opt/llvm/bin/llvm-ar" CACHE FILEPATH "Path to ar" FORCE)
# set(CMAKE_RANLIB "/opt/homebrew/opt/llvm/bin/llvm-ranlib" CACHE FILEPATH "Path to ranlib" FORCE)

if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    # llvm coverage test
    message(STATUS "Enabling coverage instrumentation")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Compiling Release Version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DREDIRECT_INPUT")

elseif (CMAKE_BUILD_TYPE STREQUAL "Sanitize")
    message(STATUS "Enabling sanitize address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSANITIZE")
    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer -O0)
    add_link_options(-fsanitize=address)
    # link_directories(/opt/homebrew/opt/llvm/lib/clang/20/lib/darwin)
endif ()

add_library(playground STATIC src/playground.cpp)
target_include_directories(playground PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(playground PRIVATE ${CMAKE_SOURCE_DIR}/external)

add_library(message_obj STATIC src/message.cpp)
target_include_directories(message_obj PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(message_obj PRIVATE ${CMAKE_SOURCE_DIR}/external)

add_library(toyqueue_obj STATIC src/toyqueue.cpp)
target_include_directories(toyqueue_obj PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(toyqueue_obj PRIVATE ${CMAKE_SOURCE_DIR}/external)

add_library(learn_coro_obj STATIC src/learn_coro.cpp)
target_include_directories(learn_coro_obj PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(learn_coro_obj PRIVATE ${CMAKE_SOURCE_DIR}/external)

# Compile main app
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(main PRIVATE playground message_obj toyqueue_obj)


option(RUN_CLANG_TIDY "run clang-tidy" OFF)
if (RUN_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
    message(STATUS "Enabled clang-tidy")
endif ()
