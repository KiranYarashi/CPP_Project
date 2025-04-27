#include "Auth.h"
#include "Database.h"
#include <iostream>
#include <string>
 
using namespace std;
 

// singup

void Auth::signUp() {
    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;
 
    string sql = "INSERT INTO users (username, password, is_approve) VALUES ('" + username + "', '" + password + "', 0);";
 
    char* errMsg;
    sqlite3* db = Database::getInstance()->getDB();
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "SignUp Failed: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Account created successfully!" << endl;
    }
}
 

//   Sigh In
bool Auth::signIn() {
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
 
    string sql = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';";
    bool found = false;
 
    auto callback = [](void* data, int argc, char** argv, char**) {
        bool* f = static_cast<bool*>(data);
        *f = true;
        return 0;
    };
 
    char* errMsg;
    sqlite3* db = Database::getInstance()->getDB();
    int rc = sqlite3_exec(db, sql.c_str(), callback, &found, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "SignIn Failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
 
    if (found) {
        cout << "Login successful!" << endl;
        return true;
    } else {
        cout << "Invalid credentials." << endl;
        return false;
    }
}