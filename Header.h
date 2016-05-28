#ifndef HEAD
#define HEAD
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <pthread.h>

#define LISTENQ 10
#define SERV_PORT 8000
#define MAXLINE 3000
#define BUFFSIZE 2048

#define TEST -1
#define DEFAULT 0
#define LOGIN 1
#define LOGOUT 2
#define CREATEACCOUNT 3
#define SHOWMYINFO 4
#define DELETEMYACCOUNT 5
#define MODIFYACCOUNT 6
#define NEWARTICAL 7
#define SHOWUSERARTICAL 8
#define NEWMESSAGE 9
#define DELETEUSERARTICAL 10
#define DELETEUSERMESSAGE 11
#define MODIFYARTICAL 12
#define MODIFYMESSAGE 13
#define SHOWFRIEND 14
#define REMOVEFRIEND 15
#define SHOWINVITE 16
#define REMOVEINVITE 17
#define ACCEPTINVITE 18
#define INVITE 19
#define SEARCH 20
#define SHOWFRIENDARTICAL 21
#define LIKE 22
#define UNLIKE 23
#define SHOWLOGIN 24
#define SHOWLOGOUT 25
#define UPDATELST 26

using namespace std;
typedef struct PACKET{
    int index;
    int number;
    int count;
    char buf[5][20];
    char artical[1000];
    //vector<string> data;
    /*
    char buf_0[50];
    char buf_1[50];
    char buf_2[50];
    char buf_3[50];
    */
}Packet;
Packet* NewPacket(int index);
Packet* PacketPush(Packet* packet, string data);
Packet* PacketPushArtical(Packet* packet, string data);

int CommandChoose(string command);
int SafeRecvfrom(int sockfd, char *data, int len, struct sockaddr *from, socklen_t *fromlen);
int SafeSendto(int sockfd, char *data, int len, const struct sockaddr *to, socklen_t tolen);
#endif
