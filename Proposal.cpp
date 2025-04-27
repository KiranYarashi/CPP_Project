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

    // Step 1: Choose insurance type
    cout << "Select Insurance Type:\n";
    cout << "1. Term Insurance\n";
    cout << "2. Whole Life\n";
    cout << "3. Money Back\n";
    cout << "4. Endowment\n";
    int policyType;
    cin >> policyType;
    string typeStr;

    switch (policyType)
    {
    case 1:
        typeStr = "Term";
        break;
    case 2:
        typeStr = "Whole Life";
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

    // Step 2: Collect user input
    string firstName, lastName, dob, email, mobile, gender;
    int tobacco;
    double income, coverAmount;
    int coverTillAge;

    cout << "First Name: ";
    cin >> firstName;
    cout << "Last Name: ";
    cin >> lastName;
    cout << "DOB (YYYY-MM-DD): ";
    cin >> dob;
    cout << "Email: ";
    cin >> email;
    cout << "Mobile: ";
    cin >> mobile;
    cout << "Gender (M/F): ";
    cin >> gender;
    cout << "Consume Tobacco? (1=Yes, 0=No): ";
    cin >> tobacco;
    cout << "Annual Income: ";
    cin >> income;
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

    // Step 4: Accident Death Benefit
    int adbCover;
    cout << "Enter Accident Death Benefit Cover (min Rs.25000 in multiples of Rs.50000): ";
    cin >> adbCover;
    double adbPremium = 0;

    if (adbCover >= 25000)
    {
        adbPremium = (adbCover / 50000) * 10; // Rs.10 per Rs.50K
    }

    // Step 5: Comprehensive Care
    int compCareCover;
    cout << "Enter Comprehensive Care Cover (min Rs.2L, in multiples of Rs.3L): ";
    cin >> compCareCover;
    double compPremium = 0;

    if (compCareCover >= 200000)
    {
        compPremium = 80 + ((compCareCover - 200000) / 300000) * 400;
    }

    // Step 6: Total premium + tax
    double totalPremium = basePremium + adbPremium + compPremium;
    double tax = totalPremium * 0.18;
    double finalPremium = totalPremium + tax;

    cout << "\n--- Premium Summary ---\n";
    cout << "Base Premium: Rs." << basePremium << endl;
    cout << "ADB Premium: Rs." << adbPremium << endl;
    cout << "Comprehensive Care Premium: Rs." << compPremium << endl;
    cout << "Tax (18%): Rs." << tax << endl;
    cout << "Final Monthly Premium: Rs." << finalPremium << endl;

    // Step 7: Payment Tenure
    int payTillAge;
    cout << "Enter Pay till Age (options: 80/60/10/5): ";
    cin >> payTillAge;

    string city, education, occupation;
    cout << "City: ";
    cin >> city;
    cout << "Educational Qualification: ";
    cin >> education;
    cout << "Occupation: ";
    cin >> occupation;

    // Step 8: Payment Frequency
    int frequency;
    cout << "Payment Frequency (1-Monthly, 2-Half-Yearly, 3-Annually): ";
    cin >> frequency;
    double finalAdjustedPremium = finalPremium;

    if (frequency == 2)
        finalAdjustedPremium *= 0.9;
    else if (frequency == 3)
        finalAdjustedPremium *= 0.85;

    // Step 9: Payment Mode
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
    // Get current date as string
    time_t now = time(0);
    char createdAt[20];
    strftime(createdAt, sizeof(createdAt), "%Y-%m-%d", localtime(&now));

    // Step 10: Save proposal to database
    sqlite3 *db = Database::getInstance()->getDB();

    string sql = "INSERT INTO proposals (client_id, policy_type, life_cover, cover_upto_age, basic_premium, "
                 "accident_cover, comprehensive_cover, total_premium, payment_tenure, payment_mode, payment_method, "
                 "status, created_at) VALUES (" +
                 to_string(clientId) + ", '" + typeStr + "', " + to_string(coverAmount) + ", " +
                 to_string(coverTillAge) + ", " + to_string(basePremium) + ", " +
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
        cout << "\nProposal submitted successfully! Awaiting approval.\n";
    }
}
