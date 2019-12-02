cmake_minimum_required(VERSION 3.1.2)
project("DataInjector"
  LANGUAGES CXX
  VERSION 1.0.1)

if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_STANDARD 11)
endif()

string(TOUPPER ${PROJECT_NAME} ${PROJECT_NAME}_UPPER)
string(TOLOWER ${PROJECT_NAME} ${PROJECT_NAME}_LOWER)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(
  "include"
  "libtins/include"
  Boost_INCLUDE_DIRS
  )

add_executable(
  "data-injector"
  "src/packager.cpp"
  "src/msc_data_group_generator.cpp"
  "src/packet_generator.cpp"
  "src/crc16.cpp"
  )

target_link_libraries(
  "data-injector"
  "${CMAKE_SOURCE_DIR}/libtins/build/lib/libtins.a"
  Threads::Threads
  "Boost::system"
  )
