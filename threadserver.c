//ThreadServer                                                                        

#include "dictionary.c"

typedef struct _session_t {

  dictionary_t *dictionary;
  int connfd;
  struct sockaddr_in clientaddr;
  pthread_t tid;
  rio_t rio;

} session_t;

void *dictionary_session(void *arg) {
  
  size_t n;
  char rw_buf[MAXLINE];
  char command[MAXLINE];
  int hashvalue;
  int value;
  char key[MAXLINE];
  
  session_t *sesh = arg;
  int fd = sesh->connfd;
  rio_t *rio = &sesh->rio;
  dictionary_t *dict = sesh->dictionary;

  while((n = Rio_readlineb(rio, rw_buf, MAXLINE)) != 0) {
    sscanf(rw_buf,"%s",command);
    
    if (strcmp(command,"lookup") == 0) {
      
      sscanf(rw_buf,"%s %s", command, key);
      listnode_t *found_entry = locked_lookup(dict,key);
      if (found_entry != NULL) {
	sprintf(rw_buf, "%d\n", found_entry->data);
	Rio_writen(fd, rw_buf, strlen(rw_buf));
      }
      else{
	Rio_writen(fd, "NULL.\n",6);
      }
      
    } else if (strcmp(command,"add") == 0) {
      sscanf(rw_buf,"%s %s %d", command, key, &value);
      add_entry(fd,dict,key,value);
      
    } else if (strcmp(command,"delete") == 0) {
      sscanf(rw_buf,"%s %s",command,key);
      delete_entry(fd,dict,key);
    }
    
  }

  Free(sesh);
  Close(fd);
  return NULL;
}

int main(int argc, char **argv)
{
  int listenfd, port;
  socklen_t clientlen;
  struct hostent *hp;
  session_t *sesh;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  port = atoi(argv[1]);
  
  static dictionary_t dict;
  printf("Initializing nameserver...\n");
  init_dictionary(&dict);
  
  listenfd = Open_listenfd(port);
  while (1) {

    sesh = (session_t*)Malloc(sizeof(session_t));
    sesh->dictionary = &dict;

    clientlen = sizeof(sesh->clientaddr);
    sesh->connfd = Accept(listenfd, (SA *)&(sesh->clientaddr), &clientlen);
    hp = Gethostbyaddr((const char *)&(sesh->clientaddr.sin_addr.s_addr),sizeof(sesh->clientaddr.sin_addr.s_addr), AF_INET);
    printf("Connection with %s.\n", hp->h_name);

    Rio_readinitb(&sesh->rio,sesh->connfd);

    Pthread_create(&sesh->tid, NULL, dictionary_session, sesh);
    
  }
  exit(0);
}
