#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define IP_MAX 0xFFFFFFFF
#define PING_CMD "/bin/ping"
#define PING_FLAGS "-n -W 1 -c 1 -q"

int ping(const char* host) {
  FILE* fd;
  char cmd[256], res[512], *ptr;
  int i, tx, rx;

  snprintf(cmd, 256, "%s %s %s", PING_CMD, PING_FLAGS, host);

  fd = popen(cmd, "r");
  fread(&res, 512, 1, fd);
  fclose(fd);

  ptr = strstr(res, "packets transmitted");
  if (ptr == NULL) return -1;
  for (ptr != NULL; (*ptr != *res && *ptr != '\n'); ptr--);

  ptr++;
  sscanf(ptr, "%i packets transmitted, %i received", &tx, &rx);

  return (tx == rx) ? 1 : 0;
}

int main(int argc, char** argv) {
  struct in_addr in;
  struct hostent* h_ent, *reverse_ent;  
  uint32_t ipstart, ipstop;
  int cidr;
  char* ptr_rec, *a_rec;
  const char not_found[] = "Not found";
  const char not_available[] = "N/A";
  int mismatch;

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
    mismatch = 0;

    h_ent = gethostbyaddr(&in, sizeof(in), AF_INET);
    if (h_ent != NULL) {
      reverse_ent = gethostbyname(h_ent->h_name);
      ptr_rec = h_ent->h_name;

      if (reverse_ent != NULL && reverse_ent->h_length > 0) {
        a_rec =  inet_ntoa(*(struct in_addr*)*reverse_ent->h_addr_list);

        if (strcmp(inet_ntoa(in), a_rec) != 0) mismatch = 1; 
      } else {
        a_rec = (char*)not_found;
      }
    } else {
      a_rec = (char*)not_available;
      ptr_rec = (char*)not_found;
    }

    printf("%s\tIN PTR\t%-32s -> \tIN A: %-15s\tPING: %-5s%s\n",
        inet_ntoa(in), 
        ptr_rec,
        a_rec,
        (ping(inet_ntoa(in)) ? "UP" : "DOWN"),
        (mismatch) ? "\t(WARNING: A/PTR mismatch)" : ""
        );
  }
  return 0;
}
