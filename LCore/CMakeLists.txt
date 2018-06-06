cmake_minimum_required (VERSION 3.11)

project (LCore VERSION 0.6.0.0 LANGUAGES CXX)

add_library(LCore STATIC Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
add_library(LCore_obj OBJECT Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
target_compile_features(LCore PUBLIC cxx_std_17)
target_compile_features(LCore_obj PUBLIC cxx_std_17)
set_property(TARGET LCore_obj
  PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

target_include_directories(
  LCore PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )
target_include_directories(
  LCore_obj PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )

set_property(TARGET LCore PROPERTY VERSION ${LCore_VERSION})
set_property(TARGET LCore_obj PROPERTY VERSION ${LCore_VERSION})
set_property(TARGET LCore APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING LCore ${LCore_VERSION})
set_property(TARGET LCore_obj APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING LCore_obj ${LCore_VERSION}
  )

set(namespace "${PROJECT_NAME}::")
set(targets_export_name "${PROJECT_NAME}Targets")
set(version_config "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake")
#set(plain_config "${PROJECT_NAME}Config.cmake")

install(
  TARGETS LCore LCore_obj
  EXPORT "${targets_export_name}"
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
  OBJECTS DESTINATION obj
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

# generate package version file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${version_config}" VERSION "${LCore_VERSION}" COMPATIBILITY ExactVersion
)

export(EXPORT "${targets_export_name}"
  FILE "${version_config}"
  NAMESPACE "${namespace}"
  )

#configure_file("cmake/${plain_config}"
#  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${plain_config}"
#  COPYONLY
#  )

#set(ConfigPackageLocation lib/cmake/"${plain_config}")

#install(EXPORT "${targets_export_name}"
#  FILE "${targets_export_name}.cmake"
#  NAMESPACE "${namespace}"
#  DESTINATION "${ConfigPackageLocation}"
#  )

#install(
#  FILES
#    cmake/"${plain_config}"
#    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cma#ke"
#  DESTINATION
#    "${ConfigPackageLocation}"
#  COMPONENT
#    Devel
#  )
