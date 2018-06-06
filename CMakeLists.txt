cmake_minimum_required (VERSION 3.11)

project (libNet VERSION 0.6.0.0 LANGUAGES CXX)

add_subdirectory(LCore)

find_package(LCore)

add_library(Net $<TARGET_OBJECTS:LCore_obj>)
