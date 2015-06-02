cmake_minimum_required (VERSION 3.2)
project (Hello)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "-ldl")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

add_library (HelloAgent SHARED
  HelloAgent.cpp)

target_link_libraries (HelloAgent
  ${LIBS})

set(HELLO_DIR ${CMAKE_CURRENT_BINARY_DIR})

FILE(MAKE_DIRECTORY ${HELLO_DIR}/bin)

set_target_properties (HelloAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${HELLO_DIR}/bin
    PREFIX "")

configure_file(manifesto.xml ${HELLO_DIR} COPYONLY)