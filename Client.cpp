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
void Client::showClientMenu()
{
    int choice;
    PaymentManager paymentManager;
    Client client;

    cout << "\n=== Client Dashboard ===\n";
    cout << "1. List Existing Clients\n";
    cout << "2. Create New Client\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1)
    {
        listClients();
        int clientId = selectClient();
        client.clientDashboard(clientId);  
    }
    else if (choice == 2)
    {
        createClient();
    }
    else
    {
        cout << "Invalid choice.\n";
    }
}

// Create new Client
void Client::createClient()
{
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

    string sql = "INSERT INTO clients (first_name, last_name, dob, email, mobile, gender, tobacco, annual_income, city, education, occupation) "
                 "VALUES ('" +
                 first_name + "', '" + last_name + "', '" + dob + "', '" + email + "', '" + mobile + "', '" + gender + "', " + to_string(tobacco) + ", " + to_string(annual_income) + ", '" + city + "', '" + education + "', '" + occupation + "');";

    char *errMsg;
    sqlite3 *db = Database::getInstance()->getDB();
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "Error creating client: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
    else
    {
        cout << "Client created successfully!\n";
    }
}

// list all clients
void Client::listClients()
{
    string sql = "SELECT client_id, first_name, last_name FROM clients;";
    sqlite3 *db = Database::getInstance()->getDB();

    auto callback = [](void *NotUsed, int argc, char **argv, char **azColName) -> int
    {
        cout << "ID: " << argv[0] << " | Name: " << argv[1] << " " << argv[2] << endl;
        return 0;
    };

    char *errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "Error listing clients: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

// Select a client by ID
int Client::selectClient()
{
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
void Client::clientDashboard(int clientId)
{
    PolicyManager policyManager;
    Proposal proposal;
    NotificationManager notificationManager;
    PaymentManager paymentManager;

    int choice;
    do
    {
        cout << "\n--- Client Dashboard ---\n";
        cout << "1. Create New Proposal\n";
        cout << "2. View Policies\n";
        cout << "3. Cancel a Policy\n";
        cout<< "4. View Notifications\n";
        cout << "5. View Payment History\n";
        cout << "6. Make a Payment\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
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
        case 6:
            makePayment();
            break;
        default:
            cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 0);
}

// ------------------------------------------------------------------------------
// make Payment page

#include <ctime>
#include <string>
void Client::makePayment() {
    sqlite3* db = Database::getInstance()->getDB();
    
    int policyId;
    double amount;
    string paymentMode, paymentMethod;

    cout << "---- Make Payment ----\n";

    // Fetch active policies
    string sql = "SELECT p.policy_id "
                 "FROM policies p "
                 "WHERE p.status = 'Active';";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch active policies: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Display active policies
    cout << "Active Policies:\n";
    bool hasPolicies = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasPolicies = true;
        int id = sqlite3_column_int(stmt, 0);
        cout << "Policy ID: " << id << endl;
    }

    if (!hasPolicies) {
        cout << "No active policies found.\n";
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Select Policy ID
    cout << "\nSelect Policy ID: ";
    cin >> policyId;

    // Fetch premium amount
    sql = "SELECT po.total_premium "
          "FROM proposals po "
          "JOIN policies p ON po.proposal_id = p.proposal_id "
          "WHERE p.policy_id = ?;";
    sqlite3_stmt* stmt1;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt1, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch policy premium: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt1, 1, policyId);

    cout << "\nPolicy Premium Amount: ";
    if (sqlite3_step(stmt1) == SQLITE_ROW) {
        double premiumAmount = sqlite3_column_double(stmt1, 0);
        cout << premiumAmount << endl;
    } else {
        cerr << "\nNo premium amount found for the selected policy.\n";
        sqlite3_finalize(stmt1);
        return;
    }
    sqlite3_finalize(stmt1);

    // Enter payment details
    cout << "\nEnter Payment Amount: ";
    cin >> amount;
    cout << "\nEnter Payment Mode (Cash/Cheque/Online): ";
    cin >> paymentMode;
    cout << "\nEnter Payment Method (Bank Transfer/Credit Card/Debit Card): ";
    cin >> paymentMethod;

    // Get current date
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string date = to_string(1900 + ltm->tm_year) + "-" +
                  to_string(1 + ltm->tm_mon) + "-" +
                  to_string(ltm->tm_mday);

    // Insert payment record
    sql = "INSERT INTO payment_records (policy_id, amount_paid, payment_mode, payment_method, paid_on) "
          "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* insertStmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &insertStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(insertStmt, 1, policyId);
    sqlite3_bind_double(insertStmt, 2, amount);
    sqlite3_bind_text(insertStmt, 3, paymentMode.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStmt, 4, paymentMethod.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStmt, 5, date.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(insertStmt) != SQLITE_DONE) {
        cerr << "\nFailed to insert payment record: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "\nPayment record inserted successfully.\n";
    }
    sqlite3_finalize(insertStmt);

    // Close database connection
    sqlite3_close(db);

    cout << "Payment process completed.\n";
    cout << "------------------------\n";
    cout << "Thank you for your payment!\n";
    cout << "------------------------\n";
}