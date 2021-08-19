//#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
//#include <local.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
using namespace std;

// To store client's socket list
list<int> clients_list;

// for debug mode
int DEBUG_MODE = 0;

int
main (int argc, char *argv[])
{
  // *** Define debug mode
  //     any additional parameres on startup
  //     i.e. like './server f' or './server debug'
  //     we will switch to switch to debug mode(very simple anmd useful)
  if (argc > 1)
    DEBUG_MODE = 1;

  if (DEBUG_MODE)
    {
      printf ("Debug mode is ON!\n");
      printf ("MAIN: argc = %d\n", argc);
      for (int i = 0; i < argc; i++)
        printf (" argv[%d] = %s\n", i, argv[i]);
    }
  else
    printf ("Debug mode is OFF!\n");

  // *** Define values
  //     main server listener
  int listener;

  // define ip & ports for server(addr)
  //     and incoming client ip & ports(their_addr)
  struct sockaddr_in addr, their_addr;
  //     configure ip & port for listen
  addr.sin_family = PF_INET;
  addr.sin_port = htons (SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr (SERVER_HOST);

  //     size of address
  socklen_t socklen;
  socklen = sizeof (struct sockaddr_in);

  //     event template for epoll_ctl(ev)
  //     storage array for incoming events from epoll_wait(events)
  //        and maximum events count could be EPOLL_SIZE
  static struct epoll_event ev, events[EPOLL_SIZE];
  //     watch just incoming(EPOLLIN)
  //     and Edge Trigged(EPOLLET) events
  ev.events = EPOLLIN | EPOLLET;

  //     chat message buffer
  char message[BUF_SIZE];

  //     epoll descriptor to watch events
  int epfd;

  //     to calculate the execution time of a program
  clock_t tStart;

  // other values:
  //     new client descriptor(client)
  //     to keep the results of different functions(res)
  //     to keep incoming epoll_wait's events count(epoll_events_count)
  int client, res, epoll_events_count;

  // *** Setup server listener
  //     create listener with PF_INET(IPv4) and
  //     SOCK_STREAM(sequenced, reliable, two-way, connection-based byte
  //     stream)
  CHK2 (listener, socket (PF_INET, SOCK_STREAM, 0));
  printf ("Main listener(fd=%d) created! \n", listener);

  //    setup nonblocking socket
  setnonblocking (listener);

  //    bind listener to address(addr)
  CHK (bind (listener, (struct sockaddr *)&addr, sizeof (addr)));
  printf ("Listener binded to: %s\n", SERVER_HOST);

  //    start to listen connections
  CHK (listen (listener, 1));
  printf ("Start to listen: %s!\n", SERVER_HOST);

  // *** Setup epoll
  //     create epoll descriptor
  //     and backup store for EPOLL_SIZE of socket events
  CHK2 (epfd, epoll_create (EPOLL_SIZE));
  printf ("Epoll(fd=%d) created!\n", epfd);

  //     set listener to event template
  ev.data.fd = listener;

  //     add listener to epoll
  CHK (epoll_ctl (epfd, EPOLL_CTL_ADD, listener, &ev));
  printf ("Main listener(%d) added to epoll!\n", epfd);

  // *** Main cycle(epoll_wait)
  while (1)
    {
      CHK2 (epoll_events_count,
            epoll_wait (epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
      if (DEBUG_MODE)
        printf ("Epoll events count: %d\n", epoll_events_count);
      // setup tStart time
      tStart = clock ();

      for (int i = 0; i < epoll_events_count; i++)
        {
          if (DEBUG_MODE)
            {
              printf ("events[%d].data.fd = %d\n", i, events[i].data.fd);
              debug_epoll_event (events[i]);
            }
          // EPOLLIN event for listener(new client connection)
          if (events[i].data.fd == listener)
            {
              CHK2 (client, accept (listener, (struct sockaddr *)&their_addr,
                                    &socklen));
              if (DEBUG_MODE)
                printf ("connection from:%s:%d, socket assigned to:%d \n",
                        inet_ntoa (their_addr.sin_addr),
                        ntohs (their_addr.sin_port), client);
              // setup nonblocking socket
              setnonblocking (client);

              // set new client to event template
              ev.data.fd = client;

              // add new client to epoll
              CHK (epoll_ctl (epfd, EPOLL_CTL_ADD, client, &ev));

              // save new descriptor to further use
              clients_list.push_back (
                  client); // add new connection to list of clients
              if (DEBUG_MODE)
                printf ("Add new client(fd = %d) to epoll and now "
                        "clients_list.size = %d\n",
                        client, clients_list.size ());

              // send initial welcome message to client
              bzero (message, BUF_SIZE);
              res = sprintf (message, STR_WELCOME, client);
              CHK2 (res, send (client, message, BUF_SIZE, 0));
            }
          else
            { // EPOLLIN event for others(new incoming message from client)
              CHK2 (res, handle_message (events[i].data.fd));
            }
        }
      // print epoll events handling statistics
      printf ("Statistics: %d events handled at: %.2f second(s)\n",
              epoll_events_count,
              (double)(clock () - tStart) / CLOCKS_PER_SEC);
    }

  close (listener);
  close (epfd);

  return 0;
}

// *** Handle incoming message from clients
int
handle_message (int client)
{
  // get row message from client(buf)
  //     and format message to populate(message)
  char buf[BUF_SIZE], message[BUF_SIZE];
  bzero (buf, BUF_SIZE);
  bzero (message, BUF_SIZE);

  // to keep different results
  int len;

  // try to get new raw message from client
  if (DEBUG_MODE)
    printf ("Try to read from fd(%d)\n", client);
  CHK2 (len, recv (client, buf, BUF_SIZE, 0));

  // zero size of len mean the client closed connection
  if (len == 0)
    {
      CHK (close (client));
      clients_list.remove (client);
      if (DEBUG_MODE)
        printf ("Client with fd: %d closed! And now clients_list.size = %d\n",
                client, clients_list.size ());
      // populate message around the world
    }
  else
    {

      if (clients_list.size () == 1)
        { // this means that noone connected to server except YOU!
          CHK (send (client, STR_NOONE_CONNECTED, strlen (STR_NOONE_CONNECTED),
                     0));
          return len;
        }

      // format message to populate
      sprintf (message, STR_MESSAGE, client, buf);

      // populate message around the world ;-)...
      list<int>::iterator it;
      for (it = clients_list.begin (); it != clients_list.end (); it++)
        {
          if (*it != client)
            { // ... except youself of course
              CHK (send (*it, message, BUF_SIZE, 0));
              if (DEBUG_MODE)
                printf ("Message '%s' send to client with fd(%d) \n", message,
                        *it);
            }
        }
      if (DEBUG_MODE)
        printf ("Client(%d) received message successfully:'%s', a total of %d "
                "bytes data...\n",
                client, buf, len);
    }

  return len;
}