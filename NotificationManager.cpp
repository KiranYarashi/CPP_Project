#include "NotificationManager.h"
#include "Database.h"
#include <iostream>
#include <sqlite3.h>
 
using namespace std;
 
void NotificationManager::viewNotifications(int clientId) {
    sqlite3* db = Database::getInstance()->getDB();
    string sql = "SELECT message, timestamp FROM notifications WHERE client_id = " + to_string(clientId) + " ORDER BY timestamp DESC;";
 
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch notifications.\n";
        return;
    }
 
    cout << "\nNotifications:\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string message = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "[" << time << "] " << message << endl;
        found = true;
    }
 
    sqlite3_finalize(stmt);
 
    if (!found) {
        cout << "No notifications found.\n";
    }
}