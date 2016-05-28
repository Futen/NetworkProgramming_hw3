#include "DataBaseHeader.h"

void UserClass::FriendInit(){
    int state = 0;
    string line;
    string user_account;
    string friend_account;
    ifstream file;
    file.open("FRIEND_LST.txt", ios::in);
    while(getline(file, line)){
        if(line == "Account:"){
            getline(file, line);
            user_account = line;
        }
        else if(line == "Friend:"){
            getline(file, line);
            if(line != ""){
                friend_account = line;
                this->AddFriend(user_account, friend_account);
            }
        }
        else if(line == "Invite:"){
            getline(file, line);
            if(line != ""){
                this->AddInvite(line, user_account);
            }
        }
    }
    file.close();
}
void UserClass::SaveFriendLst(){
    User* user_tmp;
    ofstream file;
    file.open("FRIEND_LST.txt", ios::out);
    for(user_tmp = this->first_user; user_tmp != NULL; user_tmp = user_tmp->next){
        file << "Account:\n";
        file << (user_tmp->account + "\n");
        for(int i=0; i<user_tmp->friend_lst.size(); i++){
            file << "Friend:\n";
            file << (user_tmp->friend_lst[i] + "\n");
        }
        for(int i=0; i<user_tmp->friend_invite.size(); i++){
            file << "Invite:\n";
            file << (user_tmp->friend_invite[i] + "\n");
        }
    }
    file.close();
}
User* UserClass::AddFriend(User *user, string friend_name){
    User* tmp = this->FindUser(string("account"), friend_name);
    if(tmp != NULL && user != NULL){
        int state = 0;
        for(int i=0; i<user->friend_lst.size(); i++){
            if(tmp->account == user->friend_lst[i])
                state = 1;
        }
        if(state == 0)  user->friend_lst.push_back(friend_name);
        return user;
    }
    return NULL;
}
User* UserClass::AddFriend(string user_account, string friend_name){
    User* user = this->FindUser(string("account"), user_account);
    if(user != NULL){
        User* tmp = this->FindUser(string("account"), friend_name);
        if(tmp != NULL){
            int state = 0;
            for(int i=0; i<user->friend_lst.size(); i++){
                if(tmp->account == user->friend_lst[i])
                    state = 1;
            }
            if(state == 0)  user->friend_lst.push_back(friend_name);
            return user;
        }
        return NULL;
    }
    else 
        return NULL;
}
User* UserClass::AddInvite(string from, string to){
    User* user = this->FindUser(string("account"), from);
    User* fri = this->FindUser(string("account"), to);
    if(user != NULL && fri != NULL){
        fri->friend_invite.push_back(user->account);
        return user;
    }
    else
        return NULL;
}
bool UserClass::InviteAggree(string user_account, string friend_account){
    User *tmp1, *tmp2;
    tmp1 = AddFriend(user_account, friend_account);
    tmp2 = AddFriend(friend_account, user_account);
    if(tmp1 != NULL && tmp2 != NULL)
        return true;
    else
        return false;
}
bool UserClass::RemoveFriend(string user_account, string friend_name){
    User* user = this->FindUser(string("account"), user_account);
    User* fri = this->FindUser(string("account"), friend_name);
    if(user != NULL && fri != NULL){
        vector<string>::iterator i;
        for(i = user->friend_lst.begin(); i != user->friend_lst.end(); i++){
            if(*i == friend_name){
                user->friend_lst.erase(i);
                return true;
            }
        }
        return false;
    }
    return false;
}
bool UserClass::RemoveInvite(string user_account, string who){
    User* user = this->FindUser(string("account"), user_account);
    User* from = this->FindUser(string("account"), who);
    if(user != NULL && from != NULL){
        vector<string>::iterator i;
        for(i = user->friend_invite.begin(); i != user->friend_invite.end(); i++){
            if(*i == who){
                user->friend_invite.erase(i);
                return true;
            }
        }
        return false;
    }
    return false;
}
string UserClass::ShowUserFriend(string account){
    User* user = this->FindUser(string("account"), account);
    if(user != NULL){
        string tmp;
        for(int i = 0; i < user->friend_lst.size(); i++){
            tmp += user->friend_lst[i] + "\n";
        }
        return tmp;
    }
    else
        return string("");
}
string UserClass::ShowUserFriend(string IP, int port){
    User* user = this->FindUserFromIPAndPort(IP, port);
    return this->ShowUserFriend(user->account);
}
string UserClass::ShowUserInvite(string account){
    User* user = this->FindUser(string("account"), account);
    if(user != NULL){
        string tmp;
        for(int i = 0; i < user->friend_invite.size(); i++){
            tmp += user->friend_invite[i] + "\n";
        }
        return tmp;
    }
    else
        return string("");
}
string UserClass::ShowUserInvite(string IP, int port){
    User* user = this->FindUserFromIPAndPort(IP, port);
    return this->ShowUserInvite(user->account);
}
string UserClass::SearchUser(string info){
    User* tmp;
    for(tmp = this->first_user; tmp != NULL; tmp = tmp->next){
        if(tmp->account == info || tmp->nickname == info){
            return string("Account: ") + tmp->account + "\n" + string("Nickname: ") + tmp->nickname;
        }
    }
    return string("");
}
string UserClass::ShowFriendArtical(string IP, int port){
    string output = "";
    char line[MAXLINE];
    char buf[100];
    vector<Artical*> friend_artical;
    User* user = this->FindUserFromIPAndPort(IP, port);
    if(user != NULL){
        for(int i=0; i < user->friend_lst.size(); i++){
            User* fri = this->FindUser(string("account"), user->friend_lst[i]);
            if(fri != NULL){
                for(Artical* tmp = fri->first_artical; tmp != NULL; tmp = tmp->next)
                    friend_artical.push_back(tmp);
            }
        }
    }
    for(int i=0; i < friend_artical.size(); i++){
        for(int j=i+1; j < friend_artical.size(); j++){
            if(friend_artical[i]->time_tar > friend_artical[j]->time_tar){
                Artical* tmp = friend_artical[i];
                friend_artical[i] = friend_artical[j];
                friend_artical[j] = tmp;
            }
        }
    }
    for(int i=0; i < friend_artical.size(); i++){
        Artical* artical_tmp = friend_artical[i];
        User* usr = this->FindUser(string("account"), artical_tmp->author);
        sprintf(buf, "%s", ctime(&artical_tmp->time_tar));
        sprintf(line, "Artical ID: %d\tAuthor: %s(%s)\tTime: %s\tLikes: %d\tIP: %s\tport: %d\n", artical_tmp->index, artical_tmp->author.c_str(), usr->nickname.c_str(), buf, (int)artical_tmp->like_lst.size(), artical_tmp->IP.c_str(), artical_tmp->port);
        output += string(line);
        sprintf(line, "\tBegin\n\t\t%s\n\tEnd\n", artical_tmp->artical.c_str());
        output += string(line);
        for(Message* message_tmp = artical_tmp->first_message; message_tmp != NULL; message_tmp = message_tmp->next){
            sprintf(buf, "%s", ctime(&message_tmp->time_tar));
            sprintf(line, "\tMessage ID: %d\twho: %s\tTime: %s\n", message_tmp->index, message_tmp->who.c_str(), buf);
            output += string(line);
            sprintf(line, "\t\tBegin\n\t\t\t%s\n\t\tEnd\n", message_tmp->message.c_str());
            output += string(line);
        }
    }
    return output;
}
string UserClass::ShowLoginFriend(string IP, int port){
    string output = "";
    User* user = this->FindUserFromIPAndPort(IP, port);
    for(int i=0; i < user->friend_lst.size(); i++){
        User* Friend = this->FindUser(string("account"), user->friend_lst[i]);
        if(Friend != NULL){
            if(Friend->islogin){
                output += string("Online: ") + Friend->account + string("\n");
            }
        }
    }
    return output;
}
string UserClass::ShowLogoutFriend(string IP, int port){
    time_t ticks = time(NULL);
    string output = "";
    User* user = this->FindUserFromIPAndPort(IP, port);
    for(int i=0; i < user->friend_lst.size(); i++){
        User* Friend = this->FindUser(string("account"), user->friend_lst[i]);
        if(Friend != NULL){
            if(!Friend->islogin){
                char buf[MAXLINE];
                sprintf(buf, "Logout %f minutes: %s\n", (ticks - Friend->last_time_t)/60.0, Friend->account.c_str());
                output += buf;
            }
        }
    }
    return output;

}
