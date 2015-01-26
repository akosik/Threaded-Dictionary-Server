
#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <sys/types.h>

typedef struct _listnode_t {

  char *string;
  int data;
  struct _listnode_t *next;
  struct _listnode_t *prev;

} listnode_t;

typedef struct _hashbucket_t {

  listnode_t *list;

  pthread_mutex_t mutex;
} hashbucket_t;

typedef struct _dictionary_t {

  int size;
  hashbucket_t *hashtable;

} dictionary_t;

void dictionary_init(dictionary_t *dict);

listnode_t *lookup(dictionary_t *dict, char *string, int hashvalue);

listnode_t *locked_lookup(dictionary_t *dict, char *string);

int hash(char *string);

void add_entry(int fd, dictionary_t *dict, char *string, int value);

void delete_entry(int fd, dictionary_t *dict,char *string);

#endif
