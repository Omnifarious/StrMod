#include <StrMod/DBStrChunk.h>
#include <StrMod/GroupVector.h>
#include <unistd.h>
#include <iostream.h>
#include <fcntl.h>
#include <errno.h>

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
      DataBlockStrChunk buf1;
      int result;

      buf1.Resize(131072);
      assert(buf1.Length() > 0);
      errno = 0;
      result = read(fd1, buf1.GetVoidP(), buf1.Length());
      if (result >= 0) {
	 buf1.Resize(result);
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
	 buf1.Resize(131072);
	 assert(buf1.Length() > 0);
	 errno = 0;
	 result = read(fd1, buf1.GetVoidP(), buf1.Length());
	 if (result >= 0) {
	    buf1.Resize(result);
	 }
      }
      cerr << result << ' ' << errno << endl;
      cerr << "Here 1-1\n";
      cerr << "Here 1-2\n";
   }
   cerr << "Here 1-3\n";
}
