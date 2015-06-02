cmake_minimum_required (VERSION 3.0)
project (PicServer)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden")
set(CMAKE_EXE_LINKER_FLAGS "-ldl -rdynamic")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

set(DIST_DIR ${CMAKE_CURRENT_BINARY_DIR})

file(MAKE_DIRECTORY ${DIST_DIR}/bin)
file(MAKE_DIRECTORY ${DIST_DIR}/resources)


# //\/ manifesto /\////////////////////////////////////////////////////////////

configure_file(manifesto.xml ${DIST_DIR} COPYONLY)


# //\/ PicUI /\////////////////////////////////////////////////////////////////

add_custom_command (TARGET PicServerAgent POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../java/PicUI/dist/GridUI.jar ${DIST_DIR}/resources)


# //\/ demo: UiGridOutputAgent /\//////////////////////////////////////////////

add_library (PicServerAgent SHARED
  PicServerAgent.cpp
  PicProtocol.cpp)

target_link_libraries (PicServerAgent
  ${LIBS})

set_target_properties (PicServerAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR}/bin
    PREFIX "")


# //\/ demo: PicClientAgent /\///////////////////////////////////////////////////

add_library (PicClientAgent SHARED
  PicClientAgent.cpp
  PicProtocol.cpp)

target_link_libraries (PicClientAgent
  ${LIBS})

set_target_properties (PicClientAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR}/bin
    PREFIX "")
