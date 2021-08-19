#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

enum
{
  PACKAGE = 64
};

unsigned short
checksum (void *b, int size)
{
  unsigned short *buf = b;
  unsigned int chk_sm = 0;
  unsigned short result;
  for (chk_sm = 0; size > 1; size -= 2)
    chk_sm += *buf++;
  if (size == 1)
    chk_sm += *(unsigned char *)buf;
  chk_sm = (chk_sm >> 16) + (chk_sm & 0xFFFF);
  chk_sm += (chk_sm >> 16);
  return ~chk_sm;
}

typedef struct
{
  struct icmphdr hdr;
  char msg[PACKAGE - sizeof (struct icmphdr)];
} ping_package;

uint64_t
current_time ()
{
  struct timespec tm;
  clock_gettime (CLOCK_MONOTONIC, &tm);
  return tm.tv_sec * 1000000000L + tm.tv_nsec;
}

int
main (int argc, char *argv[])
{
  uint64_t timeout = strtoull (argv[2], NULL, 10);
  uint64_t interval = strtoull (argv[3], NULL, 10);

  int socket_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

  int ttl_val = PACKAGE;
  setsockopt (socket_fd, SOL_IP, IP_TTL, &ttl_val, sizeof (ttl_val));

  struct timeval iter_time;
  iter_time.tv_sec = 1;
  iter_time.tv_usec = 0;
  setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&iter_time,
              sizeof (iter_time));

  struct sockaddr_in addr
      = { .sin_family = gethostbyname (argv[1])->h_addrtype,
          .sin_port = htons (0),
          .sin_addr.s_addr = *(long *)gethostbyname (argv[1])->h_addr };

  int recieved_packeges = 0;

  uint64_t start = current_time ();
  uint64_t overall = timeout * 1000000000L;
  uint64_t passed_time = 0;
  int i = 0;

  while (passed_time < overall)
    {
      ping_package pckt;
      bzero (&pckt, sizeof (ping_package));
      pckt.hdr.type = ICMP_ECHO;
      usleep (interval);

      int sen = sendto (socket_fd, &pckt, sizeof (pckt), 0,
                        (struct sockaddr *)&addr, sizeof (addr));
      int recv_len = sizeof (struct sockaddr_in);
      struct sockaddr_in back_addr;
      int rec = recvfrom (socket_fd, &pckt, sizeof (pckt), 0,
                          (struct sockaddr *)&back_addr, &recv_len);
      if (rec > 0)
        {
          ++recieved_packeges;
        }
      passed_time = current_time () - start;
    }
  printf ("%d", recieved_packeges);
  close (socket_fd);
  return 0;
}