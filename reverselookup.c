#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define IP_MAX 0xFFFFFFFF

int main(int argc, char** argv) {
  struct in_addr in;
  struct hostent* h_ent;  
  uint32_t ipstart, ipstop;
  int cidr;

  if (argc < 3) {
    printf("Usage: %s <subnet> <cidr>\n", argv[0]);
    exit(1);
  }

  ipstart = inet_addr(argv[1]);  
  if (ipstart == INADDR_NONE) {
    fprintf(stderr, "%s is not a valid ip address.\n", argv[1]);
    exit(1);
  }

  cidr = atoi(argv[2]);
  ipstart = ntohl(ipstart);
  ipstop = ipstart + (IP_MAX >> cidr);

  while(ipstart++ < ipstop) {
   in.s_addr = htonl(ipstart);

    h_ent = gethostbyaddr(&in, sizeof(in), AF_INET);
    printf("%s - %s\n", inet_ntoa(in), (h_ent != NULL) ? h_ent->h_name : "");
  } 

  return 0;
}
