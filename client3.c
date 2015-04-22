/**********************************************************************\
*                Copyright (C) Justin Villani, 2014.                   *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty.                                                        *
\**********************************************************************/
#define _GNU_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h> // For exit()
#include <unistd.h> // For close()
#include <malloc.h>
#include <string.h>

void reading_info_from_server(int sock)
{
  char buffer[1000];
  size_t len=0;
  int newsock_fd;

  read(sock,buffer,100);

  printf("\n\n");

  printf("Rank  Title                             Weekend Box Office\n");
  printf("%s\n",buffer);
}

int main()
{
   int sock, addrsize;
   struct sockaddr_in addr;
   unsigned int in_address;

   char* input = NULL;
   int led;
   /* 
    * Open a socket for Internet stream services.
    */

   sock = socket(AF_INET, SOCK_STREAM,0);
   if (sock == -1)
   {   perror("opening socket");
       exit(-1);
   }

   /*
    * The address structure requires an address family (Internet), a port
    * for the server (32351) and an address for the machine (esus is
    * 153.90.192.1).  Note that anyting that is not defined or a byte
    * should be converted to the appropriate network byte order.  Look
    * in in.h to see the address structure itself.  Just for fun, look at the
    * function net_addr and see if you can convert 153.90.192.1 to
    * the address more easily.
    */

   addr.sin_family = AF_INET;
   addr.sin_port = htons (12345);
   in_address = 127 << 24 | 0 << 16 | 0 << 8 | 1;
   addr.sin_addr.s_addr = htonl (in_address);
   if (connect (sock, (struct sockaddr *) &addr, 
       sizeof (struct sockaddr_in)) == -1)
   {   
      perror("on connect");
      exit(-1);
   } 

   
   //This is going to be reading from the STDIN and will be then sending this 
   //information to the server to parse the movie.txt file. Once the file
   //is parsed then it will send the information back to the client to which
   //it will be printed on the screen
  for(;;)
  {
      printf("Please enter the title of the Movie or enter [q] to quit:\n");
      //led = read(STDIN_FILENO, input , 100);
      getline(&input, &led, stdin);

      if( led == -1)
      {
           perror("Error reading the following request: throwing away.");
      }
      
      input[strlen(input)-1] = '\0';

      if(strcmp(input,"q") == 0)
      {
          send(sock,input,led,0);
          break;
      }
      else
      {
        send(sock,input,led,0);
        reading_info_from_server(sock);
        continue;
      }
  }

   /* 
    * Do a shutdown to gracefully terminate by saying - "no more data"
    * and then close the socket -- the shutdown is optional in a one way
    * communication that is going to terminate, but a good habit to get
    * into. 
    */

   if (shutdown(sock, 1) == -1)
   {  
      perror("on shutdown");
      exit(-1);
   }
   printf ("Client is done\n");
   close(sock);
}


