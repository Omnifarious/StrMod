SHELL := /bin/sh

MY_BASE := $(HOME)
MYLIB := -L$(MY_BASE)/lib
MYINCLUDE := -I$(MY_BASE)/src/include++
MYFLAGS := -O2

LDFLAGS := $(MYLIB) -lEHaudio++ -lRW -ldist -lEHAll -lsocket -lnsl

CXX := g++ -pipe -m486
CC  := gcc -pipe -m486

CXXFLAGS := $(MYINCLUDE) $(MYFLAGS)
CFLAGS := -O2 -pipe

default : lib/libEHaudio++.a

# lib/libEHaudio++.a : StrChunk.o StrChunkio.o StrFDPlug.o StreamFDModule.o \
#                  StreamModule.o StreamSplitter.o SocketModule.o \
#                  SockListenModule.o
lib/libEHaudio++.a : StreamModule.o  EchoModule.o  StrChunkio.o  StrChunk.o \
		     StrFDPlug.o  StreamFDModule.o  StreamSplitter.o \
                     StrChunkid.o  STR_ClassIdent.o  SocketModule.o \
                     SockListenModule.o
	ar ruv lib/libEHaudio++.a $?

server : server.o lib/libEHaudio++.a
	$(C++) server.o $(LDFLAGS) -o server

echoserv : echoserv.o lib/libEHaudio++.a
	$(CXX) echoserv.o $(LDFLAGS) -o echoserv

testAudFile : testAudFile.o lib/libEHaudio++.a
	$(C++) tesstAudFile.o $(LDFLAGS) -o testAudFile

testdist : testdist.o lib/libEHaudio++.a
	$(C++) testdist.o $(LDFLAGS) -o testdist

teststr : teststr.o lib/libEHaudio++.a
	$(C++) teststr.o $(LDFLAGS) -o teststr

testchunk : testchunk.o lib/libEHaudio++.a
	$(C++) testchunk.o $(LDFLAGS) -o testchunk

testecho : testecho.o lib/libEHaudio++.a
	$(CXX) testecho.o $(LDFLAGS) -o testecho 2>&1 | c++filt

depend :
	depend.perl < GNUmakefile >GNUmakefile.new
	echo '## Heh heh heh' >>GNUmakefile.new
	echo >>GNUmakefile.new
	g++  $(C++FLAGS) -M *.cc >>GNUmakefile.new
#	gcc $(CFLAGS) -MM *.c >>GNUmakefile.new
	mv GNUmakefile GNUmakefile~
	mv GNUmakefile.new GNUmakefile

## Do not remove this line, or the next, they are needed for make depend
##Heh, heh, heh.
## Heh heh heh

ChunkType.o: ChunkType.cc /home/hopper/src/include++/StrMod/ChunkType.h
EchoModule.o: EchoModule.cc /home/hopper/src/include++/StrMod/EchoModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h
HL7Module.o: HL7Module.cc /home/hopper/src/include++/StrMod/HL7Module.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h
STR_ClassIdent.o: STR_ClassIdent.cc \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/LCore/Programmers.h
SockListenModule.o: SockListenModule.cc \
  /home/hopper/src/include++/StrMod/SockListenModule.h \
  /home/hopper/src/include++/StrMod/SocketModule.h \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/EHnet++/SocketAddress.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /home/hopper/src/include++/EHnet++/InetAddress.h \
  /home/hopper/src/include++/EHnet++/NString.h /usr/include/sys/types.h \
  /usr/include/sys/select.h /usr/include/netinet/in.h \
  /usr/include/sys/stream.h /usr/include/sys/vnode.h /usr/include/sys/time.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/time.h \
  /usr/include/sys/poll.h /usr/include/sys/strmdep.h /usr/include/sys/cred.h \
  /home/hopper/src/include++/Dispatch/dispatcher.h /usr/include/fcntl.h \
  /usr/include/sys/fcntl.h /usr/include/sys/socket.h \
  /usr/include/sys/netconfig.h /usr/include/errno.h /usr/include/sys/errno.h \
  /home/hopper/src/include++/rw/cstring.h \
  /home/hopper/src/include++/rw/tooldefs.h \
  /home/hopper/src/include++/rw/defs.h \
  /home/hopper/src/include++/rw/compiler.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/limits.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/syslimits.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stddef.h \
  /home/hopper/src/include++/rw/ref.h \
  /home/hopper/src/include++/rw/mempool.h \
  /home/hopper/src/include++/rw/procinit.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/ctype.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h
SocketModule.o: SocketModule.cc \
  /home/hopper/src/include++/StrMod/SocketModule.h \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/EHnet++/SocketAddress.h /usr/include/fcntl.h \
  /usr/include/sys/types.h /usr/include/sys/select.h \
  /usr/include/sys/fcntl.h /usr/include/sys/socket.h \
  /usr/include/sys/netconfig.h /usr/include/errno.h /usr/include/sys/errno.h
StrChunk.o: StrChunk.cc /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h
StrChunkio.o: StrChunkio.cc /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/unistd.h \
  /usr/include/sys/unistd.h /usr/include/sys/types.h \
  /usr/include/sys/select.h
StrFDPlug.o: StrFDPlug.cc \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/Dispatch/dispatcher.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/unistd.h \
  /usr/include/sys/unistd.h /usr/include/sys/types.h \
  /usr/include/sys/select.h /usr/gnu/lib/g++-include/iostream.h \
  /usr/gnu/lib/g++-include/streambuf.h /usr/gnu/lib/g++-include/libio.h \
  /usr/gnu/lib/g++-include/_G_config.h /usr/include/errno.h \
  /usr/include/sys/errno.h
