#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

enum constants
{
  PORT = 0,
  DEFAULT_TIMEOUT = 1,
  PACKET_SIZE = 64
};

typedef struct
{
  struct icmphdr header;
  char message[PACKET_SIZE - sizeof (struct icmphdr)];
} PingPacket;

uint16_t
ComputeCheckSum (void *void_buffer, int size)
{
  uint16_t *buffer = (uint16_t *)void_buffer;
  uint16_t result = 0;
  uint32_t sum = 0;
  int new_size = size / 2;
  for (int i = 0; i < new_size; ++i)
    {
      sum += buffer[i];
    }
  if ((size % 2) == 1)
    {
      sum += ((uint8_t *)buffer)[size - 1];
    }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

uint64_t
CurrTime ()
{
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

void
ClientFunction (const char *hostname, uint64_t interval, uint64_t timeout)
{
  struct hostent *hp = gethostbyname (hostname);
  assert (hp && hp->h_addrtype == AF_INET);
  struct sockaddr_in whereto
      = { .sin_family = hp->h_addrtype, .sin_port = htons (PORT) };
  memcpy (&whereto.sin_addr, hp->h_addr, hp->h_length);

  int sock_fd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
  int ttl_val = PACKET_SIZE;
  setsockopt (sock_fd, SOL_IP, IP_TTL, &ttl_val, sizeof (ttl_val));
  struct timeval tv_out;
  tv_out.tv_sec = DEFAULT_TIMEOUT;
  tv_out.tv_usec = 0;
  setsockopt (sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out,
              sizeof (tv_out));

  uint64_t overall_nsec = 0;
  uint64_t max_nsec = timeout * 1000000000L;
  size_t packet_id = 0;
  size_t received_cnt = 0;

  uint64_t start_time = CurrTime ();

  while (overall_nsec < max_nsec)
    {
      PingPacket packet;
      bzero (&packet, sizeof (PingPacket));
      packet.header.type = ICMP_ECHO;
      packet.header.un.echo.id = getpid ();
      packet.header.un.echo.sequence = packet_id;
      packet.header.checksum = ComputeCheckSum (&packet, sizeof (packet));
      ++packet_id;
      usleep (interval);
      int a = sendto (sock_fd, &packet, sizeof (packet), 0,
                      (const struct sockaddr *)&whereto, sizeof (whereto));
      printf ("sen : %d\n", a);
      struct sockaddr_in from;
      int addr_len = sizeof (from);
      int recv_ret = recvfrom (sock_fd, &packet, sizeof (packet), 0,
                               (struct sockaddr *)&from, &addr_len);
      printf ("rec : %d\n", recv_ret);
      if (recv_ret > 0)
        {
          ++received_cnt;
        }
      uint64_t end_time = CurrTime ();
      overall_nsec = end_time - start_time;
    }
  printf ("%ld\n", received_cnt);
}

int
main (int argc, char *argv[])
{
  char *hostname = argv[1];
  uint64_t timeout = strtoll (argv[2], NULL, 10);
  uint64_t interval = strtoll (argv[3], NULL, 10);
  ClientFunction (hostname, interval, timeout);
  return 0;
}