
/*
    Программа принимает три аргумента: строку с IPv4-адресом,
    и два неотрицательных целых числа, первое из которых определяет
    общее время работы программы timeout,
    а второе - время между отдельными запросами в микросекундах interval.

    Необходимо реализовать упрощённый аналог утилиты ping,
    которая определяет доступность удаленного хоста, используя протокол ICMP.

    Программа должна последовательно отправлять echo-запросы к указанному
   адресу и подсчитывать количество успешных ответов. Между запросами, во
   избежание большой нагрузки на сеть, необходимо выдерживать паузу в interval
   микросекунд (для этого можно использовать функцию usleep).

    Через timeout секунд необходимо завершить работу,
    и вывести на стандартный поток вывода количество полученных ICMP-ответов,
    соответствующих запросам.

    В качестве аналога можно посмотреть утилиту /usr/bin/ping.

    Указания: используйте инструменты ping и wireshark для того,
    чтобы исследовать формат запросов и ответов.
    Для того, чтобы выполняемый файл мог без прав администратора
    взаимодействовать с сетевым интерфейсом,
    нужно после компиляции установить ему capabilities командой:
    setcap cat_net_raw+eip PROGRAM.
    Контрольная сумма для ICMP-заголовков вычисляется по алгоритму из RFC-1071.
*/

#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef enum
{
  ERROR = -1,
  BUFFER_SIZE = 8192,
  HEAD_SIZE = 12,
  TALE_SIZE = 5,
  DNS_PORT = 53,
  PING_PKT_S = 64,
  PORT_NO = 0,
  RECV_TIMEOUT = 1
} constants;

// ping packet structure
typedef struct
{
  struct icmphdr hdr;
  char msg[PING_PKT_S - sizeof (struct icmphdr)];
} ping_pkt_t;

unsigned short
checksum (void *b, int len)
{
  unsigned short *buf = b;
  unsigned int sum = 0;
  unsigned short result;

  for (sum = 0; len > 1; len -= 2)
    {
      sum += *buf++;
    }
  if (len == 1)
    {
      sum += *(unsigned char *)buf;
    }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

void
send_ping (char *IPv4, uint64_t interval, uint64_t timeout)
{
  struct hostent *host_entity;
  host_entity = gethostbyname (IPv4);
  struct sockaddr_in ping_addr
      = { .sin_family = host_entity->h_addrtype,
          .sin_port = htons (PORT_NO),
          .sin_addr.s_addr = *(long *)host_entity->h_addr };

  int socket_ping = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
  int msg_count = 0;
  int msg_received_count = 0;

  ping_pkt_t ping_pkt;
  struct sockaddr_in recv_addr;
  struct timespec tfs, tfe;

  struct timeval tv_out;
  tv_out.tv_sec = RECV_TIMEOUT;
  tv_out.tv_usec = 0;
  setsockopt (socket_ping, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out,
              sizeof (tv_out));

  long double total_sec = 0;
  clock_gettime (CLOCK_MONOTONIC, &tfs);

  while (total_sec < timeout)
    {

      bzero (&ping_pkt, sizeof (ping_pkt));

      ping_pkt.hdr.type = ICMP_ECHO;
      ping_pkt.hdr.un.echo.id = getpid ();
      int i = 0;
      for (i = 0; i < sizeof (ping_pkt.msg) - 1; i++)
        {
          ping_pkt.msg[i] = i + '0';
        }
      ping_pkt.msg[i] = 0;
      ping_pkt.hdr.un.echo.sequence = msg_count++;
      ping_pkt.hdr.checksum = checksum (&ping_pkt, sizeof (ping_pkt));

      usleep (interval);

      int sen
          = sendto (socket_ping, &ping_pkt, sizeof (ping_pkt), 0,
                    (const struct sockaddr *)&ping_addr, sizeof (ping_addr));
      printf ("send: %d\n", sen);
      int addr_len = sizeof (recv_addr);
      if (sen = recvfrom (socket_ping, &ping_pkt, sizeof (ping_pkt), 0,
                    (struct sockaddr *)&recv_addr, &addr_len)
              <= 0
          && msg_count > 1)
        {

        }
      else
        {
          msg_received_count++;
        }
      printf ("rec: %d\n", sen);
      clock_gettime (CLOCK_MONOTONIC, &tfe);
      total_sec = ((double)(tfe.tv_sec - tfs.tv_sec))
                  + ((double)(tfe.tv_nsec - tfs.tv_nsec)) / 1000000000.0;
    }
  printf ("%d\n", msg_received_count);
}

int
main (int argc, char *argv[])
{
  char *IPv4 = argv[1];
  uint64_t timeout = strtoll (argv[2], NULL, 10);
  uint64_t interval = strtoll (argv[3], NULL, 10);
  send_ping (IPv4, interval, timeout);
}
