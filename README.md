# Threaded-Dictionary-Server
Server holds a dictionary (shared over all client threads) for querying.  Client code included in nameclient.c. The dictionary is designed to be a hashtable for finer grained locking.

# Functions
Includes add, delete, and lookup functions.
