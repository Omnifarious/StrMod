cmake_minimum_required (VERSION 3.11)

project (LCore VERSION 0.6 LANGUAGES CXX)

add_library(LCore Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
add_library(LCore_obj OBJECT Debugable.cxx gentyimp.cxx lcerror.cxx simple_bitset.cxx)
target_compile_features(LCore INTERFACE cxx_std_17)
target_compile_features(LCore PUBLIC cxx_std_17)
target_compile_features(LCore_obj INTERFACE cxx_std_17)
target_compile_features(LCore_obj PUBLIC cxx_std_17)

target_include_directories(
  LCore INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(
  LCore PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(
  LCore_obj INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(
  LCore_obj PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}")
set(pkgconfig_install_dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
set(version_config "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# generate package version file
include(CMakePackageConfigHelpers)

# generate package version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# install headers
install(
    DIRECTORY "${HEADER_BASE}/${PROJECT_NAME}"
    DESTINATION "${include_install_dir}"
)

# install project version file
install(
    FILES "${version_config}"
    EXCLUDE_FROM_ALL
    DESTINATION "${config_install_dir}"
)

# install targets
install(
    TARGETS LCore LCore_obj
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    EXCLUDE_FROM_ALL
)

# install project config file
install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
    FILE ${project_config}
    EXCLUDE_FROM_ALL
)

# export build directory config file
export(
    EXPORT ${targets_export_name}
    NAMESPACE "${namespace}"
    FILE ${project_config}
)

# register project in CMake user registry
export(PACKAGE ${PROJECT_NAME})

file(GLOB_RECURSE libnet_lcore_include_SRCS "${HEADER_BASE}/*.h")
add_custom_target(libnet_lcore_headers_for_ide SOURCES ${libnet_lcore_include_SRCS})
