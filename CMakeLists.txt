cmake_minimum_required(VERSION 3.0.0)
project(store VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -fcoroutines-ts")

set(BOOST_ROOT "/usr/local/boost_1_82_0")
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED)

if(Boost_FOUND)
    add_executable(server src/server/server.cpp)
    target_include_directories(server PRIVATE Boost::boost) 
    target_link_libraries(server PRIVATE Boost::boost)
    target_compile_options(server PRIVATE -Wextra -Wshadow -Werror
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)

    add_executable(client src/client/client.cpp)
    target_include_directories(client PRIVATE Boost::boost) 
    target_link_libraries(client Boost::boost)
    target_compile_options(client PRIVATE -Wextra -Wshadow -Werror
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
