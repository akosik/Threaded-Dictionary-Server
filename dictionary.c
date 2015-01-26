#include "dictionary.h"
#include "csapp.c"

//init_dictionary initializes the hashtable and malloc's a front sentinel node.

void init_dictionary(dictionary_t *dict) {

  dict->hashtable = (hashbucket_t*)malloc(23 * sizeof(hashbucket_t));
  for(int i = 0; i<23;i++) {
    dict->hashtable[i].list = (listnode_t*)malloc(sizeof(listnode_t));
    dict->hashtable[i].list->next = NULL;
    pthread_mutex_init(&dict->hashtable[i].mutex, NULL);
  }
}

//malloc's a listnode and loops through the hashbucket's linked list to find a matching key.

listnode_t *lookup(dictionary_t *dict,char *string,int hashvalue) {
  listnode_t *search = (listnode_t*)malloc(sizeof(listnode_t));
  for(search = dict->hashtable[hashvalue].list->next; search != NULL; search = search->next) {
    if (!strcmp(string,search->string)) {
      return search;
    }
  }
  return NULL;
}

//same as above, with locks. lookup is always nested in locked functions so it doesn't need locks like this.

listnode_t *locked_lookup(dictionary_t *dict, char *string) {
  int hashvalue = hash(string);
  pthread_mutex_lock(&dict->hashtable[hashvalue].mutex);
  listnode_t *found_entry = lookup(dict,string,hashvalue);
  pthread_mutex_unlock(&dict->hashtable[hashvalue].mutex);
  return found_entry;
}

//simple hash

int hash(char *string) {
  int hashvalue = 0;

  for(;*string!= '\0';string++) {
    hashvalue = (hashvalue * 27 + *string - 'a' - 1) % 23;
  }
  return hashvalue;
}

//checks if the entry is already there, otherwise slips the node into the linked list at the appropriate hashbucket.

void add_entry(int fd, dictionary_t *dict, char *string, int value) {
  int hashvalue = hash(string);
  pthread_mutex_lock(&dict->hashtable[hashvalue].mutex);
  if (lookup(dict,string,hashvalue) != NULL) {
    Rio_writen(fd, "NULL.\n",6);
  }
  else{
    listnode_t *new_entry = (listnode_t*)malloc(sizeof(listnode_t));
    new_entry->string = (char*) string;
    new_entry->data = value;
    if(dict->hashtable[hashvalue].list->next != NULL) {
      dict->hashtable[hashvalue].list->next->prev = new_entry;
    }
    new_entry->next = dict->hashtable[hashvalue].list->next;
    new_entry->prev = dict->hashtable[hashvalue].list;
    dict->hashtable[hashvalue].list->next = new_entry;
    Rio_writen(fd,"Ok.\n",4);
  }
  pthread_mutex_unlock(&dict->hashtable[hashvalue].mutex);
}

//looks up node, rearranges surrounding next's and prev's and frees the node.

void delete_entry(int fd, dictionary_t *dict,char *string) {
  listnode_t *THEnode = (listnode_t*)malloc(sizeof(listnode_t));
  int hashvalue = hash(string);
  pthread_mutex_lock(&dict->hashtable[hashvalue].mutex);
  THEnode = lookup(dict,string,hashvalue);
  if(THEnode != NULL) {
    THEnode->prev->next = THEnode->next;
    if(THEnode->next != NULL) {
      THEnode->next->prev = THEnode->prev;
    }
    THEnode->data = 0;
    THEnode->string = NULL;
    Free(THEnode);
    Rio_writen(fd,"Ok.\n",4);
  }
  else{
    Rio_writen(fd,"NULL.\n",6);
  }
  pthread_mutex_unlock(&dict->hashtable[hashvalue].mutex);
}
