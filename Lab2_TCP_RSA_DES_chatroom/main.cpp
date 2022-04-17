#include <iostream>
#include <stdio.h>
#include <limits>
#include "./include/cli_ser.h"

using namespace std;

int main() {
    cout << "———————————————— chat start ————————————————" << endl;
    while(1) {
        cout << "Client or Server? please type c for client, s for server" << endl;
        char server_or_client;
        cin >> server_or_client;
        if (server_or_client == 's') {
            int state = server();
            if(state != 0){
                break;
            }
        }
        else if (server_or_client == 'c') {
            int state = client();
            if(state != 0){
                break;
            }
        }
        else cout << "error input!" << endl;
    }
    cout << "———————————————— chat end ————————————————" << endl;
    return 0;
}
