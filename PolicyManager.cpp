#include "PolicyManager.h"
#include "Database.h"
#include <iostream>
#include <sqlite3.h>
#include <ctime>
#include <vector>
 
using namespace std;
 
void PolicyManager::viewPolicies(int clientId) {
    sqlite3* db = Database::getInstance()->getDB();
    string sql = "SELECT policy_id, policy_number, start_date, status FROM policies WHERE client_id=" + to_string(clientId) + ";";
 
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch policies.\n";
        return;
    }
 
    cout << "\nYour Policies:\n";
    bool found = false;
 
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
 
        cout << "Policy ID: " << id << ", Number: " << number << ", Start Date: " << date << ", Status: " << status << endl;
        found = true;
    }
 
    sqlite3_finalize(stmt);
 
    if (!found) {
        cout << "No policies found.\n";
    }
}
 
void PolicyManager::cancelPolicy(int clientId) {
    viewPolicies(clientId); // Show user current policies
 
    int policyId;
    cout << "\nEnter Policy ID to cancel: ";
    cin >> policyId;
 
    sqlite3* db = Database::getInstance()->getDB();
 
    // Check if policy exists and is active
    string checkSql = "SELECT status FROM policies WHERE policy_id=" + to_string(policyId) +
                      " AND client_id=" + to_string(clientId) + ";";
 
    sqlite3_stmt* stmt;
    string status;
 
    if (sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK &&
        sqlite3_step(stmt) == SQLITE_ROW) {
        status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
 
        if (status != "Active") {
            cout << "Policy is already cancelled or inactive.\n";
            return;
        }
    } else {
        cout << "Policy not found.\n";
        return;
    }
 
    string reason;
    cout << "Enter reason for cancellation: ";
    cin.ignore();
    getline(cin, reason);
 
    // Get cancel date
    time_t now = time(0);
    char cancelDate[20];
    strftime(cancelDate, sizeof(cancelDate), "%Y-%m-%d", localtime(&now));
 
    // Insert into cancelled_policies
    string cancelSql = "INSERT INTO cancelled_policies (policy_id, client_id, reason, cancel_date) VALUES (" +
                       to_string(policyId) + ", " + to_string(clientId) + ", '" + reason + "', '" + cancelDate + "');";
 
    char* errMsg;
    int rc = sqlite3_exec(db, cancelSql.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Cancellation failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }
 
    // Update policy status
    string updateSql = "UPDATE policies SET status='Cancelled' WHERE policy_id=" + to_string(policyId) + ";";
    rc = sqlite3_exec(db, updateSql.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Failed to update policy status: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
 
    // Send notification
    string msg = "Your policy ID " + to_string(policyId) + " has been cancelled.";
    string notifSql = "INSERT INTO notifications (client_id, message, timestamp) VALUES (" + to_string(clientId) +
                      ", '" + msg + "', '" + cancelDate + "');";
    rc = sqlite3_exec(db, notifSql.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Failed to send notification: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Policy cancelled successfully and client notified.\n";
    }
}