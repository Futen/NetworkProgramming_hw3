#include "Header.h"

int CommandChoose(string command){
    if (command == "LOGIN")
        return LOGIN;
    else if(command == "TEST")
        return TEST;
    else if(command == "LOGOUT")
        return LOGOUT;
    else if(command == "CREATEACCOUNT")
        return CREATEACCOUNT;
    else if(command == "SHOWMYINFO")
        return SHOWMYINFO;
    else if(command == "DELETEMYACCOUNT")
        return DELETEMYACCOUNT;
    else if(command == "MODIFYACCOUNT")
        return MODIFYACCOUNT;
    else if(command == "NEWARTICAL")
        return NEWARTICAL;
    else if(command == "SHOWUSERARTICAL")
        return SHOWUSERARTICAL;
    else if(command == "NEWMESSAGE")
        return NEWMESSAGE;
    else if(command == "DELETEUSERARTICAL")
        return DELETEUSERARTICAL;
    else if(command == "DELETEUSERMESSAGE")
        return DELETEUSERMESSAGE;
    else if(command == "MODIFYARTICAL")
        return MODIFYARTICAL;
    else if(command == "MODIFYMESSAGE")
        return MODIFYMESSAGE;
    else if(command == "SHOWFRIEND")
        return SHOWFRIEND;
    else if(command == "REMOVEFRIEND")
        return REMOVEFRIEND;
    else if(command == "SHOWINVITE")
        return SHOWINVITE;
    else if(command == "REMOVEINVITE")
        return REMOVEINVITE;
    else if(command == "ACCEPTINVITE")
        return ACCEPTINVITE;
    else if(command == "INVITE")
        return INVITE;
    else if(command == "SEARCH")
        return SEARCH;
    else if(command == "SHOWFRIENDARTICAL")
        return SHOWFRIENDARTICAL;
    else if(command == "LIKE")
        return LIKE;
    else if(command == "UNLIKE")
        return UNLIKE;
    else if(command == "SHOWLOGIN")
        return SHOWLOGIN;
    else if(command == "SHOWLOGOUT")
        return SHOWLOGOUT;
    else
        return DEFAULT;
}

int SafeRecvfrom(int sockfd, char *data, int len, struct sockaddr *from, socklen_t *fromlen){
    int nbytes;
    char ack[4] = "ack";

    nbytes = recvfrom(sockfd, data, len, 0, from, fromlen);
    //cout << data << endl;
    //
    usleep(100000);
    sendto(sockfd, data, sizeof(Packet), 0, from, *fromlen);
    //sleep(1);
    return nbytes;
}
int SafeSendto(int sockfd, char *data, int len, const struct sockaddr *to, socklen_t tolen){
    int nready;
    int nbytes;
    char line[MAXLINE];
    char ack[4] = "ack";
    struct timeval timeout;
    struct sockaddr from;
    socklen_t fromlen;
    fd_set allset;
    Packet *buf1,*buf2;

    timeout.tv_sec = 1;
    //timeout.tv_usec = 50000;
    FD_ZERO(&allset);
    FD_SET(sockfd, &allset);

    while(true){
        //cout << "start\n";
        nready = select(sockfd + 1, &allset, NULL, NULL, &timeout);
        //cout << "end\n";
        if(!FD_ISSET(sockfd, &allset)){
            sendto(sockfd, data, len, 0, to, tolen);
            //cout << "HH" << endl;
        }
        else{
            nbytes = recvfrom(sockfd, line, MAXLINE, 0, &from, &fromlen);
            buf1 = (Packet*)line;
            buf2 = (Packet*)data;
            if(buf1->index == buf2->index)
                return len;
        }
        FD_SET(sockfd, &allset);
    }
}
Packet* NewPacket(int index){
    Packet* tmp = new Packet;
    tmp->count = 0;
    tmp->index = index;
    return tmp;
}
Packet* PacketPush(Packet* packet, string data){
    strcpy(packet->buf[packet->count], data.c_str());
    packet->count++;
    return packet;
}
Packet* PacketPushArtical(Packet* packet, string data){
    strcpy(packet->artical, data.c_str());
    return packet;
}
