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
