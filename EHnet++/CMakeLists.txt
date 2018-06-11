cmake_minimum_required (VERSION 3.11)

project (EHnet++ VERSION 0.6.0.0 LANGUAGES CXX)

add_library(EHnet++ STATIC InetAddress.cxx SocketAddress.cxx InetAddrNetDB.cxx)
add_library(EHnet++_obj OBJECT InetAddress.cxx SocketAddress.cxx InetAddrNetDB.cxx)
target_compile_features(EHnet++ PUBLIC cxx_std_17)
target_compile_features(EHnet++_obj PUBLIC cxx_std_17)
set_property(TARGET EHnet++_obj
  PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property(TARGET EHnet++ EHnet++_obj PROPERTY CXX_EXTENSIONS OFF)
add_dependencies(EHnet++ LCore)
add_dependencies(EHnet++_obj LCore)

get_target_property(lcore_include_dirs LCore INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(
  EHnet++ PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  ${lcore_include_dirs}
  "$<INSTALL_INTERFACE:include>"
  )
target_include_directories(
  EHnet++_obj PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  ${lcore_include_dirs}
  "$<INSTALL_INTERFACE:include>"
  )


file(GLOB_RECURSE libnet_ehnetpp_include_SRCS "pubinclude/EHnet++/*.h")
install(
  FILES
    ${libnet_ehnetpp_include_SRCS}
  DESTINATION
    include/EHnet++
  COMPONENT
    Devel
    )
