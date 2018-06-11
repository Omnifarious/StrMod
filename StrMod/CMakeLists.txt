cmake_minimum_required (VERSION 3.11)

project (StrMod VERSION 0.6.0.0 LANGUAGES CXX)

set(my_source_files
  EchoModule.cxx
  StreamModule.cxx
  StrChunk.cxx
  LinearExtent.cxx
  StrSubChunk.cxx
  GroupChunk.cxx
  StreamFDModule.cxx
  LinearExtentStrmOp.cxx
  OutSerializer.cxx
  InSerializer.cxx
  StreamSplitter.cxx
  SocketModule.cxx
  SockListenModule.cxx
  EOFStrChunk.cxx
  FDUtil.cxx
  SimpleMulti.cxx
  StreamProcessor.cxx
  ProcessorModule.cxx
  CharChopper.cxx
  PassThrough.cxx
  NewlineChopper.cxx
  BufferChunk.cxx
  PreAllocBuffer.cxx
  DynamicBuffer.cxx
  BufferChunkFactory.cxx
  InfiniteModule.cxx
  TelnetParser.cxx
  TelnetChunker.cxx
  TelnetChars.cxx
  TelnetChunkBuilder.cxx
  ChunkVisitor.cxx
  UseTrackingVisitor.cxx
  GraphVizVisitor.cxx
  ChunkIterator.cxx
  StaticBuffer.cxx
  ApplyVisitor.cxx
  SimpleTelnetClient.cxx
  RouterModule.cxx
  StrChunkDecorator.cxx
  StrChunkUDPDecorator.cxx
  # UDPSocketModule.cxx  MCastSocketModule.cxx - Not ready yet.
  )

add_library(StrMod STATIC ${my_source_files})
add_library(StrMod_obj OBJECT ${my_source_files})
target_compile_features(StrMod PUBLIC cxx_std_17)
target_compile_features(StrMod_obj PUBLIC cxx_std_17)
set_property(TARGET StrMod_obj
  PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_property(TARGET StrMod StrMod_obj PROPERTY CXX_EXTENSIONS OFF)
target_link_libraries(StrMod PUBLIC UniEvent EHnet++ LCore)

# In CMake 3.12, this should become possible
# target_link_libraries(StrMod_obj PUBLIC UniEvent EHnet++ LCore)

target_include_directories(
  StrMod PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  "$<INSTALL_INTERFACE:include>"
  )

# In CMake 3.12, this should no longer be necessary.
get_target_property(unievent_include_dirs UniEvent INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(ehnetpp_include_dirs EHnet++ INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(lcore_include_dirs LCore INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(
  StrMod_obj PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pubinclude>"
  ${unievent_include_dirs}
  ${ehnetpp_include_dirs}
  ${lcore_include_dirs}
  "$<INSTALL_INTERFACE:include>"
  )


file(GLOB_RECURSE libnet_unievent_include_SRCS "pubinclude/StrMod/*.h")
install(
  FILES
    ${libnet_unievent_include_SRCS}
  DESTINATION
    include/StrMod
  COMPONENT
    Devel
    )
