#include <iostream>
#include "Auth.h"
#include "Client.h"
#include "Approver.h"
#include "PolicyManager.h"

using namespace std;

int main() {
    // creating objects
    Auth auth;
    Client client;
    PolicyManager policyManager;
    Approver approver;

    int choice;
    cout << "\n1. Sign Up\n2. Sign In\n3. Approver Sign In\nChoice: ";
    cin >> choice;

    bool loggedIn = false;

    if (choice == 1) {
        auth.signUp();
    } else if (choice == 2) {
        loggedIn = auth.signIn();
    } else if (choice == 3) {
        approver.login();
    }

    if (loggedIn) {
     
        // int clientId = client.selectClient(); // Prompt user to select a client
        client.showClientMenu();             // Show client menu
    }

    return 0;
}