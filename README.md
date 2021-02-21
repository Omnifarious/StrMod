If you're looking at this on GitHub, the master repository is actually
here: https://bitbucket.org/omnifarious/StrMod

OK, you've downloaded this thing, now what?

The first thing to do would be to build it.  This builds with CMake and
there are two major targets.  The Net target, which builds the shared
library, and the PortForward target, which builds an example program.

Eventually, there should be tests.  Though, I've been saying that for a
very long time.  Also, I should make this so it can easily be installed
and worked with by programs that depend on it.  This includes being
installed by a Linux distribution's package manager, but it also
includes being a sub-repo in Mercurial or git and being able to depend
on it from the main repo.

Also, I really need to add a Doxygen target for all of this.

Anyway, I hope this is enough to get you started.  Feel free to come
onto irc at irc.freenode.net, channel #StrMod and ask questions.
