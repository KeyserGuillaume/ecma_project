cmake_minimum_required(VERSION 2.8.4)
project(ECMA)

set(CMAKE_MODULE_PATH               "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -std=c++14 -DIL_STD -m64 -Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -Og -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3 -flto")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX   "-isystem ")
set(LINKER_OPTIONS                  -flto -Wl,--no-as-needed)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -save-temps=obj")
endif()

find_package(Cplex)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})

set(SOURCE_FILES
        src/main.cpp src/Instance.cpp src/Instance.h
        src/Dualization.cpp src/Dualization.h
        src/branch_cut.cpp src/branch_cut.h
        src/heuristic.cpp src/heuristic.h
        src/cutting_plane.cpp src/cutting_plane.h)

add_executable(         ecma ${SOURCE_FILES})
target_link_libraries(  ecma PRIVATE ${CPLEX_LIBRARIES})
target_link_libraries(  ecma PRIVATE dl)
