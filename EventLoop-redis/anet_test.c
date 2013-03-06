#include "anet_test.h"

char error[1024];
int maxclients = 50;
int stat_numconnections = 0;
struct aeEventLoop* el;

void freeClient(struct Client *c)
{
    close(c->fd);
    fprintf(stdout,"end communication, cut off connection with");
    fprintf(stdout," [%s:%d]\n",c->addr,c->port);
    zfree(c);
    aeDeleteFileEvent(el,c->fd, AE_WRITABLE);
    aeDeleteFileEvent(el,c->fd, AE_READABLE);
    --stat_numconnections;
}

int add_reply(struct Client* c)
{
    if(aeCreateFileEvent(el, c->fd, AE_WRITABLE,
                    sendReplyToClient, c) == AE_ERR) return -1;
    return 0;
}
void sendReplyToClient(struct aeEventLoop *el, int fd,
        void *clientData, int mask){
    int nwrite;
    struct Client* c = (struct Client*) clientData;
    nwrite = write(fd,c->reply,c->replyLen);
    if(nwrite < 0){
        if (errno == EAGAIN) {
            nwrite = 0;
        }else {
            fprintf(stderr,"send error\n");
            freeClient(c);
            return;
        }
    }

    if(nwrite != 0){
        aeDeleteFileEvent(el,c->fd,AE_WRITABLE);
    }
}
void readQueryFromClient(struct aeEventLoop *el, int fd, 
        void *clientData, int mask){
    char buf[BUFLEN];
    int nread;
    NOTUSED(el);
    NOTUSED(mask);
    struct Client *c = (struct Client*) clientData;
    nread = read(fd,buf,BUFLEN);
    if(nread == -1){
        fprintf(stderr,"read error\n");
        freeClient(c);
        return;
    }else if(nread == 0){
        freeClient(c);
    }else{
        strncpy(c->reply,buf,nread);
        c->replyLen = nread;
        if( add_reply(c)<0 ){
            freeClient(c);
            return;
        }
    }
}
struct Client* createClient(int fd){
    struct Client *c = zmalloc(sizeof(struct Client));
    c->replyLen=0;
    c->fd = fd;
    anetNonBlock(NULL,fd);
    anetTcpNoDelay(NULL,fd);
    if (aeCreateFileEvent(el,fd,AE_READABLE,
        readQueryFromClient, c) == AE_ERR)
    {
        close(fd);
        zfree(c);
        return NULL;
    }
    return c;
}
void acceptHandler(struct aeEventLoop *eventLoop, int fd, void *clientData, 
        int mask)
{
    int cport, cfd;
    char cip[128];
    char error[128];
    struct Client *c;
    NOTUSED(el);
    NOTUSED(mask);
    NOTUSED(clientData);
    fprintf(stdout,"DEBUG:accept\n");
    cfd = anetTcpAccept(error, fd, cip, &cport);
    if (cfd == AE_ERR) {
        fprintf(stderr,"Accepting client connection: %s", error);
        return;
    }
    fprintf(stdout,"Accepted %s:%d\n", cip, cport);
    if ((c = createClient(cfd)) == NULL) {
        fprintf(stderr,"Error allocating resoures for the client");
        close(fd);
        return;
    }
    if (stat_numconnections > maxclients) {
        char *err = "-ERR max number of clients reached\r\n";
        fprintf(stderr,"%s\n",err);
        freeClient(c);
        return;
    }
    stat_numconnections++;
}
/*
 * An Echo Server with ae eventloop
 */
int
main(int argc, char** argv){
    int listenfd;
    //int status,n;
    //char buf[BUFLEN];
    listenfd = anetTcpServer(error,9999,"127.0.0.1");
    if(listenfd < 0){
        fprintf(stderr,"%s\n",error);
        exit(1);
    }
    el = aeCreateEventLoop();
    if( aeCreateFileEvent(el,listenfd,AE_READABLE,acceptHandler,NULL) == AE_ERR)
    {
        fprintf(stderr,"creating listening file event\n");
        exit(1);
    }
    aeMain(el);
    return 0;
}
