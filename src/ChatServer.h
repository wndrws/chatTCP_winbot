//
// Created by WebSter on 28.11.2016.
//

#pragma once

#include "etcp.h"
#include <string>
#include <vector>
#include <unordered_map>

#define CODE_LOGINREQUEST 1
#define CODE_LOGOUTREQUEST 2
#define CODE_LOGINANSWER 3
#define CODE_LOGOUTANSWER 4 // Currently not in use
#define CODE_FORCEDLOGOUT 5
#define CODE_LOGINNOTIFY 6
#define CODE_LOGOUTNOTIFY 7
#define CODE_SRVERR 8
#define CODE_HEARTBEAT 9
#define CODE_SRVMSG 10

#define CODE_INMSG 128
#define CODE_OUTMSG 129

#define MAX_USERNAME_LENGTH 32
#define MAX_MSG_LENGTH 60000

using namespace std;

class Exception {};

class ChatServer {
private:
    SOCKET m_Socket;
    unordered_map<int, string> m_Users;
    unordered_map<int, vector<string>> m_Pending;
    //int m_ThisUserID;
    volatile int m_CurrentPeer = 0;

    int receiveUsersListPacket();
public:
    ChatServer() : m_Socket(0) {};
    ChatServer(SOCKET);
    void setCurrentPeer(int id);
    int getCurrentPeer() const;
    string getFullName(int id) const;
    void showMessage(int id_from);
    string getPendingList() const;

    const unordered_map<int, string>& getUsersMap() const;
    unordered_map<int, vector<string>>& getPendingMap();

    int login(string username);
    int logout();
    int receiveUsersList();
    void showUsersList();
    string startChat(string peername);
    int sendMessage(string msg);
    bool receiveMessage();
    string receiveServerMessage();
    bool receiveLoginNotification();
    bool receiveLogoutNotification();
    int sendHeartbeat() const;
};
