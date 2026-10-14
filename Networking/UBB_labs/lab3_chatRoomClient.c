#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>


int main (int argc, char **argv)
{
  //Initialising socket
  struct sockaddr_in server;
  int socket_fd;
  printf ("Creating socket.\n");

  if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror ("Socket creation failed.\n");
      exit (EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    server.sin_port = htons(6767); //port
    server.sin_family =  AF_INET;
    server.sin_addr.s_addr = inet_addr("10.92.107.1"); //address

  fd_set read_fds;
  fd_set master;

  char buf[512];
  printf ("Connecting to server %s:%s.\n", argv[1], argv[2]);
  //Checking if the connection was successfull
  if (connect (socket_fd, (struct sockaddr *) &server, sizeof (server)) < 0)
    {
      perror ("Connection error.\n");
      exit (EXIT_FAILURE);
    }

  int n = recv (socket_fd, buf, sizeof (buf), 0);
  if (n > 0){
  	buf[n] = '\0';
  }
  printf ("%s\n", buf);

  FD_ZERO (&read_fds);		// clear the set
  FD_ZERO (&master);
  FD_SET (0, &master);
  FD_SET (socket_fd, &master);

  while (1)
    {
      read_fds = master;

      if (select (socket_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
	{
	  perror ("Select error.\n");
	  exit (EXIT_FAILURE);
	}


      int nbytes;
      if (FD_ISSET (0, &read_fds))
	{

	  char buf[512];

	  nbytes = read (0, buf, sizeof (buf) - 1);
	  int ret = send (socket_fd, buf, nbytes, 0);
	  if (ret <= 0)
	    {
	      perror ("Send error\n");
	      exit (EXIT_FAILURE);
	    }
	}
      if (FD_ISSET (socket_fd, &read_fds))
	{
	  nbytes = recv (socket_fd, buf, sizeof (buf) - 1, 0);
	  if (nbytes <= 0)
	    {
	      printf ("Server has closed connection...\n");
	      exit (2);
	    }

	  buf[nbytes] = '\0';
	  write (1, buf, nbytes);
	    }
    }
  return 0;
}
