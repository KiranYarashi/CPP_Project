#include <iostream>
#include "sqlite3.h" // Make sure this file is in your folder

using namespace std;

// Callback function to print query results
int callback(void* NotUsed, int argc, char** argv, char** colName) {
    for (int i = 0; i < argc; i++) {
        cout << colName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << "-------------------" << endl;
    return 0;
}

int main() {
    sqlite3* db;
    char* errMsg = nullptr;

    // Open or create a database
    int rc = sqlite3_open("e.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Failed to open DB: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    cout << "Database opened successfully!" << endl;

    // Create a users table
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            password TEXT NOT NULL
        );
    )";

    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error creating table: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Table created successfully!" << endl;
    }

    // Insert a user
    const char* insertSQL = R"(
        INSERT INTO users (username, password) VALUES ('kiran', 'kiran123');
    )";
    sqlite3_exec(db, insertSQL, nullptr, nullptr, nullptr);

    // Query and print users
    const char* selectSQL = "SELECT * FROM users;";
    sqlite3_exec(db, selectSQL, callback, nullptr, nullptr);

    // Close the database
    sqlite3_close(db);
    return 0;
}
