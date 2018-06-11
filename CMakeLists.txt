cmake_minimum_required (VERSION 3.11)

project (libNet VERSION 0.6.0.0 LANGUAGES CXX)

add_subdirectory(LCore)
add_subdirectory(EHnet++)
add_subdirectory(UniEvent)
add_subdirectory(StrMod)

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

set_property(TARGET Net PROPERTY CXX_EXTENSIONS OFF)
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
