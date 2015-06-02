cmake_minimum_required (VERSION 3.0)
project (Phased)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden")
set(CMAKE_EXE_LINKER_FLAGS "-ldl -rdynamic")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

set(DIST_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_library (SingInSyncAgent SHARED
  SingInSyncAgent.cpp)

target_link_libraries (SingInSyncAgent
  ${LIBS})

file(MAKE_DIRECTORY ${DIST_DIR}/bin)

set_target_properties (SingInSyncAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR}/bin
    PREFIX "")

configure_file(manifesto.xml ${DIST_DIR} COPYONLY)
