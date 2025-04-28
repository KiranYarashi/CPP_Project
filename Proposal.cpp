#include "Proposal.h"
#include "Database.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

int calculateAge(const string &dob)
{
    int year, month, day;
    sscanf(dob.c_str(), "%d-%d-%d", &year, &month, &day);

    time_t now = time(0);
    tm *ltm = localtime(&now);

    int age = ltm->tm_year + 1900 - year;
    if ((ltm->tm_mon + 1 < month) || ((ltm->tm_mon + 1 == month) && (ltm->tm_mday < day)))
    {
        age--;
    }
    return age;
}
void Proposal::createProposal(int clientId)
{
    cout << "\n=== Create New Proposal ===\n";

    // Fetch client details from the database
    sqlite3 *db = Database::getInstance()->getDB();
    string sql = "SELECT first_name, last_name, dob, email, mobile, gender, tobacco, annual_income, city, education, occupation "
                 "FROM clients WHERE client_id = " + to_string(clientId) + ";";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to fetch client details: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string firstName, lastName, dob, email, mobile, gender, city, education, occupation;
    int tobacco;
    double income;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        firstName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        lastName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        dob = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        mobile = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
        gender = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
        tobacco = sqlite3_column_int(stmt, 6);
        income = sqlite3_column_double(stmt, 7);
        city = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
        education = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
        occupation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10));
    }
    else
    {
        cerr << "Client not found.\n";
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Step 1: Choose insurance type
    cout << "Select Insurance Type:\n";
    cout << "1. Term Insurance Policy\n";
    cout << "2. Whole Life Insurance Policy\n";
    cout << "3. Money Back Policy\n";
    cout << "4. Endowment Policy\n";
    int policyType;
    cin >> policyType;
    string typeStr;

    switch (policyType)
    {
    case 1:
        typeStr = "Term Insurance";
        break;
    case 2:
        typeStr = "Whole Life Insurance";
        break;
    case 3:
        typeStr = "Money Back";
        break;
    case 4:
        typeStr = "Endowment";
        break;
    default:
        cout << "Invalid option.\n";
        return;
    }

    // Step 2: Collect additional inputs
    double coverAmount;
    int coverTillAge;

    cout << "Life Cover Amount (max 3 Cr): ";
    cin >> coverAmount;
    cout << "Cover till Age (max 100): ";
    cin >> coverTillAge;

    if (coverAmount > 30000000 || coverTillAge > 100)
    {
        cout << "Invalid input: Check max limits.\n";
        return;
    }

    // Step 3: Age and base premium calculation
    int age = calculateAge(dob);
    double basePremium = (coverAmount / 1000) * (tobacco ? 2.0 : 1.0) * (100 - age) * 0.1;

    cout << "Calculated Age: " << age << "\n";
    cout << "Base Monthly Premium: Rs." << basePremium << "\n";

    // Step 4: Adjust life cover and age
    char adjust;
    cout << "Do you want to adjust Life Cover Amount or Cover till Age? (Y/N): ";
    cin >> adjust;
    if (adjust == 'Y' || adjust == 'y')
    {
        cout << "New Life Cover Amount (max 3 Cr): ";
        cin >> coverAmount;
        cout << "New Cover till Age (max 100): ";
        cin >> coverTillAge;

        if (coverAmount > 30000000 || coverTillAge > 100)
        {
            cout << "Invalid input: Check max limits.\n";
            return;
        }

        basePremium = (coverAmount / 1000) * (tobacco ? 2.0 : 1.0) * (100 - age) * 0.1;
        cout << "Updated Base Monthly Premium: Rs." << basePremium << "\n";
    }

    // Step 5: Accident Death Benefit
    int adbCover;
    cout << "Enter Accident Death Benefit Cover (min Rs.25000 in multiples of Rs.50000): ";
    cin >> adbCover;
    double adbPremium = 0;

    if (adbCover >= 25000)
    {
        adbPremium = (adbCover / 50000) * 10; // Rs.10 per Rs.50K
    }

    // Step 6: Comprehensive Care
    int compCareCover;
    cout << "Enter Comprehensive Care Cover (min Rs.2L, in multiples of Rs.3L): ";
    cin >> compCareCover;
    double compPremium = 0;

    if (compCareCover >= 200000)
    {
        compPremium = 80 + ((compCareCover - 200000) / 300000) * 400;
    }

    // Step 7: Total premium + tax
    double totalPremium = basePremium + adbPremium + compPremium;
    double tax = totalPremium * 0.18;
    double finalPremium = totalPremium + tax;

    cout << "\n--- Premium Summary ---\n";
    cout << "Base Premium: Rs." << basePremium << endl;
    cout << "ADB Premium: Rs." << adbPremium << endl;
    cout << "Comprehensive Care Premium: Rs." << compPremium << endl;
    cout << "Tax (18%): Rs." << tax << endl;
    cout << "Final Monthly Premium: Rs." << finalPremium << endl;

    // Step 8: Payment Tenure
    int payTillAge;
    cout << "Enter Pay till Age (options: 80/60/10/5): ";
    cin >> payTillAge;

              // Step 9: Payment Frequency
        int frequency;
        cout << "Payment Frequency (1-Monthly, 2-Half-Yearly, 3-Annually): ";
        cin >> frequency;
        double finalAdjustedPremium = finalPremium;
        
        if (frequency == 2) // Half-Yearly
        {
            finalAdjustedPremium *= 0.9; // 10% discount
        }
        else if (frequency == 3) // Annually
        {
            finalAdjustedPremium *= 0.85; // 15% discount
        }
        
        // Display the updated premium summary
        cout << "\n--- Updated Premium Summary Based on Payment Frequency ---\n";
        cout << "Base Premium: Rs." << basePremium << endl;
        cout << "ADB Premium: Rs." << adbPremium << endl;
        cout << "Comprehensive Care Premium: Rs." << compPremium << endl;
        cout << "Tax (18%): Rs." << tax << endl;
        cout << "Final Adjusted Premium: Rs." << finalAdjustedPremium << endl;
    // Step 10: Payment Mode
    int mode;
    cout << "Select Payment Mode:\n";
    cout << "1. Cash\n2. Credit Card\n3. Debit Card\n4. Net Banking\n5. UPI\n6. Paytm\n";
    cin >> mode;

    string modeStr;
    switch (mode)
    {
    case 1:
        modeStr = "Cash";
        break;
    case 2:
        modeStr = "Credit Card";
        break;
    case 3:
        modeStr = "Debit Card";
        break;
    case 4:
        modeStr = "Net Banking";
        break;
    case 5:
        modeStr = "UPI";
        break;
    case 6:
        modeStr = "Paytm";
        break;
    default:
        modeStr = "Unknown";
    }

    // Accept terms
    char accept;
    cout << "Do you accept terms and conditions? (Y/N): ";
    cin >> accept;
    if (accept != 'Y' && accept != 'y')
    {
        cout << "Proposal cancelled by user.\n";
        return;
    }

    // Generate Proposal ID
    srand(time(0));
    int proposalId = rand() % 1000000 + 1;

    // Get current date as string
    time_t now = time(0);
    char createdAt[20];
    strftime(createdAt, sizeof(createdAt), "%Y-%m-%d", localtime(&now));

    // Step 11: Save proposal to database
    sql = "INSERT INTO proposals (proposal_id, client_id, policy_type, life_cover, cover_upto_age, basic_premium, "
          "accident_cover, comprehensive_cover, total_premium, payment_tenure, payment_mode, payment_method, "
          "status, created_at) VALUES (" +
          to_string(proposalId) + ", " + to_string(clientId) + ", '" + typeStr + "', " +
          to_string(coverAmount) + ", " + to_string(coverTillAge) + ", " + to_string(basePremium) + ", " +
          to_string(adbCover) + ", " + to_string(compCareCover) + ", " +
          to_string(finalAdjustedPremium) + ", '" + to_string(payTillAge) + "', '" +
          (frequency == 1 ? "Monthly" : (frequency == 2 ? "Half-Yearly" : "Annual")) + "', '" +
          modeStr + "', 'Pending', '" + createdAt + "');";

    char *errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "Failed to create proposal: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
    else
    {
        cout << "\nProposal submitted successfully! Proposal ID: " << proposalId << "\n";
    }
}