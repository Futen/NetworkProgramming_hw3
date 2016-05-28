#include "ClientHeader.h"

string user_account;
string user_password;
string user_nickname;
string user_birthday;

void ConnectProcess(int sockfd, struct sockaddr *server_addr, socklen_t servlen){
    int nbytes;
    int shutdown;
    int nready;
    int maxfd;
    int islogin;
    int fileno_stdin;
    int command;
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    string sendData;
    string recvData;
    struct timeval timeout;
    fd_set allset;
    
    islogin = 0;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fileno_stdin = fileno(stdin);
    FD_ZERO(&allset);
    FD_SET(sockfd, &allset);
    FD_SET(fileno_stdin, &allset);
    maxfd = sockfd > fileno_stdin?sockfd:fileno_stdin;
    
    shutdown = 0;
    while(!islogin){
        islogin = Login(sockfd, server_addr, servlen);
    }
    cout << "Welcome !!!!!" << endl;
    ShowCommand();
    while(shutdown == 0){
        nready = select(maxfd+1, &allset, NULL, NULL, &timeout);
        FD_SET(sockfd, &allset);
        FD_SET(fileno_stdin, &allset);
        if(nready > 0){
            if(FD_ISSET(fileno_stdin, &allset)){
                fgets(sendline, MAXLINE, stdin);
                sendline[strlen(sendline)-1] = '\0';
                sendData = GetCommandString(string(sendline));
                /*
                //cout << senddata << endl;
                sendto(sockfd, sendline, strlen(sendline)+1, 0, server_addr, servlen);
                //recvfrom(sockfd, recvline, MAXLINE, 0, server_addr, &servlen);
                //recvdata = recvline;
                //cout << recvdata << endl;
                */
                command = CommandChoose(sendData);
                ProcessCommand(command, sendData, sockfd, server_addr, servlen);
                ShowCommand();
            }
        }
    }
}
int Login(int sockfd, struct sockaddr *server_addr, socklen_t servlen){
    char recvline[MAXLINE];
    string login_mode = "LOGIN";
    string create_mode = "CREATEACCOUNT";
    string recvData;
    string command;
    Packet *packet = NewPacket(0);
    cout << "$ Create?" << endl;
    cout << "$ Login?" << endl;
    cin >> command;
    if(command != "Login" && command != "Create")
        return 0;
    cout << "Account: ";
    cin >> user_account;
    cout << "Password: ";
    cin >> user_password;
    if(command == "Login")
        PacketPush(packet, login_mode);
    else if(command == "Create"){
        cout << "Nickname: ";
        cin >> user_nickname;
        cout << "Birthday(yy/mm/dd): ";
        cin >> user_birthday;
        PacketPush(packet, create_mode);
    }
    else
        return 0;
    PacketPush(packet, user_account);
    PacketPush(packet, user_password);
    if(command == "Create"){
        PacketPush(packet, user_nickname);
        PacketPush(packet, user_birthday);
    }
    sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
    delete packet;
    recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
    packet = (Packet*)recvline;
    if(string("success") == packet->buf[0]){
        user_nickname = packet->buf[1];
        user_birthday = packet->buf[2];
        cout << "Login Success " << user_nickname  << "!!" << endl;
        return 1;
    }
    else{
        cout << "Error account or password" << endl;
        return 0;
    }
}
void ShowCommand(){
    cout << "***************" << endl;
    cout << "$ ShowMyInfo" << endl;
    cout << "$ ShowMyArtical" << endl;
    cout << "$ ShowFriendArtical" << endl;
    cout << "$ GiveLike" << endl;
    cout << "$ UnLike" << endl;
    cout << "$ NewArtical" << endl;
    cout << "$ ModifyMyArtical" << endl;
    cout << "$ DeleteMyArtical" << endl;
    cout << "$ NewMessage" << endl;
    cout << "$ ModifyMyMessage" << endl;
    cout << "$ DeleteMyMessage" << endl;
    cout << "$ DeleteMyAccount" << endl;
    cout << "$ ModifyMyAccount" << endl;
    cout << "$ ShowMyFriend" << endl;
    cout << "$ ShowLoginFriend" << endl;
    cout << "$ ShowLogoutFriend" << endl;
    cout << "$ RemoveMyFriend" << endl;
    cout << "$ ShowMyInvite" << endl;
    cout << "$ InviteWho" << endl;
    cout << "$ AcceptMyInvite" << endl;
    cout << "$ RemoveMyInvite" << endl;
    cout << "$ SearchWho" << endl;
    cout << "$ Logout" << endl;
    cout << "***************" << endl;
}
string GetCommandString(string input){
    if(input == "ShowMyInfo")   return string("SHOWMYINFO");
    else if(input == "TEST") return string("TEST");
    else if(input == "DeleteMyAccount") return string("DELETEMYACCOUNT");
    else if(input == "Logout")  return string("LOGOUT");
    else if(input == "ModifyMyAccount") return string("MODIFYACCOUNT");
    else if(input == "NewArtical") return string("NEWARTICAL");
    else if(input == "ShowMyArtical") return string("SHOWUSERARTICAL");
    else if(input == "NewMessage") return string("NEWMESSAGE");
    else if(input == "DeleteMyArtical") return string("DELETEUSERARTICAL");
    else if(input == "DeleteMyMessage") return string("DELETEUSERMESSAGE");
    else if(input == "ModifyMyArtical") return string("MODIFYARTICAL");
    else if(input == "ModifyMyMessage") return string("MODIFYMESSAGE");
    else if(input == "ShowMyFriend") return string("SHOWFRIEND");
    else if(input == "RemoveMyFriend") return string("REMOVEFRIEND");
    else if(input == "ShowMyInvite") return string("SHOWINVITE");
    else if(input == "RemoveMyInvite") return string("REMOVEINVITE");
    else if(input == "AcceptMyInvite") return string("ACCEPTINVITE");
    else if(input == "InviteWho") return string("INVITE");
    else if(input == "SearchWho") return string("SEARCH");
    else if(input == "ShowFriendArtical") return string("SHOWFRIENDARTICAL");
    else if(input == "GiveLike") return string("LIKE");
    else if(input == "UnLike") return string("UNLIKE");
    else if(input == "ShowLoginFriend") return string("SHOWLOGIN");
    else if(input == "ShowLogoutFriend") return string("SHOWLOGOUT");
    else    return string("UNKNOWN");
}
void ProcessCommand(int command, string sendData, int sockfd, struct sockaddr *server_addr, socklen_t servlen){
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    string buf;
    string recvData;
    string success = "success";
    string fail = "fail";
    Packet* packet;

    switch(command){
        case TEST:
            sendto(sockfd, (char*)sendData.c_str(), strlen(sendData.c_str())+1, 0,server_addr, servlen);
            packet = NewPacket(0);
            PacketPush(packet, user_account);
            PacketPush(packet, user_password);
            PacketPush(packet, user_nickname);
            PacketPush(packet, user_birthday);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            break;
        case SHOWMYINFO:
            cout << "My Account:{" << endl;
            cout << "\taccount: " << user_account << endl;
            cout << "\tpassword: " << user_password << endl;
            cout << "\tnickname: " << user_nickname << endl;
            cout << "\tbirthday: " << user_birthday << endl;
            cout << "}" << endl;
            break;
        case DELETEMYACCOUNT:
            //sendto(sockfd, sendData.c_str(), strlen(sendData.c_str())+1, 0, server_addr, servlen);
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success){
                cout << "Account Delete Success!!" << endl;
            }
            else{
                cout << "Account Delete Fail!!" << endl;
            }
            exit(0);
            break;
        case LOGOUT:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Account Logout Success!!" << endl;
            else    cout << "Account Logout Fail!!" << endl;
            exit(0);
            break;
        case MODIFYACCOUNT:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "New Password: ";
            cin >> user_password;
            cout << "New Nickname: ";
            cin >> user_nickname;
            cout << "New Birthday: ";
            cin >> user_birthday;
            PacketPush(packet, user_password);
            PacketPush(packet, user_nickname);
            PacketPush(packet, user_birthday);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Account Modify Success!!" << endl;
            else    cout << "Account Modify Fail!!" << endl;
            break;
        case NEWARTICAL:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "New Artical:" << endl;
            fgets(sendline, MAXLINE, stdin);
            sendline[strlen(sendline)-1] = '\0';
            PacketPushArtical(packet, string(sendline));
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Create Artical Success!!" << endl;
            else    cout << "Create Artical Fail!!" << endl;
            break;
        case SHOWUSERARTICAL:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success){
                cout << packet->artical;
            }
            else
                cout << "Show My Artical Fail!!" << endl;
            break;
        case NEWMESSAGE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Artical Author: ";
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The Artical Index: ";
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The Message: " << endl;
            getline(cin, buf);
            PacketPushArtical(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0,server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Create Message Success!!" << endl;
            else    cout << "Create Message Fail!!" << endl;
            break;
        case DELETEUSERARTICAL:
            //sendto(sockfd, sendData.c_str(), strlen(sendData.c_str())+1, 0, server_addr, servlen);
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Artical Index: ";
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Delete Artical Success" << endl;
            else cout << "Delete Artical Fail" << endl;
            break;
        case DELETEUSERMESSAGE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Artical Author:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            cout << "The Artical Index:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            cout << "The Message Index:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Delete Artical Success" << endl;
            else cout << "Delete Artical Fail" << endl;
            break;
        case MODIFYARTICAL:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Artical Index:" << endl;
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The New Artical:" << endl;
            getline(cin, buf);
            PacketPushArtical(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Modify Artical Success" << endl;
            else cout << "Modify Artical Fail" << endl;
            break;
        case MODIFYMESSAGE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Artical Author:" << endl;
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The Artical Index:" << endl;
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The Message Index:" << endl;
            getline(cin, buf);
            PacketPush(packet, buf);
            cout << "The New Message:" << endl;
            getline(cin, buf);
            PacketPushArtical(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success) cout << "Modify Message Success" << endl;
            else cout << "Modify Message Fail" << endl;
            break;
        case SHOWFRIEND:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            cout << packet->artical;
            break;
        case REMOVEFRIEND:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "Remove Friend Success!" << endl;
            else
                cout << "Remove Friend Fail!" << endl;
            break;
        case SHOWINVITE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            cout << packet->artical;
            break;
        case REMOVEINVITE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "Remove Invite Success!" << endl;
            else
                cout << "Remove Invite Fail!" << endl;
            break;
        case ACCEPTINVITE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "Accept Invite Success!" << endl;
            else
                cout << "Accept Invite Fail!" << endl;
            break;
         case INVITE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "Invite Success!" << endl;
            else
                cout << "Invite Fail!" << endl;
            break;
        case SEARCH:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Info" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->artical) != "")   cout << packet->artical << endl;
            else cout << "Search Fail!!!!" << endl;
            break;
        case SHOWFRIENDARTICAL:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            cout << packet->artical;
            break;
        case LIKE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            cout << "The Index:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "Give Like Success!" << endl;
            else
                cout << "Give Like Fail!" << endl;
            break;
        case UNLIKE:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            cout << "The Account:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            cout << "The Index:" << endl;
            cin >> buf;
            PacketPush(packet, buf);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            if(string(packet->buf[0]) == success)
                cout << "UnLike Success!" << endl;
            else
                cout << "UnLike Fail!" << endl;
            break;
        case SHOWLOGIN:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            cout << packet->artical;
            break;
        case SHOWLOGOUT:
            packet = NewPacket(0);
            PacketPush(packet, sendData);
            sendto(sockfd, (char*)packet, sizeof(Packet), 0, server_addr, servlen);
            delete packet;
            recvfrom(sockfd, recvline, MAXLINE, 0,server_addr, &servlen);
            packet = (Packet*)recvline;
            cout << packet->artical;
            break;
       
        default:
            cout << "Unknown command" << endl;
    }
}









