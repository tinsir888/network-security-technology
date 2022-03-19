#include "generalheader.h"

using namespace std;

int main() {
    cout << "————————chat start————————" << endl;
    while(1) {
        cout << "Client or server? s for server, c for client, q for quit the program" << endl;
        string client_or_server;
        cin >> client_or_server;
        if (client_or_server == "s") {
            if(server() != 0){
                break;
            }
        }
        else if (client_or_server == "c") {
            if(client() != 0){
                break;
            }
        }
        else if(client_or_server == "q"){
            cout << "Successfully quit the program!" << endl;
            break;
        }
        else {
            cout << "Error input! Please type s for server, c for client." << endl;
        }
    }
    cout << "————————chat end————————" << endl;
    return 0;
}
