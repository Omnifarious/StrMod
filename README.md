OK, you've downloaded this thing, now what?

The first thing to do would be to build it.

This version should've come with a 'configure' script in the main
directory.  If it didn't, you'll have to first run 'autoconf' to create
it.  If you have autoconf, running autoconf with no arguments should
work.

Now, the configure script should be run with `./configure`.  You can set
the `CXX` and `CXXFLAGS` environment variables beforehand if you like.

Now you should have a Makefile.

Type `make`.  You'll need a C++ that implements C++11 for the whole
thing to work.  I use g++ 6.3.1 last time I built it, so it may require
at least that version in the future.

After this is done, you'll have two subdirectories of note.

The first is:

`include`

The second is:

`generated/lib`

The first should be given as a -I parameter when you compile things that
use StreamModule, and the second should be given to -L when you link.
The name of the library is 'libNet.a', so you'll need the -lNet
parameter.

Anyway, I hope this is enough to get you started.  Feel free to come
onto irc at irc.freenode.net, channel #StrMod and ask questions. Also,
feel free to join the mailing list at
<http://lists.sourceforge.net/mailman/listinfo/strmod-devel>
