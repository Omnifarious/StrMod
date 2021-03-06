/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <StrMod/DynamicBuffer.h>
#include <StrMod/GroupVector.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

class NoStrChunk : public StrChunk {
 public:
   NoStrChunk() {}
   virtual ~NoStrChunk();

   virtual unsigned int Length() const { return(0); }
   virtual unsigned int NumSubGroups(const LinearExtent &extent) const
   { return(0); }
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index) {}

 protected:
   virtual void i_DropUnused(const LinearExtent &usedextent) {}
};

NoStrChunk::~NoStrChunk()
{
}

int main(int argc, char *argv[])
{
   int fd1, fd2;

   if (argc != 3) {
      cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
      exit(1);
   }
   fd1 = open(argv[1], O_RDONLY);
   if (fd1 < 0) {
      int terror = errno;

      cerr << "Couldn't open \"" << argv[1] << "\" for reading.\n";
      cerr << "error: " << strerror(terror) << endl;
      exit(2);
   }
   fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if (fd2 < 0) {
      int terror = errno;

      cerr << "Couldn't open \"" << argv[2] << "\" for writing.\n";
      cerr << "error: " << strerror(terror) << endl;
      exit(2);
   }
   fcntl(fd1, F_SETFL, fcntl(fd1, F_GETFL, 0) | O_NDELAY);
   fcntl(fd2, F_SETFL, fcntl(fd1, F_GETFL, 0) | O_NDELAY);

   {
      DynamicBuffer buf1;
      int result;

      buf1.resize(131072);
      assert(buf1.Length() > 0);
      errno = 0;
      result = read(fd1, buf1.getVoidP(), buf1.Length());
      if (result >= 0) {
	 buf1.resize(result);
      }
      while (result > 0 || errno == EWOULDBLOCK) {
	 if (buf1.Length() > 0) {
	    cout << "Got chunk of size " << buf1.Length() << endl;
	 }

	 {
	    GroupVector vec(buf1.NumSubGroups(LinearExtent::full_extent));
	    int start = 0, laststart = -1;

	    {
	       unsigned int i = 0;

	       buf1.FillGroupVec(LinearExtent::full_extent, vec, i);
	       cerr << "i == " << i << "\n";
	    }
	    while (start < vec.TotalLength()) {
	       GroupVector::IOVecDesc desc;

	       vec.FillIOVecDesc(start, desc);
//	    cerr << "desc.iovcnt == " << desc.iovcnt << "\n";

	       int temp = writev(fd2, desc.iov, desc.iovcnt);

	       if (temp >= 0) {
		  start += temp;
	       }
	       if (start != laststart) {
		  cout << "start == " << start << endl;
		  laststart = start;
	       }
	    }
	 }
	 buf1.resize(131072);
	 assert(buf1.Length() > 0);
	 errno = 0;
	 result = read(fd1, buf1.getVoidP(), buf1.Length());
	 if (result >= 0) {
	    buf1.resize(result);
	 }
      }
      cerr << result << ' ' << errno << endl;
      cerr << "Here 1-1\n";
      cerr << "Here 1-2\n";
   }
   cerr << "Here 1-3\n";
}
