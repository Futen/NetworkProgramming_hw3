#include "DataBaseHeader.h"

typedef struct INFO{
    char IP[50];
    int port;
    int connfd;
}Info;

UserClass userObject;

void WaitForData(int sockfd, struct sockaddr *servaddr);
void CommandProcess(int command, Packet *packet_in, int sockfd, string IP, int port);
//void CommandProcess(int command, char *line, int sockfd, struct sockaddr *pcliaddr, socklen_t clilen, string IP, int port);
void* Process(void* arg);
int main(int argc, char* argv[]){
    int sockfd;
    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(sockfd, LISTENQ);
    WaitForData(sockfd, (struct sockaddr*)&server_addr);
    
    return 0;
}
void WaitForData(int sockfd, struct sockaddr *servaddr){
    int connfd;
    int command;
    int port;
    string IP;
    struct sockaddr_in client_addr;
    socklen_t clilen;
    pthread_t tid;
    Info *info;

    userObject.Init();

    while(true){
        info = (Info*)malloc(sizeof(Info));
        clilen = sizeof(client_addr);
        connfd = accept(sockfd, (struct sockaddr*)&client_addr, &clilen);
        IP = inet_ntoa(((struct sockaddr_in*)&client_addr)->sin_addr);
        port = ((struct sockaddr_in*)&client_addr)->sin_port;
        //printf("%s %d connect\n", IP.c_str(), port);
        strcpy(info->IP, IP.c_str());
        info->port = port;
        info->connfd = connfd;
        pthread_create(&tid, NULL, &Process, info);
    }
}
void* Process(void* arg){
    int connfd;
    string IP;
    int port;
    int nbytes;
    char recvline[MAXLINE];
    Packet *packet;
    Info info;
    info = *((Info*)arg);
    free(arg);

    connfd = info.connfd;
    IP = info.IP;
    port = info.port;
    pthread_detach(pthread_self());
    printf("%s %d connect\n", IP.c_str(), port);
    while((nbytes = read(connfd, recvline, sizeof(Packet))) != 0){
        int command;
        packet = (Packet*)recvline;
        command = CommandChoose(string(packet->buf[0]));
        CommandProcess(command, packet, connfd, IP, port);
    }
    close(connfd);
    return NULL;
}
void CommandProcess(int command, Packet *packet_in, int sockfd, string IP, int port){
    int nbytes;
    int number;
    char recvline[MAXLINE];
    string recvData;
    string sendData;
    string account;
    string password;
    string nickname;
    string birthday;
    string success = "success";
    string fail = "fail";
    string show = "show";
    string transfer = "transfer";
    vector<string> file_lst;
    User* check;
    Packet *packet;
    File *file;
    switch(command){
        case LOGIN:
            account = packet_in->buf[1];
            password = packet_in->buf[2];
            if((check = userObject.UserLogin(account, password, IP, port)) != NULL){
                check->service_port = packet_in->number;
                check->massage_port = packet_in->number2;
                userObject.SaveUserList();
                packet = NewPacket(0);
                PacketPush(packet, success);
                PacketPush(packet, check->nickname);
                PacketPush(packet, check->birthday);
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
                read(sockfd, recvline, sizeof(Packet));
                packet = (Packet*)recvline;
                int number = packet->number;
                //cout << number << endl;
                for(int i=0; i<number; i++){
                    nbytes = read(sockfd, recvline, sizeof(Packet));
                    packet = (Packet*)recvline;
                    file_lst.push_back(string(packet->buf[0]));
                    //cout << packet->buf[0] << endl;
                }
                cout << "Account Login:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "\tfile: ";
                for(int i=0; i<file_lst.size(); i++){
                    cout << file_lst[i] << " ";
                }
                cout << endl;
                cout << "}" << endl;
            }
            else{
                packet = NewPacket(0);
                PacketPush(packet, fail);
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
            }
            break;
        case CREATEACCOUNT:
            time_t ticks;
            char buf[MAXLINE];
            ticks = time(NULL);
            sprintf(buf, "%d", (int)ticks);
            //snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
            account = packet_in->buf[1];
            password = packet_in->buf[2];
            nickname = packet_in->buf[3];
            birthday = packet_in->buf[4];
            //cout << account << endl;
            //cout << password << endl;
            userObject.CreateUser(account, password, nickname, birthday, string(buf), string(buf));
            check = userObject.UserLogin(account, password, IP, port);
            if(check != NULL){
                check->service_port = packet_in->number;
                check->massage_port = packet_in->number2;
                packet = NewPacket(0);
                PacketPush(packet, success);
                PacketPush(packet, check->nickname);
                PacketPush(packet, check->birthday);
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
                userObject.SaveUserList();
                read(sockfd, recvline, sizeof(Packet));
                packet = (Packet*)recvline;
                int number = packet->number;
                for(int i=0; i<number; i++){
                    nbytes = read(sockfd, recvline, sizeof(Packet));
                    packet = (Packet*)recvline;
                    file_lst.push_back(string(packet->buf[0]));
                }
                cout << "Account Login:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "\tfile: ";
                for(int i=0; i<file_lst.size(); i++){
                    cout << file_lst[i] << " ";
                }
                cout << endl;
                cout << "}" << endl;
            }
            else{
                packet = NewPacket(0);
                PacketPush(packet, fail);
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
            }
            break;
        case LOGOUT:
            if((check=userObject.UserLogout(IP, port)) != NULL){
                userObject.SaveUserList();
                cout << "Account Logout:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "}" << endl;
                userObject.RemoveOwner(check->account);
            }
            break;
        case DELETEMYACCOUNT:
            packet = NewPacket(0);
            if(userObject.DeleteUser(IP, port)){
                userObject.SaveArtical();
                PacketPush(packet, show);
                PacketPushArtical(packet, success);
                write(sockfd, (char*)packet, sizeof(Packet));
                userObject.SaveUserList();
            }
            else{
                PacketPush(packet, show);
                PacketPushArtical(packet, fail);
                write(sockfd, (char*)packet, sizeof(Packet));
            }
            delete packet;
            break;
        case UPDATELST:
            number = packet_in->number;
            //cout << "fffff" << endl;
            if((check = userObject.FindUserFromIPAndPort(IP, port)) != NULL){
                for(int i=0; i<number; i++){
                    nbytes = read(sockfd, recvline, sizeof(Packet));
                    packet = (Packet*)recvline;
                    userObject.AddFile(string(packet->buf[0]), check->account, packet->number);
                    //cout<<packet->buf[0] << endl;
                }
                userObject.PrintFileLst();
            }
            break;
        case SHOWFILELST:
            packet = NewPacket(0);
            PacketPush(packet, show);
            sendData = userObject.GetFileLst();
            //cout << sendData;
            PacketPushArtical(packet, sendData);
            write(sockfd, (char*)packet, sizeof(Packet));
            delete packet;
            break;
        case REQUESTFILE:
            file = userObject.FindFile(string(packet_in->buf[1]));
            if(file != NULL){
                packet = NewPacket(0);
                PacketPush(packet, transfer);
                PacketPush(packet, file->f_name);
                packet->number = file->owner_lst.size();
                packet->number2 = file->file_size;
                write(sockfd, (char*)packet, sizeof(Packet));
                delete packet;
                for(int i=0; i<file->owner_lst.size(); i++){
                    packet = NewPacket(0);
                    PacketPush(packet, string(file->owner_lst[i]));
                    check = userObject.FindUser(string("account"), file->owner_lst[i]);
                    PacketPush(packet, check->IP);
                    packet->number = check->service_port;
                    write(sockfd, (char*)packet, sizeof(Packet));
                    delete packet;
                }
            }
        default:
            cout << "GG command" << endl;
    }
}
/*
void CommandProcess(int command, char* line, int sockfd, struct sockaddr *pcliaddr, socklen_t clilen, string IP, int port){
    int nbytes;
    int artical_index;
    int message_index;
    char recvline[MAXLINE];
    char success[] = "success";
    char fail[] = "fail";
    User* check;
    User* who;
    Artical* artical;
    string sendData;
    string recvData;
    string account;
    string password;
    string nickname;
    string birthday;
    Packet* packet = (Packet*)line;
    Packet* packet_tmp;

    switch(command){
        case TEST:
            nbytes = recvfrom(sockfd, recvline, MAXLINE, 0,pcliaddr, &clilen);
            packet = (Packet*)recvline;
            cout << packet->count << endl;
            cout << packet->buf[0] << endl;
            cout << packet->buf[1] << endl;
            cout << packet->buf[2] << endl;
            cout << packet->buf[3] << endl;
            break;
        case LOGIN:
            account = packet->buf[1];
            password = packet->buf[2];
            //cout << account << endl;
            //cout << password << endl;
            check = userObject.UserLogin(account, password, IP, port);
            if(check != NULL){
                userObject.SaveUserList();
                packet_tmp = NewPacket(0);
                PacketPush(packet_tmp, string(success));
                PacketPush(packet_tmp, check->nickname);
                PacketPush(packet_tmp, check->birthday);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                delete packet_tmp;
                cout << "Account Login:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "}" << endl;
            }
            else{
                packet_tmp = NewPacket(0);
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                delete packet_tmp;
            }
            break;
        case CREATEACCOUNT:
            time_t ticks;
            char buf[MAXLINE];
            ticks = time(NULL);
            sprintf(buf, "%d", (int)ticks);
            //snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
            account = packet->buf[1];
            password = packet->buf[2];
            nickname = packet->buf[3];
            birthday = packet->buf[4];
            //cout << account << endl;
            //cout << password << endl;
            userObject.CreateUser(account, password, nickname, birthday, string(buf), string(buf));
            check = userObject.UserLogin(account, password, IP, port);
            if(check != NULL){
                packet = NewPacket(0);
                PacketPush(packet, string(success));
                PacketPush(packet, check->nickname);
                PacketPush(packet, check->birthday);
                sendto(sockfd, (char*)packet, sizeof(Packet), 0, pcliaddr, clilen);
                userObject.SaveUserList();
                cout << "Account Login:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "}" << endl;
            }
            else{
                packet = NewPacket(0);
                PacketPush(packet, string(fail));
                sendto(sockfd, fail, strlen(fail)+1, 0, pcliaddr, clilen);
            }
            delete packet;
            break;
        case DELETEMYACCOUNT:
            packet_tmp = NewPacket(0);
            if(userObject.DeleteUser(IP, port)){
                userObject.SaveArtical();
                PacketPush(packet_tmp, success);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                userObject.SaveUserList();
            }
            else{
                PacketPush(packet_tmp, fail);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case LOGOUT:
            if((check = userObject.UserLogout(IP, port)) != NULL){
                userObject.SaveUserList();
                packet_tmp = NewPacket(0);
                PacketPush(packet_tmp, success);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                cout << "Account Logout:{" << endl;
                cout << "\taccount: " << check->account << endl;
                cout << "\tpassword: " << check->password << endl;
                cout << "\tnickname: " << check->nickname << endl;
                cout << "\tbirthday: " << check->birthday << endl;
                cout << "\tIP: " << check->IP << endl;
                cout << "\tport: " << check->port << endl;
                cout << "}" << endl;
            }
            else{
                packet_tmp = NewPacket(0);
                PacketPush(packet_tmp, fail);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case MODIFYACCOUNT:
            password = string(packet->buf[1]);
            nickname = string(packet->buf[2]);
            birthday = string(packet->buf[3]);
            packet_tmp = NewPacket(0);
            if((who = userObject.UserModify(IP, port, password, nickname, birthday)) != NULL){
                userObject.SaveUserList();
                PacketPush(packet_tmp , success);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                cout << "Account Modify:{" << endl;
                cout << "\taccount: " << who->account << endl;
                cout << "\tpassword: " << who->password << endl;
                cout << "\tnickname: " << who->nickname << endl;
                cout << "\tbirthday: " << who->birthday << endl;
                cout << "\tIP: " << who->IP << endl;
                cout << "\tport: " << who->port << endl;
                cout << "}" << endl;
            }
            else{
                PacketPush(packet_tmp , fail);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case NEWARTICAL:
            recvData = string(packet->artical);
            packet_tmp = NewPacket(0);
            if((artical = userObject.NewUserArtical(IP, port, recvData)) != NULL){
                userObject.SaveArtical();
                PacketPush(packet_tmp, success);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
                cout << artical->author  << " Create New Artical:{" << endl << "\t";
                cout << recvData << endl << "}" << endl;
            }
            else{
                PacketPush(packet_tmp, fail);
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case SHOWUSERARTICAL:
            packet_tmp = NewPacket(0);
            recvData = userObject.ShowUserArtical(IP, port);
            PacketPush(packet_tmp, success);
            PacketPushArtical(packet_tmp, recvData);
            sendto(sockfd, (char*) packet_tmp, sizeof(Packet), 0,pcliaddr, clilen);
            delete packet_tmp;
            break;
        case NEWMESSAGE:
            account = packet->buf[1];
            artical_index = atoi(packet->buf[2]);
            recvData = packet->artical;
            packet_tmp = NewPacket(0);
            if( userObject.NewUserMessage(IP, port, account, artical_index, recvData) != NULL ){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case DELETEUSERARTICAL:
            recvData = packet->buf[1];
            packet_tmp = NewPacket(0);
            if(userObject.DeleteUserArtical(IP, port, atoi(recvData.c_str()))){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case DELETEUSERMESSAGE:
            account = packet->buf[1];
            artical_index = atoi(packet->buf[2]);
            message_index = atoi(packet->buf[3]);
            packet_tmp = NewPacket(0);
            if(userObject.DeleteUserMessage(IP, port, account, artical_index, message_index)){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case MODIFYARTICAL:
            artical_index = atoi(packet->buf[1]);
            recvData = packet->artical;
            packet_tmp = NewPacket(0);
            if(userObject.ModifyArtical(IP, port, artical_index, recvData) != NULL){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case MODIFYMESSAGE:
            account = packet->buf[1];
            artical_index = atoi(packet->buf[2]);
            message_index = atoi(packet->buf[3]);
            recvData = packet->artical;
            packet_tmp = NewPacket(0);
            if(userObject.ModifyMessage(IP, port, account, artical_index, message_index, recvData) != NULL){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case SHOWFRIEND:
            sendData = userObject.ShowUserFriend(IP, port);
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, sendData);
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
        case REMOVEFRIEND:
            check = userObject.FindUserFromIPAndPort(IP, port);
            account = packet->buf[1];
            packet_tmp = NewPacket(0);
            if(userObject.RemoveFriend(check->account, account)){
                userObject.RemoveFriend(account, check->account);
                userObject.SaveFriendLst();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case SHOWINVITE:
            sendData = userObject.ShowUserInvite(IP, port);
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, sendData);
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
        case REMOVEINVITE:
            check = userObject.FindUserFromIPAndPort(IP, port);
            account = packet->buf[1];
            packet_tmp = NewPacket(0);
            if(userObject.RemoveInvite(check->account, account)){
                userObject.SaveFriendLst();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case ACCEPTINVITE:
            check = userObject.FindUserFromIPAndPort(IP, port);
            account = packet->buf[1];
            packet_tmp = NewPacket(0);
            if(userObject.InviteAggree(check->account, account)){
                userObject.RemoveInvite(check->account, account);
                userObject.SaveFriendLst();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case INVITE:
            check = userObject.FindUserFromIPAndPort(IP, port);
            account = packet->buf[1];
            packet_tmp = NewPacket(0);
            if((who = userObject.AddInvite(check->account, account)) != NULL){
                userObject.SaveFriendLst();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case SEARCH:
            account = packet->buf[1];
            sendData = userObject.SearchUser(account);
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, sendData);
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
        case SHOWFRIENDARTICAL:
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, userObject.ShowFriendArtical(IP, port));
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
        case LIKE:
            account = packet->buf[1];
            artical_index = atoi(packet->buf[2]);
            packet_tmp = NewPacket(0);
            if(userObject.GiveLike(IP, port, account, artical_index)){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case UNLIKE:
            account = packet->buf[1];
            artical_index = atoi(packet->buf[2]);
            packet_tmp = NewPacket(0);
            if(userObject.UnLike(IP, port, account, artical_index)){
                userObject.SaveArtical();
                PacketPush(packet_tmp, string(success));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            else{
                PacketPush(packet_tmp, string(fail));
                sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            }
            delete packet_tmp;
            break;
        case SHOWLOGIN:
            sendData = userObject.ShowLoginFriend(IP, port);
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, sendData);
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
        case SHOWLOGOUT:
            sendData = userObject.ShowLogoutFriend(IP, port);
            packet_tmp = NewPacket(0);
            PacketPushArtical(packet_tmp, sendData);
            sendto(sockfd, (char*)packet_tmp, sizeof(Packet), 0, pcliaddr, clilen);
            delete packet_tmp;
            break;
    }
}
*/







