cmake_minimum_required (VERSION 3.0)
project (Chat)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden")
set(CMAKE_EXE_LINKER_FLAGS "-ldl -rdynamic")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

set(CHAT_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_library (ChatAgent SHARED
  ChatAgent.cpp)

target_link_libraries (ChatAgent
  ${LIBS})


file(MAKE_DIRECTORY ${CHAT_DIR}/bin)

set_target_properties (ChatAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CHAT_DIR}/bin
    PREFIX "")

configure_file(manifesto.xml ${CHAT_DIR} COPYONLY)