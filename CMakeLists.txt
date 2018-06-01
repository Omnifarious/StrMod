cmake_minimum_required (VERSION 3.11)

project (libNet VERSION 0.6 LANGUAGES CXX)

add_subdirectory(LCore)

find_package(LCore)
set_property(TARGET LCore_obj PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(Net SHARED empty.cxx $<TARGET_OBJECTS:LCore_obj>)
