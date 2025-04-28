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
