cmake_minimum_required (VERSION 3.11)

project (libNet VERSION 0.6.0.0 LANGUAGES CXX)

add_subdirectory(LCore)
add_subdirectory(EHnet++)

add_library(Net $<TARGET_OBJECTS:LCore_obj> $<TARGET_OBJECTS:EHnet++_obj>)
