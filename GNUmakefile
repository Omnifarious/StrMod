CC = gcc -mpentiumpro -pipe
CFLAGS = -O2 -g
CXX = g++ -mpentiumpro -pipe
CXXFLAGS = -O2 -g
CPPFLAGS = -I/home/hopper/src/include++ -DBYTE_ORDER_DEFINED -DORDER_XINU

%.cc.d : %.cc
	+$(CXX) -c $< $(CPPFLAGS) $(CXXFLAGS) -M | sed "s;$<;;" | sed 's;^$(notdir $*)\.o;$*.cc;' >$@
	+echo '	' touch $< >>$@
	+cat $@ | dependdb.pl update $<
	+rm -f $@

%.o : %.cc
	$(CXX) -c $(@:.o=.cc) $(CPPFLAGS) $(CXXFLAGS) -o $@

NETLIB = /home/hopper/lib/libNet.a
NETTAG = libnet.stamp

all : $(NETLIB)
.PHONY : all $(NETLIB) $(ALLIB)

$(NETLIB) :
	( cd EHnet++ ; \
	  $(MAKE) EXTLIBTAG=$(NETTAG) EXTLIB=$(NETLIB) $(NETTAG) )
	( cd LCore ; \
	  $(MAKE) EXTLIBTAG=$(NETTAG) EXTLIB=$(NETLIB) $(NETTAG) )
	( cd StrMod ; \
	  $(MAKE) EXTLIBTAG=$(NETTAG) EXTLIB=$(NETLIB) $(NETTAG) )
	( cd UniEvent ; \
	  $(MAKE) EXTLIBTAG=$(NETTAG) EXTLIB=$(NETLIB) $(NETTAG) )
# 	( cd Dispatch ; \
# 	  $(MAKE) EXTLIBTAG=$(NETTAG) EXTLIB=$(NETLIB) $(NETTAG) )

#$(ALLIB) :
#	( cd EHnet++ ; \
#	  $(MAKE) EXTLIBTAG=$(ALLTAG) EXTLIB=$(ALLIB) $(ALLTAG) )
#	( cd LCore ; \
#	  $(MAKE) EXTLIBTAG=$(ALLTAG) EXTLIB=$(ALLIB) $(ALLTAG) )
#	( cd Dispatch ; \
#	  $(MAKE) EXTLIBTAG=$(ALLTAG) EXTLIB=$(ALLIB) $(ALLTAG) )
#	( cd StrMod ; \
#	  $(MAKE) EXTLIBTAG=$(ALLTAG) EXTLIB=$(ALLIB) $(ALLTAG) )
#	( cd containe ; \
#	  $(MAKE) EXTLIBTAG=$(ALLTAG) EXTLIB=$(ALLIB) $(ALLTAG) )
