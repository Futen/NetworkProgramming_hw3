#ifndef DataBase
#define DataBase
#include "Header.h"
struct Message{
    int index;
    string who;
    string message;
    string time;
    time_t time_tar;
    Message* previous;
    Message* next;
};
struct Artical{
    int index;
    int message_count;
    string author;
    string artical;
    string time;
    string IP;
    time_t time_tar;
    int port;
    vector<string> like_lst;
    Message* first_message;
    Message* last_message;
    Artical* next;
    Artical* previous;
};
struct User{
    int index;
    int artical_index;
    string account;
    string nickname;
    string password;
    string birthday;
    string IP;
    int port;
    time_t reg_time_t;
    time_t last_time_t;
    string reg_time;
    string last_time;
    int islogin;
    vector<string> friend_lst;
    vector<string> friend_invite;
    User* next;
    User* previous;
    Artical* first_artical;
    Artical* last_artical;
};
class UserClass;
class ArticalClass;

class ArticalClass{
private:
    int artical_index;
    Artical* first_artical;
    Artical* last_artical;
protected:
    Artical* FindArticalFromIndex(int index);
    Message* FindMessageFromIndex(int art_index, int mess_index);
public:
    //int artical_index;
    void Init();
    void SaveArtical();
    int CreateArtical(string account, string artical);
    void ShowArtical();
    Message* CreateMessage(int parent_artical_index, string account, string message);
    Message* ModifyMessage(int art_index, int mess_index, string message);
    bool DeleteMessage(int art_index, int mess_index);
    bool DeleteArtical(int art_index);
};

class UserClass{
private:
    int user_index;
    User* first_user;
    User* last_user;
protected:
    Artical* CreateArtical(User* user, string artical);
    Artical* CreateArtical(User* user, string artical, string IP, int port, string time);
    Message* CreateMessage(Artical* artical, string who, string message);
    Message* CreateMessage(Artical* artical, string who, string message, string time);
    Artical* FindArticalFromIndex(string account, int artical_index);
    Message* FindMessageFromIndex(string account, int artical_index, int message_index);
    Artical* DeleteArticalMessage(Artical* artical);
public:
    /****************/
    void Init();
    void SaveUserList();
    User* CreateUser(string account, string password, string nickname, string birthday);

    User* FindUserFromIPAndPort(string IP, int port);
    User* CreateUser(string account, string password, string nickname, string birthday, string reg_time, string log_time);
    User* FindUser(string whatkind, string info);
    User* UserLogin(string account, string password, string IP, int port);
    User* UserLogout(string IP, int port);
    User* UserModify(string IP, int port, string password, string nickname, string birthday);
    bool DeleteUser(string IP, int port);
    /***************/
    void ArticalInit();
    void ShowArtical(User* user);
    void SaveArtical();
    Artical* NewUserArtical(string IP, int port, string artical);
    Message* NewUserMessage(string IP, int port, string author_account, int artical_index, string message);
    bool GiveLike(string IP, int port, string account, int artical_index);
    bool UnLike(string IP, int port, string account, int artical_index);
    bool DeleteUserArtical(string IP, int port, int artical_index);
    bool DeleteUserMessage(string IP, int port, string author_account, int artical_index, int message_index);
    string ShowUserArtical(string IP, int port);
    string ShowFriendArtical(string IP, int port);
    Artical* ModifyArtical(string IP, int port, int artical_index, string artical);
    Message* ModifyMessage(string IP, int port, string artical_author , int artical_index, int message_index, string message);
    /***************/
    void FriendInit();
    void SaveFriendLst();
    User* AddFriend(User* user, string friend_name);
    User* AddFriend(string user_account, string friend_name);
    User* AddInvite(string from, string to);
    string SearchUser(string info);
    string ShowUserFriend(string account);
    string ShowUserFriend(string IP, int port);
    string ShowUserInvite(string account);
    string ShowUserInvite(string IP, int port);
    string ShowLoginFriend(string IP, int port);
    string ShowLogoutFriend(string IP, int port);
    bool RemoveFriend(string user_account, string friend_name);
    bool RemoveInvite(string user_account, string who);
    bool InviteAggree(string user_account, string friend_account);
};
#endif
