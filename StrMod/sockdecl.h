#if defined(NEED_SOCKET_DECL)
#  ifdef __cplusplus
      extern "C" {
#  endif
int socket(int AddressFamily, int Type, int Protocol);
int bind(int Socket, struct sockaddr *Name, int NameLength);
int listen(int Socket, int Backlog);
int accept(int Socket, struct sockaddr *PeerAddr, int *AddrLength);
int connect(int Socket, struct sockaddr *Name, int NameLength);
#  ifdef __cplusplus
      }
#  endif
#endif

#ifdef HAVE_NO_SOCKLEN_T
#  ifdef SOCKLEN_IS_SIZE_T
#    include <cstddef>
typedef size_t socklen_t;
#  else
typedef int socklen_t;
#  endif // SOCKLEN_IS_SIZE_T
#endif // HAVE_NO_SOCKLEN_T
