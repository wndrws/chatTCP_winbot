#ifdef __cplusplus
extern "C" {
#endif
char* program_name;
#ifdef __cplusplus
}
#endif

#include <iostream>
#include <queue>
#include <conio.h>
#include "etcp.h"
#include "ChatServer.h"

using namespace std;

ChatServer chatServer;
queue<string> MessagesToSend;

int main(int argc, char** argv) {
    //struct sockaddr_in peer;
    SOCKET s;

    if(argc < 3) {
        cout << "Please provide IP address and Port of the server." << endl;
        return 1;
    }

    INIT();

    s = tcp_client(argv[1], argv[2]);
    chatServer = ChatServer(s);

    cout << "Chat bot operational. Press any key to stop the program." << endl;
    chatServer.login(""); //Chat bot has id only
    chatServer.receiveUsersList();

    unsigned char code = 255;
    int rcvdb, r;
    bool ok = true;
    u_long mode = 0;
    int hbcnt = 0;
    int timeQuantum = 200; // sleep time between readings from non-blocking socket (ms)
    int timeHb = 10000; // time to send the first heartbeat (ms)

    while(!kbhit() && ok) {
        // Broadcast incoming messages
        for(auto&& pen : chatServer.getPendingMap()) {
            if(!pen.second.empty()) { // If there are messages from user with id = pen.first
                auto it = pen.second.begin(); // Iterator to the beginning of vector of messages
                for( ; it != pen.second.end(); ++it) {
                    for(auto&& user : chatServer.getUsersMap()) {
                        if (!user.second.empty() && // If name of user is not empty (empty name is only for bot)
                                user.first != pen.first) {  // and there shouldn't be echo for message author
                            chatServer.setCurrentPeer(user.first);
                            r = chatServer.sendMessage("[ " + chatServer.getFullName(pen.first) + " ]: " + *it);
                            if (r < 0) {
                                cerr << "Failed to send message to " << chatServer.getFullName(user.first) << endl;
                                continue;
                            }
                        }
                    }
                }
                pen.second.clear(); // erase sent messages
            }
        }
        // Receive packets
        if(mode == 0) {
            mode = 1;
            ioctlsocket(s, FIONBIO, &mode); // Make socket non-blocking
        }
        rcvdb = readn(s, (char*) &code, 1);
        if (rcvdb == 0) {
            cout << "[ Server closed the connection ]" << endl;
            break;
        } else if(rcvdb < 0) {
            int error = WSAGetLastError();
            if(error == WSAEWOULDBLOCK || error == WSAEINTR) {
                // It's ok, continue doing job after some time
                Sleep((DWORD) timeQuantum);
                if(++hbcnt == timeHb/timeQuantum) chatServer.sendHeartbeat();
                else if(hbcnt == 2*timeHb/timeQuantum) chatServer.sendHeartbeat();
                else if(hbcnt == 3*timeHb/timeQuantum) chatServer.sendHeartbeat();
                else if(hbcnt == 4*timeHb/timeQuantum) {
                    cerr << "Error: server is not responding." << endl;
                    break;
                }
            } else {
                cerr << "Error: reading from socket " << s << endl;
                break;
            }
        } else {
            mode = 0;
            ioctlsocket(s, FIONBIO, &mode); // Make socket blocking again
            switch(code) {
                case CODE_OUTMSG: // Incoming message from some client
                    chatServer.receiveMessage();
                    break;
                case CODE_SRVERR:
                    ok = false;
                    // no break here
                case CODE_SRVMSG:
                    cout << "[ Message from server ]" << endl;
                    cout << chatServer.receiveServerMessage();
                    cout << "[ End of message ]" << endl;
                    break;
                case CODE_FORCEDLOGOUT:
                    cout << "[ Chat bot was forced to logout by server ]" << endl;
                    ok = false;
                    break;
                case CODE_LOGINNOTIFY:
                    chatServer.receiveLoginNotification();
                    break;
                case CODE_LOGOUTNOTIFY:
                    chatServer.receiveLogoutNotification();
                    break;
                case CODE_HEARTBEAT:
                    hbcnt = 0;
                    break;
                default:
                    cerr << "Warning: Packet with unknown code is received!" << endl;
                    break;
            }
        }
    }
    chatServer.logout();
    cout << "Disconnecting from server..." << endl;
    shutdown(s, SD_BOTH);
    cout << "Closing socket and exiting..." << endl;
    CLOSE(s);
    EXIT(0);
}