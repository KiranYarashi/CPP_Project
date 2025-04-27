#include "Client.h"
#include "Database.h"
#include "Proposal.h"
#include "Policy.h"
#include "CanclePolicy.h"
#include "PaymentManager.h"



#include <iostream>
#include <string>
 
using namespace std;
 
        // Display   client options
void Client::showClientMenu() {
    int choice;
    PaymentManager paymentManager;


    cout << "\n=== Client Dashboard ===\n";
    cout << "1. List Existing Clients\n";
    cout << "2. Create New Client\n";
    cout << "3. Make Payments\n";
    
    cout << "Enter choice: ";
    cin >> choice;
 
    if (choice == 1) {
        listClients();
        selectClient();
    } else if (choice == 2) {
        createClient();
    } else if(choice == 3){
        paymentManager.makePayment();
    }else {
        cout << "Invalid choice.\n";
    }
}
 
// Create new Client
void Client::createClient() {
    string first_name, last_name, dob, email, mobile, gender, city, education, occupation;
    int tobacco;
    double annual_income;
 
    cout << "Enter First Name: ";
    cin >> first_name;
    cout << "Enter Last Name: ";
    cin >> last_name;
    cout << "Enter DOB (YYYY-MM-DD): ";
    cin >> dob;
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Mobile: ";
    cin >> mobile;
    cout << "Enter Gender (M/F/Other): ";
    cin >> gender;
    cout << "Do you consume tobacco? (1 = Yes, 0 = No): ";
    cin >> tobacco;
    cout << "Enter Annual Income: ";
    cin >> annual_income;
    cout << "Enter City: ";
    cin >> city;
    cout << "Enter Education: ";
    cin >> education;
    cout << "Enter Occupation: ";
    cin >> occupation;
 
    string sql = "INSERT INTO clients (first_name, last_name, dob, email, mobile, gender, tobacco, annual_income, city, education, occupation) " "VALUES ('" + first_name + "', '" + last_name + "', '" + dob + "', '" + email + "', '" + mobile + "', '" + gender + "', " + to_string(tobacco) + ", " + to_string(annual_income) + ", '" + city + "', '" + education + "', '" + occupation + "');";
 
    char* errMsg;
    sqlite3* db = Database::getInstance()->getDB();
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "Error creating client: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Client created successfully!\n";
    }
}
 
// list all clients
void Client::listClients() {
    string sql = "SELECT client_id, first_name, last_name FROM clients;";
    sqlite3* db = Database::getInstance()->getDB();
 
    auto callback = [](void* NotUsed, int argc, char** argv, char** azColName) -> int {
        cout << "ID: " << argv[0] << " | Name: " << argv[1] << " " << argv[2] << endl;
        return 0;
    };
 
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "Error listing clients: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}
 
// Select a client by ID
int Client::selectClient() {
    int client_id;
    cout << "Enter Client ID to select: ";
    cin >> client_id;
    cout << "Client with ID " << client_id << " selected.\n";
    return client_id;
}


// #include "PolicyManager.h"
// #include "PaymentManager.h"
// #include "NotificationManager.h"

// //  cliet dashboard 
// void Client::clientDashboard(int clientId) {
//     PolicyManager policyManager;
//     Proposal proposal;
//     Policy policy;
//     CanclePolicy canclePolicy;
 
//     while (true) {
//         cout << "\n--- Client Dashboard ---\n";
//         cout << "1. Create New Proposal\n";
//         cout << "2. View Policies\n";
//         cout << "3. Cancel Policy\n";
//         cout << "4. View Notifications\n";
//         cout << "5. View Payments\n";
//         cout << "4. Exit\n";
//         cout << "Enter choice: ";
 
//         int choice;
//         cin >> choice;
 
//         switch (choice) {
//             case 1:
//             proposal.createProposal(clientId);
//                 break;
//             case 2:
//                 policyManager.viewPolicies(clientId);
//                 break;
//             case 3:
//                 policyManager.cancelPolicy(clientId);
//                 break;
//             case 4:
//                 return;
//             default:
//                 cout << "Invalid choice.\n";
//         }
//     }
// }





// -----------------------------------------------------------------------------

#include "NotificationManager.h"
#include "PaymentManager.h"
#include "PolicyManager.h"
#include "Proposal.h"
#include <iostream>
using namespace std;
 
// updated clientDashboard inside Client class
void Client::clientDashboard(int clientId) {
    PolicyManager policyManager;
    Proposal proposal;
    NotificationManager notificationManager;
    PaymentManager paymentManager;
 
    int choice;
    do {
        cout << "\n--- Client Dashboard ---\n";
        cout << "1. Create New Proposal\n";
        cout << "2. View Policies\n";
        cout << "3. Cancel a Policy\n";
        cout << "4. View Notifications\n";
        cout << "5. View Payment History\n";
        cout << "Enter your choice: ";
        cin >> choice;
 
        switch (choice) {
            case 1:
                proposal.createProposal(clientId);
                break;
            case 2:
                policyManager.viewPolicies(clientId);
                break;
            case 3:
                policyManager.cancelPolicy(clientId);
                break;
            case 4:
                notificationManager.viewNotifications(clientId);
                break;
            case 5:
                paymentManager.viewPayments(clientId);
                break;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 0);
}