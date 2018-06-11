cmake_minimum_required (VERSION 3.11)

project (UniEvent VERSION 0.6.0.0 LANGUAGES CXX)

set(my_source_files
  Event.cxx
  Dispatcher.cxx
  SimpleDispatcher.cxx
  SimpleDispatcherImp.cxx
  UNIXError.cxx
  Timer.cxx
  UnixEventRegistry.cxx
  UnixEventPoll.cxx
  TimerEventTracker.cxx
  RegistryDispatcherGlue.cxx
  )

add_library(UniEvent STATIC ${my_source_files})
add_library(UniEvent_obj OBJECT ${my_source_files})
target_compile_features(UniEvent PUBLIC cxx_std_17)
target_compile_features(UniEvent_obj PUBLIC cxx_std_17)
set_property(TARGET UniEvent_obj
  PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property(TARGET UniEvent UniEvent_obj PROPERTY CXX_EXTENSIONS OFF)
target_link_libraries(UniEvent PUBLIC LCore)

# In CMake 3.12, this should become possible
# target_link_libraries(UniEvent_obj PUBLIC LCore)

target_include_directories(
  UniEvent PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:include>"
  )

# In CMake 3.12, this should no longer be necessary.
get_target_property(lcore_include_dirs LCore INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(
  UniEvent_obj PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  ${lcore_include_dirs}
  "$<INSTALL_INTERFACE:include>"
  )


file(GLOB_RECURSE libnet_unievent_include_SRCS "pubinclude/UniEvent/*.h")
install(
  FILES
    ${libnet_unievent_include_SRCS}
  DESTINATION
    include/UniEvent
  COMPONENT
    Devel
    )
