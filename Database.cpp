#include "Database.h"
#include <iostream>
using namespace std;

Database* Database::instance = nullptr;

Database::Database(){
    int exit = sqlite3_open("mydb.db", &DB);
    if(exit){
        cerr <<"DB connection Failed" << sqlite3_errmsg(DB) << endl;
        DB = nullptr;
    }else{
        cout<< "Hurry!! Connected to DB." << endl;
    }
    
}

Database* Database::getInstance(){
    if(instance == nullptr){
        instance = new Database();

    }
    return instance;
}


sqlite3* Database::getDB(){
    return DB;
}

Database::~Database(){
    if (DB) sqlite3_close(DB);
}