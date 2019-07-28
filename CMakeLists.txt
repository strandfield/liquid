cmake_minimum_required(VERSION 3.9)
project(liquid)

set (CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

##################################################################
###### coverage build
##################################################################

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

##################################################################
###### json-toolkit
##################################################################

file(GLOB_RECURSE JSON_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/json-toolkit/include/*.h)

add_custom_target(json-toolkit SOURCES ${JSON_HDR_FILES})

#add_library(json-toolkit INTERFACE)
#target_sources(json-toolkit INTERFACE ${JSON_HDR_FILES})

##################################################################
###### liquid
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
add_library(liquid ${HDR_FILES} ${SRC_FILES})
target_include_directories(liquid PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(liquid PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/json-toolkit/include")

foreach(_source IN ITEMS ${HDR_FILES} ${SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_compile_definitions(liquid PUBLIC -DLIQUID_BUILD_SHARED_LIBRARY)

##################################################################
###### tests, examples & benchmarks
##################################################################

add_subdirectory(tests)
#add_subdirectory(examples)
#add_subdirectory(benchmarks)
