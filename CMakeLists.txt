cmake_minimum_required(VERSION 3.0.0)
project(stash VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED)

add_executable(stashserver src/server/server.cpp)
add_executable(stashclient src/client/client.cpp)

foreach(target stashclient stashserver)
    target_include_directories(${target} PRIVATE include Boost::boost)
    target_link_libraries(${target} PRIVATE Boost::boost)
    target_compile_options(${target} PRIVATE -Wall -Werror -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wunused)
endforeach()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
