//
//  cclient.c
//  cclient
//
//  Created by T.J. Stone on 4/20/15.
//  Copyright (c) 2015 T.J. Stone. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "testing.h"
#include <stdlib.h>
#include <string.h>
#include "cclient.h"

#define PROMPT_LEN 25
#define SERVER_NAME 250
#define MESSAGE_LEN 1000

#define INIT_PCKT_FLAG 1
#define INIT_ACK_FLAG 2
#define INIT_FAIL_FLAG 3
#define BROADCAST_FLAG 4
#define MESSAGE_FLAG 5
#define DEST_FOUND_FLAG 6
#define DEST_NOT_FOUND_FLANG 7
#define EXIT_FLAG 8
#define EXIT_ACK_FLAG 9
#define HANDLES_FLAG 10
#define HANDLES_RESP_FLAG 11

/**
 * Checks for correct commands. (%M %B %L %E)
 */
int verifyInput(char *input) {
   return ((strcmp(input, "%M") == 0)
           || (strcmp(input, "%m") == 0)
           || (strcmp(input, "%B") == 0)
           || (strcmp(input, "%b") == 0)
           || (strcmp(input, "%L") == 0)
           || (strcmp(input, "%l") == 0)
           || (strcmp(input, "%E") == 0)
           || (strcmp(input, "%e") == 0));
}

void sendMessage(int socket_num, char input[]) {
   char *send_buf;         //data buffer
   int send_len= 0;        //amount of data to send
   int sent= 0;            //actual amount of data sent
   int bufsize = 1024;     //data buffer size
   
   /* initialize data buffer for the packet */
   send_buf= (char *) malloc(bufsize);
   
   send_len = 0;
   while ((send_buf[send_len] = getchar()) != '\n' && send_len < 80)
      send_len++;
   
   send_buf[send_len] = '\0';
   
   /* now send the data */
   sent =  send(socket_num, send_buf, send_len, 0);
   
   if(sent < 0)
   {
      perror("send call");
      exit(-1);
   }
   
   printf("String sent: %s \n", send_buf);
   printf("Amount of data sent is: %d\n", sent);
   
}


void checkCommand(int socket_num, char input[]) {
   char command = input[1];
   
   if (command == 'M' || command == 'm') {
      sendMessage(socket_num, input);
   }
}

void run(int socket_num) {
   char input[PROMPT_LEN];
   int exit = 0;
   
   while (!exit) {
      printf("$:\n");
      scanf("%s", input);
      
      if (verifyInput(input)) {
         if (strcmp(input, "%e") == 0
             || strcmp(input, "%E") == 0) {
            exit = 1;
         }
         checkCommand(socket_num, input);
      }
      else {
         printf("Unkown Command\n");
      }
   }
}


int main(int argc, char * argv[])
{
   int socket_num;         //socket descriptor
   char *send_buf;         //data buffer
   int bufsize= 0;         //data buffer size
   int send_len= 0;        //amount of data to send
   int sent= 0;            //actual amount of data sent
   
   /* check command line arguments  */
   if (argc != 4) {
      printf("\nINCORRECT INPUT\n Usage: ");
      printf("Server IP Address, Server Port Number, Client Handle (name).\n");
      return -1;
   }
   
   /* set up the socket for TCP transmission  */
   socket_num= tcp_send_setup(argv[2], argv[3]);
   
   run(socket_num);
   
   
   close(socket_num);
   return 0;
   
}



int tcp_send_setup(char *host_name, char *port)
{
   int socket_num;
   struct sockaddr_in remote;       // socket address for remote side
   struct hostent *hp;              // address of remote host

   // create the socket
   if ((socket_num = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("socket call");
      exit(-1);
   }
   
   
   // designate the addressing family
   remote.sin_family= AF_INET;
   
   // get the address of the remote host and store
   if ((hp = gethostbyname(host_name)) == NULL)
   {
      printf("Error getting hostname: %s\n", host_name);
      exit(-1);
   }
   
   memcpy((char*)&remote.sin_addr, (char*)hp->h_addr, hp->h_length);
   
   // get the port used on the remote side and store
   remote.sin_port= htons(atoi(port));
   
   if(connect(socket_num, (struct sockaddr*)&remote, sizeof(struct sockaddr_in)) < 0)
   {
      perror("connect call");
      exit(-1);
   }
   
   return socket_num;
}



/*int main (int argc, const char *argv[]) {
   char srvName[SERVER_NAME];
   unsigned int srvPrt = 0;
   char cHndl[SERVER_NAME];
   
   
   if (argc < 4) {
      printf("\nINCORRECT INPUT\n Usage: ");
      printf("Server IP Address, Server Port Number, Client Handle (name).\n");
      return -1;
   }
   
   
   
   strcpy(cHndl, (char *) argv[1]);
   strcpy(srvName, (char *) argv[2]);
   srvPrt = (int) (argv[3] - '0');
   
   run();
   
   return 0;
}*/