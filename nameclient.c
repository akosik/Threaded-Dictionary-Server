//Client

#include "csapp.h"
#include "csapp.c"

void query_server(int fd, rio_t *rio, char *msg, char *rtrn) {
  Rio_writen(fd, msg, strlen(msg));
  Rio_readlineb(rio, rtrn, MAXLINE);
}

void output_key(){
  printf("\tPossible commands:\n");
  printf("\tlookup <key>\n");
  printf("\tadd <key> <value>\n");
  printf("\tdelete <key>\n");
}

int main(int argc, char **argv) 
{
  int clientfd, port;
  char *host;
  char buf[MAXLINE];
  char rtrn[MAXLINE];
  char command[MAXLINE];
  rio_t rio;
  char key[MAXLINE];

  if (argc != 3) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = atoi(argv[2]);

  clientfd = Open_clientfd(host, port);
  Rio_readinitb(&rio, clientfd);

  output_key();

  while (Fgets(buf, MAXLINE, stdin) != NULL) {
    sscanf(buf,"%s",command);
    int value = 0;

    if (strcmp(command,"lookup") == 0) {
      sscanf(buf,"%s %s",command,key);
      query_server(clientfd,&rio,buf,rtrn);
      if(strcmp(rtrn,"NULL.\n")) {
	sscanf(rtrn, "%d",&value);
	printf("Found it! The value of %s is %d.\n", key, value);
      }
      else{
	printf("Sorry, we couldn't find that value.\n");
      }
    }

    else if (strcmp(command,"add") == 0) {
      query_server(clientfd,&rio,buf,rtrn);
      if (strcmp("Ok.\n",rtrn) == 0) {
	printf("Done.\n");
      }
      else{
	printf("Entry insertion failed.\n");
      }
    }

    else if (strcmp(command,"delete") == 0) {
      query_server(clientfd,&rio,buf,rtrn);
      if (strcmp("Ok.\n",rtrn) == 0) {
	printf("Entry Deleted.\n");
      } else if (strcmp("NULL.\n",rtrn) == 0) {
	  printf("Entry deletion failed.\n");
	}
    }
    else {
      printf("Request not understood.\n");
      output_key();
    }
  }
  Close(clientfd); 
  exit(0);
}
