#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database{
private:
// Private Constructor: Prevents creating instances from outside the class.
// Static Instance: Holds the single instance of the class.
// Lazy Initialization: The instance is created only when getInstance() is called for the first time
    sqlite3* DB;
    static Database* instance;
    Database();

public:
    //used singlton design pattern
    static Database* getInstance();  
    sqlite3* getDB();
    ~Database();
};

#endif