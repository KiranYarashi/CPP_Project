#include "Approver.h"
#include "Database.h"
#include <iostream>
#include <sqlite3.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
 
void Approver::login() {
    string username, password;
    cout << "Approver Login:\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
 
    sqlite3* db = Database::getInstance()->getDB();
    string sql = "SELECT user_id FROM users WHERE username='" + username + "' AND password='" + password + "' AND is_approve=1;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int userId = sqlite3_column_int(stmt, 0);
            cout << "Login successful.\n";
            approveProposals(userId);
        } else {
            cout << "Invalid credentials or not an approver.\n";
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Login query failed.\n";
    }
}
 
void Approver::approveProposals(int userId) {
    sqlite3* db = Database::getInstance()->getDB();
    string sql = "SELECT proposal_id, client_id, policy_type, life_cover, cover_upto_age, total_premium FROM proposals WHERE status='Pending';";
    sqlite3_stmt* stmt;
 
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to fetch proposals.\n";
        return;
    }
 
    cout << "\nPending Proposals:\n";
    vector<int> proposalIds;
 
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int clientId = sqlite3_column_int(stmt, 1);
        string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        double cover = sqlite3_column_double(stmt, 3);
        int age = sqlite3_column_int(stmt, 4);
        double premium = sqlite3_column_double(stmt, 5);
 
        proposalIds.push_back(id);
        cout << "Proposal ID: " << id << ", Type: " << type << ", Life Cover: " << cover
             << ", Upto Age: " << age << ", Premium: Rs." << premium << endl;
    }
 
    sqlite3_finalize(stmt);
 
    if (proposalIds.empty()) {
        cout << "No proposals to approve.\n";
        return;
    }
 
    int selectedId;
    cout << "Enter Proposal ID to approve: ";
    cin >> selectedId;
 
    if (find(proposalIds.begin(), proposalIds.end(), selectedId) == proposalIds.end()) {
        cout << "Invalid Proposal ID.\n";
        return;
    }
 
    // Get proposal details
    sql = "SELECT client_id FROM proposals WHERE proposal_id=" + to_string(selectedId) + ";";
    int clientId = -1;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            clientId = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
 
    if (clientId == -1) {
        cout << "Failed to get client ID.\n";
        return;
    }
 
    // Generate policy number (simple: "POL" + current timestamp)
    time_t now = time(0);
    tm* t = localtime(&now);
    stringstream ss;
    ss << "POL" << put_time(t, "%Y%m%d%H%M%S");
    string policyNumber = ss.str();
 
    // Start date
    char startDate[20];
    strftime(startDate, sizeof(startDate), "%Y-%m-%d", localtime(&now));
 
    // Insert into policies
    sql = "INSERT INTO policies (proposal_id, policy_number, client_id, approved_by, start_date, status) "
          "VALUES (" + to_string(selectedId) + ", '" + policyNumber + "', " + to_string(clientId) + ", " +
          to_string(userId) + ", '" + startDate + "', 'Active');";
 
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "Policy creation failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }
 
    // Update proposal status
    sql = "UPDATE proposals SET status='Approved' WHERE proposal_id=" + to_string(selectedId) + ";";
    rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
 
    if (rc != SQLITE_OK) {
        cerr << "Failed to update proposal status: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Proposal approved successfully. Policy Number: " << policyNumber << endl;
    }
}
 