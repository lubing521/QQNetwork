#ifndef ANET_TEST_H
#define ANET_TEST_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "anet.h"
#include "ae.h"
#include "zmalloc.h"
#define BUFLEN 1024
/*  Anti-warning macro... */
#define NOTUSED(V) ((void) V)


typedef struct Client{
    int fd;
    char addr[30];
    int port;
    char reply[BUFLEN];
    int replyLen;
} Client;

void freeClient(struct Client *c);
int add_reply(struct Client *c);
void readQueryFromClient(struct aeEventLoop *el, int fd, void *clientData, int mask);
void sendReplyToClient(struct aeEventLoop* el, int fd,void *clientData, int mask);
struct Client* craeteClient(int fd);
void acceptHandler(struct aeEventLoop *el,int fd, void *clientData,int mask);

#endif
