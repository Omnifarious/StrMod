#include <StrMod/StrChunk.h>
#include <unistd.h>
#include <iostream.h>
#include <fcntl.h>
#include <errno.h>

extern char *sys_errlist[];
extern int errno;

int main(int argc, char *argv[])
{
   int fd1, fd2, result;
   StrChunk *buf1;

   if (argc != 3) {
      cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
      exit(1);
   }
   if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
      int terror = errno;

      cerr << "Couldn't open \"" << argv[1] << "\" for reading.\n";
      cerr << "error: " << sys_errlist[terror] << endl;
      exit(2);
   }
   fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if (fd2 < 0) {
      int terror = errno;

      cerr << "Couldn't open \"" << argv[2] << "\" for writing.\n";
      cerr << "error: " << sys_errlist[terror] << endl;
      exit(2);
   }
   fcntl(fd1, F_SETFL, fcntl(fd1, F_GETFL, 0) | FNDELAY);
   fcntl(fd2, F_SETFL, fcntl(fd1, F_GETFL, 0) | FNDELAY);
   buf1 = new DataBlockStrChunk();
   result = buf1->FillFromFd(fd1);
   while (result > 0 || buf1->GetLastErrno() == EWOULDBLOCK) {
      int start = 0;

      if (buf1->Length() > 0)
	 cout << "Got chunk of size " << buf1->Length() << endl;
      while (start != buf1->Length()) {
	 int temp = buf1->PutIntoFd(fd2, start);

	 if (temp >= 0)
	    start += temp;
	 cout << "start == " << start << endl;;
      }
      result = buf1->FillFromFd(fd1);
   }
   cerr << result << ' ' << buf1->GetLastErrno() << endl;
   delete buf1;
}
