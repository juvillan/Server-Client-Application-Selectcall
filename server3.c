/* ======================================================> server1.c 
 * Generic sort of server process for Unix 
 *
 * This is an extremely simple use of sockets for communication.
 * The server opens a socket and then reads messages and prints them out
 * without further ado until the client closes the socket. 
 * =====================================================================
 */

 /**********************************************************************\
*                Copyright (C) Justin Villani, 2014.                   *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty.                                                        *
\**********************************************************************/

#define _XOPEN_SOURCE //this is used to compile with -c=99

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>

int sorting_movie_file(int connection_of_client);

void sig_handler(int signo)
{
    pid_t temp;

    //Cleaning up all the child proccess
    //The WNOHANG is the flag being used so if it is blocked i dont have to take care of it.
    while((temp = waitpid(-1,NULL,WNOHANG)) > 0)
    {
      printf("Child process %d terminated\n",temp);
    }

}

int main()
{
   int sock, clientsock, mlen, addrsize, msgct, chc, chct, test=0;
   struct sockaddr_in addr;
   char ch, buf[80];
   char input[30];

   //Setting up the SIGCLD Handler to clean up zombie process
   struct sigaction sa;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;
   sa.sa_handler = sig_handler;
   //This is calling the sigaction to handle the zombie child process
   sigaction(SIGCLD,&sa,NULL);

   //Setting up the FD_set for the select function later on.
   fd_set client_set;
   int clientset[100];
   int number_of_fd=0;
   int max_fd=0;

   /*
    * Create a socket.
    */

   sock = socket(AF_INET, SOCK_STREAM,0);
   if (sock == -1)
   {   
      perror("opening socket");
      exit(-1);
   }

   /*
    * Bind a name to the socket.  Since the server will bind with
    * any client, the machine address is zero or INADDR_ANY.  The port
    * has to be the same as the client uses.
    */

   addr.sin_family = AF_INET;
   addr.sin_port = htons (12345);
   addr.sin_addr.s_addr = htonl (INADDR_ANY);

   if (bind(sock, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)) == -1) 
   {  
      perror ("on bind");
      exit (-1);
   }

   /*
    * Make the socket available for potential clients.
    */

   if (listen(sock,1) == -1)  
   {  
      perror("on listen");
      exit(-1);
   }

   /*
    * Wait for a client to connect.  When this happens, print out a 
    * message including the address of the client.  Note that clientsock
    * is a socket just like sock, except that it is specifically connected
    * to the client.  sock is used just to get connections.
    */

   addrsize = sizeof(struct sockaddr_in);
   int process;


   max_fd = sock;
   int SockReady=0;
   int x=0;
   FD_SET(sock,&client_set);
   clientset[0]=sock;

   for(;;)
   {
   			//calling the select() here
   			SockReady = select(max_fd+1,&client_set,NULL,NULL,NULL);
   			//Calling the FD_ISSET then accept the client
   			if(SockReady < 0)
   			{
   				perror("The fd_set had a probem");
   			}
   			else if(SockReady > 0)
   			{
   				for(int i=0;i<number_of_fd+1;i++)
   				{
   					if(FD_ISSET(clientset[i],&client_set)==1) //This is checking if anything is in the client set
   					{
   						if(FD_ISSET(sock,&client_set)==1) //This is checking if the server is in the client set
   						{
   							clientsock = accept(sock, (struct sockaddr *) &addr, &addrsize);
                number_of_fd++;

   							clientset[number_of_fd]=clientsock;

   							printf("connection made with client ");
                printf ("%s\n", inet_ntoa (addr.sin_addr));

   							if(clientsock > max_fd)
            		{
            				max_fd = clientsock;
            		}

   						}
   						else
   						{
                //printf("I am now receiving from the client");
   							test = sorting_movie_file(clientsock);

            				if(test == -1)
           					{
                      //Shift through array and remove the client from the list
                      int y;
                      for(y=i;y<number_of_fd;y++)
                      {
                        clientset[y]=clientset[y+1];
                      }
                      clientset[y] = -1;

           						 shutdown(clientsock, SHUT_RDWR);
                			 close(clientsock);
                 				break;
            				}
   					  }
   					}
   					
   				}

            FD_ZERO(&client_set);
            //The reason why it wasnt working was becuase i was starting at 1 and not 0.
            for(int j=0; j<number_of_fd+1;j++)
            {
              FD_SET(clientset[j], &client_set);
            }
   			}     
   }
}

int sorting_movie_file(int connection_of_client)
{
    FILE *pFile;
    char buffer[100];
    char filebuffer[100];
    int message;
    char * found;

    //Opening movie.txt file
    pFile = fopen("movie.txt","r");
    if(pFile == NULL)
    {
      perror("File couldnt open");
    }

    //This is reading from the socket for what the client sent to the server
    if(!(message = recv(connection_of_client,buffer,80,0)))
    {
    	//telling the client that it has quit, so FD_CLR() of the FD
        return(-1);
    }

    buffer[message-1]= '\0';

    if (strlen(buffer) == 0)
    {
      fclose(pFile);
      return(1);
    }

    //printf("Buffer length: %d\n buffer: %s\n",strlen(buffer), buffer);

    if(message < 0)
    {
      perror("Couldn't read from the socket!!");
    }

    //Ingnoring the firstline
    if(fgets(filebuffer,100,pFile)!=NULL);

    //This has read the file. Ingnoring the first line
    for(int x=0; x<5 ;x++)
    {
        fgets(filebuffer,100,pFile);
        //printf("%s",filebuffer);

        found = strstr(filebuffer,buffer);

        //If it is found then you will send it to the file
        if(found != NULL)
        {
          //printf("%s",filebuffer);
          message = send(connection_of_client,filebuffer,strlen(filebuffer)+1,0);
          //printf("%d",message);
          break;
        }
    }

    //This is sending that the file wasnt in there
    if(found == NULL)
    {
      message = write(connection_of_client,"No Results Found\n\n",19);
    }

    if(message < 0)
    {
      perror("Couldn't write to the socket!!");
    }

    fclose(pFile);

    if(strcmp(buffer,"q")==0)
    {
      return(-1);
    }
    else
    {
      return(1);
    }
}
