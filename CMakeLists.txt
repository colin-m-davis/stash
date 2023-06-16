cmake_minimum_required(VERSION 3.0.0)
project(stash VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

set(BOOST_ROOT "/opt/homebrew/Cellar/boost/1.82.0/include/boost")
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED)

add_executable(stashserver src/server/server.cpp)
target_include_directories(stashserver PRIVATE include Boost::boost) 
target_link_libraries(stashserver PRIVATE Boost::boost)
target_compile_options(stashserver PRIVATE -Wall -Werror -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wunused)

add_executable(stashclient src/client/client.cpp)
target_include_directories(stashclient PRIVATE include Boost::boost)
target_link_libraries(stashclient PRIVATE Boost::boost)
target_compile_options(stashclient PRIVATE -Wall -Werror -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wunused)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
