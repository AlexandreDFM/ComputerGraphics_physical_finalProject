#
# File Name: CMakeLists.txt
# Author : Alexandre Kévin DE FREITAS MARTINS
# Creation Date: 3/5/2025
# Description: This is the CMakeLists.txt
# Copyright (c) 2025 Alexandre Kévin DE FREITAS MARTINS
# Version: 1.0.0
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Use file(GLOB ...) to collect all .cpp files in the directory
#file(GLOB SOURCES1 "${SOURCE_DIR1}/*.cpp")
#file(GLOB SOURCES2 "${SOURCE_DIR2}/*.cpp")

#execution file name
set(PROJECT_NAME lab)

#Current lab
set(CURRENT lab_number_name)

set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

find_package(FLTK CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

if (UNIX)
  find_package(FLTK REQUIRED)
  include_directories(${FLTK_INCLUDE_DIR})
  include_directories(${CMAKE_SOURCE_DIR}/Cyclone)
endif()

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}/*.h"
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/"  "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE fltk fltk_gl fltk_forms fltk_images glm::glm)
