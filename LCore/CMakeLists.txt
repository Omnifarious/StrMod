cmake_minimum_required (VERSION 3.11)

project (LCore VERSION 0.6.0.0 LANGUAGES CXX)

add_library(LCore STATIC Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
add_library(LCore_obj OBJECT Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
target_compile_features(LCore PUBLIC cxx_std_17)
target_compile_features(LCore_obj PUBLIC cxx_std_17)
set_property(TARGET LCore_obj
  PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property(TARGET LCore LCore_obj PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(
  LCore
    PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
      "$<INSTALL_INTERFACE:include>"
  )
target_include_directories(
  LCore_obj
    PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
      "$<INSTALL_INTERFACE:include>"
  )


file(GLOB_RECURSE libnet_lcore_include_SRCS "pubinclude/LCore/*.h")
install(
  FILES
    ${libnet_lcore_include_SRCS}
  DESTINATION
    include/LCore
  COMPONENT
    Devel
    )
