cmake_minimum_required (VERSION 3.11)

project (StrMod VERSION 0.6.0.0 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build libNet as a shared library." ON)

add_subdirectory(LCore)
add_subdirectory(EHnet++)
add_subdirectory(UniEvent)
add_subdirectory(StrMod)

include(GenerateExportHeader)

# Hopefully the correct thing to do in CMake 3.12
# add_library(Net empty.cxx)
#target_link_libraries(Net PUBLIC LCore_obj EHnet++_obj)

# The correct thing to do now.
add_library(Net
  $<TARGET_OBJECTS:LCore_obj>
  $<TARGET_OBJECTS:EHnet++_obj>
  $<TARGET_OBJECTS:UniEvent_obj>
  $<TARGET_OBJECTS:StrMod_obj>
  )
add_library(StrMod::Net ALIAS Net)

generate_export_header(Net)

set_property(TARGET Net PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET Net PROPERTY VERSION ${StrMod_VERSION})
set_property(TARGET Net PROPERTY SOVERSION 0)
set_property(TARGET Net PROPERTY INTERFACE_Net_MAJOR_VERSION 0)

target_compile_features(Net PUBLIC cxx_std_17)

# In CMake 3.12, this should no longer be necessary.
get_target_property(lcore_include_dirs LCore INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(ehnetpp_include_dirs EHnet++ INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(unievent_include_dirs UniEvent INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(strmod_include_dirs StrMod INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(
  Net PUBLIC
  ${lcore_include_dirs}
  ${ehnetpp_include_dirs}
  ${unievent_include_dirs}
  ${strmod_include_dirs}
  )

include(GNUInstallDirs)

install(TARGETS Net EXPORT NetTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/Net/NetConfigVersion.cmake"
  VERSION ${StrMod_VERSION}
  COMPATIBILITY ExactVersion
)

export(EXPORT NetTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/Net/NetTargets.cmake"
  NAMESPACE StrMod::
)

set(STRMOD_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Net CACHE STRING
  "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/NetConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/Net/NetConfig.cmake"
  INSTALL_DESTINATION ${STRMOD_CMAKE_DIR}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Net/NetConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Net/NetConfigVersion.cmake"
  DESTINATION
    ${STRMOD_CMAKE_DIR}
)
install(EXPORT NetTargets DESTINATION ${STRMOD_CMAKE_DIR} NAMESPACE StrMod::)
