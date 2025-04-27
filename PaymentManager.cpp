#include "PaymentManager.h"
#include "Database.h"
#include <iostream>
#include <sqlite3.h>
 
using namespace std;
 
void PaymentManager::viewPayments(int clientId) {
    sqlite3* db = Database::getInstance()->getDB();
    string sql = "SELECT pr.amount_paid, pr.payment_mode, pr.payment_method, pr.paid_on "
                 "FROM payment_records pr "
                 "JOIN policies p ON pr.policy_id = p.policy_id "
                 "WHERE p.client_id = " + to_string(clientId) + " ORDER BY pr.paid_on DESC;";
 
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch payment records.\n";
        return;
    }
 
    cout << "\nPayment History:\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        double amount = sqlite3_column_double(stmt, 0);
        string mode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string method = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
 
        cout << date << " | Rs. " << amount << " | " << mode << " | via " << method << endl;
        found = true;
    }
 
    sqlite3_finalize(stmt);
 
    if (!found) {
        cout << "No payment records found.\n";
    }
}


#include <ctime>
#include <string>


void PaymentManager::makePayment() {
    sqlite3* db = Database::getInstance()->getDB();
    
    int policyId;
    double amount;
    string paymentMode, paymentMethod;

    cout << "---- Make Payment ----\n";

    // Corrected SQL query to show the active policies of the client
    string sql = "SELECT p.policy_id "
                 "FROM policies p "
                 "WHERE p.status = 'Active';";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch active policies: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Display the active policies
    cout << "Active Policies:\n";
    bool hasPolicies = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasPolicies = true;
        int id = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double premium = sqlite3_column_double(stmt, 2);

        cout << "Policy ID: " << id  << endl;
    }

    if (!hasPolicies) {
        cout << "No active policies found.\n";
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Step 1: Select Policy ID
    cout << "\nSelect Policy ID: ";
    cin >> policyId;

    // Fetch the total premium for the selected policy
    // sql = "SELECT po.total_premium "
    //       "FROM proposals po "
    //       "JOIN policies p ON po.policy_id = p.policy_id "
    //       "WHERE p.policy_id = ?;";
      sql = "SELECT po.total_premium "
          "FROM proposals po "
          "JOIN policies p "
          "ON po.proposal_id = p.proposal_id "
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

    // Step 2: Enter Payment Amount
    cout << "\nEnter Payment Amount: ";
    cin >> amount;

    // Step 3: Enter Payment Mode
    cout << "\nEnter Payment Mode (Cash/Cheque/Online): ";
    cin >> paymentMode;

    // Step 4: Enter Payment Method
    cout << "\nEnter Payment Method (Bank Transfer/Credit Card/Debit Card): ";
    cin >> paymentMethod;

    // Step 5: Get the current date and time
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string date = to_string(1900 + ltm->tm_year) + "-" +
                  to_string(1 + ltm->tm_mon) + "-" +
                  to_string(ltm->tm_mday);

    // Step 6: Insert the payment record into the database
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

    // Close the database connection
    sqlite3_close(db);

    // End of the function
    cout << "Payment process completed.\n";
    cout << "------------------------\n";
    cout << "Thank you for your payment!\n";
    cout << "------------------------\n";
}