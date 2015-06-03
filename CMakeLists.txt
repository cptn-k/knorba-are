cmake_minimum_required (VERSION 3.0)
project (KnoRBA-ARE)

set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_CXX_FLAGS "-fPIC -fvisibility=hidden -pthread -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "-ldl")
include_directories(SYSTEM /usr/local/include)
include_directories(src)
link_directories(/usr/local/lib)

set(LIBS knorba kfoundation cityhash unistring KnorbaStandardProtocols)

# //\/ are /\//////////////////////////////////////////////////////////////////

add_executable (are
  src/are/are.cpp
  src/are/PeerElement.cpp
  src/are/Arguments.cpp
  src/are/AreException.cpp
  src/are/KernelRuntime.cpp
  src/are/VirtualRuntime.cpp
  src/are/Bundle.cpp
  src/are/ManifestoElement.cpp
  src/are/LoadElement.cpp
  src/are/RunElement.cpp
  src/are/MessageElement.cpp
  src/are/OpcodeTable.cpp
  src/are/AgentDatabase.cpp
  src/are/AgentInstance.cpp
  src/are/DynamicAgentLoader.cpp
  src/are/RuntimeBase.cpp
  src/kernel/KernelAgent/ClusterSetupProtocol.cpp
  src/kernel/KernelAgent/KernelAgent.cpp)

add_dependencies(are
  Kernel
  Shell)

target_link_libraries (are
  ${LIBS})


# //\/ bin /\//////////////////////////////////////////////////////////////////

if(APPLE)
  set(BINARIES_DIR ${CMAKE_SOURCE_DIR}/macos-x64-bin)
endif(APPLE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(BINARIES_DIR ${CMAKE_SOURCE_DIR}/linux-x64-bin)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(NOT BINARIES_DIR)
  message(FATAL_ERROR "Unrecognized operating system: ${CMAKE_SYSTEM_NAME}")
endif(NOT BINARIES_DIR)

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

add_custom_command (TARGET are PRE_BUILD
COMMAND cp ${BINARIES_DIR}/tar ${BIN_DIR})


# //\/ repo /\/////////////////////////////////////////////////////////////////

set(REPO_DIR ${CMAKE_BINARY_DIR}/repo)
file(MAKE_DIRECTORY ${REPO_DIR})

set(POOL_DIR ${REPO_DIR}/pool)
file(MAKE_DIRECTORY ${POOL_DIR})


# //\/ UnixSocketAgent /\//////////////////////////////////////////////////////

add_library(UnixSocketAgent  SHARED
  src/kernel/UnixSocketAgent/UnixSocketInputStream.cpp
  src/kernel/UnixSocketAgent/UnixSocketOutputStream.cpp
  src/kernel/UnixSocketAgent/UnixSocketAgent.cpp)

target_link_libraries(UnixSocketAgent
  ${LIBS})

set_target_properties(UnixSocketAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${POOL_DIR}
    PREFIX "")


# //\/ InternetAgent /\////////////////////////////////////////////////////////

add_library(InternetAgent SHARED
  src/kernel/InternetAgent/InternetAgent.cpp)

target_link_libraries(InternetAgent
  ${LIBS})

set_target_properties(InternetAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${POOL_DIR}
    PREFIX "")


# //\/ ConsoleAgent /\/////////////////////////////////////////////////////////

add_library(ConsoleAgent SHARED
  src/kernel/ConsoleAgent/ConsoleAgent.cpp)

target_link_libraries(ConsoleAgent
  ${LIBS}
  ncurses)

set_target_properties(ConsoleAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${POOL_DIR}
    PREFIX "")


# //\/ DisplayInfoAgent /\/////////////////////////////////////////////////////

add_library(DisplayInfoAgent SHARED
  src/kernel/DisplayInfoAgent/DisplayInfoAgent.cpp)

target_link_libraries(DisplayInfoAgent
  ${LIBS})

set_target_properties(DisplayInfoAgent
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${POOL_DIR}
    PREFIX "")



# //\/ Kernel /\///////////////////////////////////////////////////////////////

add_custom_target(Kernel)

add_dependencies(Kernel
  UnixSocketAgent
  InternetAgent
  ConsoleAgent)

set(KERNEL_DIR ${REPO_DIR}/Kernel.kap)

add_custom_command (TARGET Kernel PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR})

add_custom_command (TARGET Kernel PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}/bin)

if(NOT EXISTS ${KERNEL_DIR}/manifesto.xml)
  file(COPY ${CMAKE_SOURCE_DIR}/src/kernel/Kernel.kap/manifesto.xml
    DESTINATION ${KERNEL_DIR})
endif(NOT EXISTS ${KERNEL_DIR}/manifesto.xml)

add_custom_command (TARGET Kernel POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:UnixSocketAgent> ${KERNEL_DIR}/bin)

add_custom_command (TARGET Kernel POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:InternetAgent> ${KERNEL_DIR}/bin)

add_custom_command (TARGET Kernel POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ConsoleAgent> ${KERNEL_DIR}/bin)

add_custom_command (TARGET Kernel POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DisplayInfoAgent> ${KERNEL_DIR}/bin)


# //\/ Shell /\////////////////////////////////////////////////////////////////

add_custom_target(Shell)

add_dependencies(Shell
  UnixSocketAgent)

set(SHELL_DIR ${REPO_DIR}/Shell.kap)

add_custom_command (TARGET Shell PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${SHELL_DIR})

add_custom_command (TARGET Shell PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${SHELL_DIR}/bin)

add_custom_command (TARGET Shell PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/kernel/Shell.kap/manifesto.xml ${SHELL_DIR})

add_custom_command (TARGET Shell POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:UnixSocketAgent> ${SHELL_DIR}/bin)

add_custom_command (TARGET Shell POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ConsoleAgent> ${SHELL_DIR}/bin)


# //\/ demo /\/////////////////////////////////////////////////////////////////

set(DEMO_DIR ${CMAKE_BINARY_DIR}/demo)
file(MAKE_DIRECTORY ${DEMO_DIR})

set(DEMO_SHARED_DIR ${DEMO_DIR}/shared)
file(MAKE_DIRECTORY ${DEMO_SHARED_DIR})

add_subdirectory(src/demo/Hello.kap ${DEMO_DIR}/Hello.kap)
add_subdirectory(src/demo/Echo.kap ${DEMO_DIR}/Echo.kap)
add_subdirectory(src/demo/Chat.kap ${DEMO_DIR}/Chat.kap)
add_subdirectory(src/demo/PingPong.kap ${DEMO_DIR}/PingPong.kap)
add_subdirectory(src/demo/Phased.kap ${DEMO_DIR}/Phased.kap)
add_subdirectory(src/demo/CellAutomata.kap ${DEMO_DIR}/CellAutomata.kap)
add_subdirectory(src/demo/PicServer.kap ${DEMO_DIR}/PicServer.kap)
