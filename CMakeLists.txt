cmake_minimum_required(VERSION 2.6)
project(CLAIRVOYANCE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (DEFINED DEBUG OR DEFINED ASSERT)
  if (DEFINED DEBUG)
    add_definitions(-DDEBUG=${DEBUG})
    message("Debugging enabled")
  endif()
  if (DEFINED ASSERT)
    add_definitions(-DUSE_ASSERTS=${ASSERT})
    message("Assertions enabled")
  endif()
  set(CMAKE_BUILD_TYPE Debug)
  message("Building a debug release")
else()
  set(CMAKE_BUILD_TYPE Release)
  add_definitions(-DNDEBUG=1)
  message("Building an optimized release")
endif()

if (NOT DEFINED NO_JPEG_SUPPORT OR NOT NO_JPEG_SUPPORT)
  find_package(LIBJPEG)
  include_directories(${LIBJPEG_INCLUDE_PATH})
else()
  add_definitions(-DNO_JPEG_SUPPORT=1)
  message("Jpeg support disabled")
endif()


if (NOT DEFINED NO_OMP OR NO_OMP EQUAL 0)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
else()
  add_definitions(-DNO_OMP=${NO_OMP})
endif()


if (NOT DEFINED NO_PNG_SUPPORT OR NOT NO_PNG_SUPPORT)
  find_package(PNG)
  include_directories(${PNG_INCLUDE_PATH})
else()
  add_definitions(-DNO_PNG_SUPPORT=1)
  message("Png support disabled")
endif()


if (SHARED)
  set(CLAIRVOYANCE_LIBRARY_TYPE SHARED)
else()
  set(CLAIRVOYANCE_LIBRARY_TYPE STATIC)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-aliasing") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99") 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${DOMLIB_PATH}")
add_subdirectory("src")
#add_subdirectory("include")

