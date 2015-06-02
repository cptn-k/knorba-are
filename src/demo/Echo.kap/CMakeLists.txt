cmake_minimum_required (VERSION 3.0)
project (Echo)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden")
set(CMAKE_EXE_LINKER_FLAGS "-ldl -rdynamic")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

set(ECHO_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_library (EchoAgent SHARED
  EchoAgent.cpp)

target_link_libraries (EchoAgent
  ${LIBS})

file(MAKE_DIRECTORY ${ECHO_DIR}/bin)

set_target_properties (EchoAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${ECHO_DIR}/bin
    PREFIX "")

configure_file(manifesto.xml ${ECHO_DIR} COPYONLY)