StreamFDModule.o: StreamFDModule.cc \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h /usr/include/sys/types.h \
  /usr/include/sys/select.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/stat.h \
  /usr/include/sys/time.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/time.h \
  /usr/include/errno.h /usr/include/sys/errno.h \
  /home/hopper/src/include++/rw/cstring.h \
  /home/hopper/src/include++/rw/tooldefs.h \
  /home/hopper/src/include++/rw/defs.h \
  /home/hopper/src/include++/rw/compiler.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/limits.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/syslimits.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stddef.h \
  /home/hopper/src/include++/rw/ref.h \
  /home/hopper/src/include++/rw/mempool.h \
  /home/hopper/src/include++/rw/procinit.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/ctype.h
StreamModule.o: StreamModule.cc \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h
StreamSplitter.o: StreamSplitter.cc \
  /home/hopper/src/include++/StrMod/StreamSplitter.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h \
  /home/hopper/src/include++/Dispatch/dispatcher.h
StreambufModule.o: StreambufModule.cc \
  /home/hopper/src/include++/StrMod/StreambufModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h \
  /usr/gnu/lib/g++-include/strstream.h /usr/gnu/lib/g++-include/strfile.h
server.o: server.cc /home/hopper/src/include++/StrMod/SockListenModule.h \
  /home/hopper/src/include++/StrMod/SocketModule.h \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/EHnet++/SocketAddress.h \
  /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /home/hopper/src/include++/EHnet++/InetAddress.h \
  /home/hopper/src/include++/EHnet++/NString.h /usr/include/sys/types.h \
  /usr/include/sys/select.h /usr/include/netinet/in.h \
  /usr/include/sys/stream.h /usr/include/sys/vnode.h /usr/include/sys/time.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/time.h \
  /usr/include/sys/poll.h /usr/include/sys/strmdep.h /usr/include/sys/cred.h \
  /home/hopper/src/include++/Dispatch/dispatcher.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h \
  /home/hopper/src/include++/ibmpp/String.h \
  /home/hopper/src/include++/ibmpp/Sortable.h \
  /home/hopper/src/include++/LCore/Protocol.h \
  /home/hopper/src/include++/ibmpp/Key.h \
  /home/hopper/src/include++/ibmpp/Hashable.h \
  /home/hopper/src/include++/ibmpp/RefCounting.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h
testchunk.o: testchunk.cc /home/hopper/src/include++/StrMod/StrChunk.h \
  /usr/gnu/i486-univel-sysv4.2/include/assert.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/stdlib.h \
  /home/hopper/src/include++/StrMod/ChunkType.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/unistd.h \
  /usr/include/sys/unistd.h /usr/include/sys/types.h \
  /usr/include/sys/select.h /usr/gnu/lib/g++-include/iostream.h \
  /usr/gnu/lib/g++-include/streambuf.h /usr/gnu/lib/g++-include/libio.h \
  /usr/gnu/lib/g++-include/_G_config.h /usr/include/fcntl.h \
  /usr/include/sys/fcntl.h /usr/include/errno.h /usr/include/sys/errno.h
testdist.o: testdist.cc /home/hopper/src/include++/Dispatch/dispatcher.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/ibmpp/String.h \
  /home/hopper/src/include++/ibmpp/Sortable.h \
  /home/hopper/src/include++/LCore/Protocol.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/ibmpp/Key.h \
  /home/hopper/src/include++/ibmpp/Hashable.h \
  /home/hopper/src/include++/ibmpp/RefCounting.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h
teststr.o: teststr.cc /home/hopper/src/include++/StrMod/SocketModule.h \
  /home/hopper/src/include++/StrMod/StreamFDModule.h \
  /home/hopper/src/include++/StrMod/StreamModule.h \
  /home/hopper/src/include++/LCore/Object.h \
  /home/hopper/src/include++/LCore/ClassTypes.h \
  /home/hopper/src/include++/LCore/GenTypes.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/sys/byteorder.h \
  /home/hopper/src/include++/StrMod/STR_ClassIdent.h \
  /home/hopper/src/include++/LCore/HopClTypes.h \
  /home/hopper/src/include++/LCore/Programmers.h \
  /home/hopper/src/include++/Dispatch/iohandler.h \
  /home/hopper/src/include++/EHnet++/SocketAddress.h \
  /home/hopper/src/include++/EHnet++/InetAddress.h \
  /home/hopper/src/include++/EHnet++/NString.h /usr/include/sys/types.h \
  /usr/include/sys/select.h /usr/include/netinet/in.h \
  /usr/include/sys/stream.h /usr/include/sys/vnode.h /usr/include/sys/time.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/time.h \
  /usr/include/sys/poll.h /usr/include/sys/strmdep.h /usr/include/sys/cred.h \
  /home/hopper/src/include++/StrMod/StreamSplitter.h \
  /home/hopper/src/include++/Dispatch/dispatcher.h \
  /usr/gnu/lib/g++-include/iostream.h /usr/gnu/lib/g++-include/streambuf.h \
  /usr/gnu/lib/g++-include/libio.h /usr/gnu/lib/g++-include/_G_config.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/unistd.h \
  /usr/include/sys/unistd.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
  /home/hopper/src/include++/ibmpp/String.h \
  /home/hopper/src/include++/ibmpp/Sortable.h \
  /home/hopper/src/include++/LCore/Protocol.h \
  /home/hopper/src/include++/ibmpp/Key.h \
  /home/hopper/src/include++/ibmpp/Hashable.h \
  /home/hopper/src/include++/ibmpp/RefCounting.h \
  /usr/gnu/lib/gcc-lib/i486-univel-sysv4.2/2.6.3/include/string.h
