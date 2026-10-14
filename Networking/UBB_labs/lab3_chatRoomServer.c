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

int
validate_number (char *str)
{
  while (*str)
    {
      if (!isdigit (*str))
	{			//if the character is not a number, return
	  return 0;
	}
      str++;			//point to next character
    }
  return 1;
}

int
validate_ip (char *ip)
{				//check whether the IP is valid or not

  if (ip == NULL)
    return 0;
  int num, dots = 0;
  char *ptr;
  char *aux = malloc (strlen (ip) * sizeof (char) + 1);
  memcpy (aux, ip, strlen (ip) + 1);
  ptr = strtok (aux, ".");	//cut the string using dor delimiter
  if (ptr == NULL)
    {
      free (aux);
      return 0;
    }
  while (ptr)
    {
      if (!validate_number (ptr))	//check whether the sub string is holding only number or not
	{
	  free (aux);
	  return 0;
	}
      num = atoi (ptr);		//convert substring to number
      if (num >= 0 && num <= 255)
	{
	  ptr = strtok (NULL, ".");	//cut the next part of the string
	  if (ptr != NULL)
	    dots++;		//increase the dot count
	}
      else
	{
	  free (aux);
	  return 0;
	}
    }
  if (dots != 3)		//if the number of dots are not 3, return false
    {
      free (aux);
      return 0;
    }

  free (aux);
  return 1;
}

int main (int argc, char **argv)
{

  //Validating cli arguments
/*
  if (argc < 3)
    {
      printf ("Give server and port.\n");
      return 1;
    }

  if (!validate_ip (argv[1]))
    {
      printf ("Server must be a valid ip address server.\n");
      return 2;
    }

  if (!validate_number (argv[2]))
    {
      printf ("Port must be integer.\n");
      return 3;
    }
*/

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
