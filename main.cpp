//
//  main.cpp
//  Cplus-project

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sqlite3.h>
#include <limits>

class BankAccount
{
private:
    int accountNumber;
    std::string name;
    double balance;
    
public:
    BankAccount(int accNumber, std::string accountName, double initialBalance) 
        : accountNumber(accNumber), name(accountName), balance(initialBalance) {}   
   
    void deposit(double amount){
        if(amount > 0){
            balance += amount;
        }
    }
    
    void withdraw(double amount){
        if(amount > 0 && amount <= balance){
            balance -= amount;
        }
    }
    
    void display() const {
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Account Name: " << name << std::endl;
        std::cout << "Balance: " << balance << std::endl;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    std::string getName() const {
        return name;
    }

    double getBalance() const {
        return balance;
    }

    int getAccountNumber() const {
        return accountNumber;
    }
    
};

void saveAccountToDatabase(const BankAccount& account, sqlite3* db) {
    std::string sql = "INSERT INTO accounts (account_number, name, balance) VALUES (" 
                      + std::to_string(account.getAccountNumber()) + ", '" 
                      + account.getName() + "', " 
                      + std::to_string(account.getBalance()) + ");";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Account saved successfully" << std::endl;
    }
}

void updateBalanceInDatabase(const BankAccount& account, sqlite3* db) {
    std::string sql = "UPDATE accounts SET balance = " + std::to_string(account.getBalance()) 
                      + " WHERE account_number = " + std::to_string(account.getAccountNumber()) + ";";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Account balance updated successfully" << std::endl;
    }
}

void updateAccountNameInDatabase(int accountNumber, const std::string& newName, sqlite3* db) {
    std::string sql = "UPDATE accounts SET name = '" + newName + "' WHERE account_number = " + std::to_string(accountNumber) + ";";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Account name updated successfully" << std::endl;
    }
}


void deleteAccountFromDatabase(int accountNumber, sqlite3* db) {
    std::string sql = "DELETE FROM accounts WHERE account_number = " + std::to_string(accountNumber) + ";";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Account deleted successfully" << std::endl;
    }
}

void viewAllAccounts(sqlite3* db) {
    const char* sql = "SELECT account_number, name, balance FROM accounts;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "All Accounts:" << std::endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int accountNumber = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double balance = sqlite3_column_double(stmt, 2);
        std::cout << "Account Number: " << accountNumber << ", Name: " << name << ", Balance: " << balance << std::endl;
    }

    sqlite3_finalize(stmt);
}

double getValidatedAmount() {
    double amount;
    while (true) {
        std::cin >> amount;
        if (std::cin.fail() || amount < 0) {
            std::cin.clear(); // clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter a positive number: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard any extra input
            return amount;
        }
    }
}

void transferFunds(BankAccount* fromAccount, BankAccount* toAccount, double amount, sqlite3* db) {
    if (fromAccount->getBalance() >= amount) {
        fromAccount->withdraw(amount);
        toAccount->deposit(amount);
        updateBalanceInDatabase(*fromAccount, db);
        updateBalanceInDatabase(*toAccount, db);
        std::cout << "Transfer successful" << std::endl;
    } else {
        std::cout << "Insufficient funds for transfer" << std::endl;
    }
}

int getValidatedChoice(int min, int max) {
    int choice;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail() || choice < min || choice > max) {
            std::cin.clear(); // clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard any extra input
            return choice;
        }
    }
}

BankAccount* searchAccount(int accountNumber, sqlite3* db) {
    std::string sql = "SELECT account_number, name, balance FROM accounts WHERE account_number = " + std::to_string(accountNumber) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    BankAccount* account = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int accountNumber = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double balance = sqlite3_column_double(stmt, 2);
        account = new BankAccount(accountNumber, name, balance);
        std::cout << "Account found: " << accountNumber << ", Name: " << name << ", Balance: " << balance << std::endl;
    } else {
        std::cout << "Account not found" << std::endl;
    }

    sqlite3_finalize(stmt);
    return account;
}


int main(int argc, const char * argv[]) {
    sqlite3* db;
    int rc = sqlite3_open("bank_system.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }

    const char* sqlCreateTable = "CREATE TABLE IF NOT EXISTS accounts ("
                                 "account_number INTEGER PRIMARY KEY,"
                                 "name TEXT NOT NULL,"
                                 "balance REAL NOT NULL);";
    char* errMsg = 0;
    rc = sqlite3_exec(db, sqlCreateTable, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Table created successfully" << std::endl;
    }
    
    int choice;
    double amount;
    std::string name;    
    int accountNumber;
    BankAccount* account = nullptr;
    
    do{
        std::cout << "1. Create Account" << std::endl;
        std::cout << "2. Deposit" << std::endl;
        std::cout << "3. Withdraw" << std::endl;
        std::cout << "4. Display Account" << std::endl;
        std::cout << "5. Delete Account" << std::endl;
        std::cout << "6. View All Accounts" << std::endl;
        std::cout << "7. Search Account" << std::endl;
        std::cout << "8. Transfer Funds" << std::endl;
        std::cout << "9. Update Account Name" << std::endl;
        std::cout << "10. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        choice = getValidatedChoice(1, 10);
        
        switch(choice)
        {
            case 1:
                 std::cout << "Enter account number: ";
                accountNumber = getValidatedChoice(1, std::numeric_limits<int>::max());
                std::cout << "Enter account name: ";
                std::cin >> name;
                std::cout << "Enter initial balance: ";
                amount = getValidatedAmount();
                account = new BankAccount(accountNumber, name, amount);
                saveAccountToDatabase(*account, db);
                break;
            case 2:
                if (account) {
                    std::cout << "Enter deposit amount: ";
                    amount = getValidatedAmount();
                    account->deposit(amount);
                    account->display();
                    updateBalanceInDatabase(*account, db);
                }else{
                    std::cout << "No Account selected" << std::endl;
                }
                break;
            case 3:
                if(account){
                    std::cout << "Enter witdraw amount: ";
                    amount = getValidatedAmount();
                    account->withdraw(amount);
                    account->display();
                    updateBalanceInDatabase(*account, db);       
                }else{
                    std::cout << "No Account selected" << std::endl;
                }
                break;
            case 4:
                if(account){
                    account->display();
                }else{
                    std::cout << "No Account selected" << std::endl;
                }
                break;
            case 5:
                if(account){
                    deleteAccountFromDatabase(account->getAccountNumber(), db);
                    delete account;
                    account = nullptr;
                }else{
                    std::cout << "No Account selected" << std::endl;
                }
                break;
            case 6:
                viewAllAccounts(db);
                break;
            case 7:
                std::cout << "Enter account number: ";
                accountNumber = getValidatedChoice(1, std::numeric_limits<int>::max());
                delete account;
                account = searchAccount(accountNumber, db);
                break;
            case 8: {
                int toAccountNumber;
                std::cout << "Enter the account number to transfer to: ";
                toAccountNumber = getValidatedChoice(1, std::numeric_limits<int>::max());
                BankAccount* toAccount = searchAccount(toAccountNumber, db);
                if (toAccount) {
                    std::cout << "Enter amount to transfer: ";
                    amount = getValidatedAmount();
                    transferFunds(account, toAccount, amount, db);
                    delete toAccount;
                } else {
                    std::cout << "Destination account not found" << std::endl;
                }
                break;
            }
            case 9:
                if (account) {
                    std::string newName;
                    std::cout << "Enter new account name: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                    std::getline(std::cin, newName); // Use getline to allow spaces in the account name
                    updateAccountNameInDatabase(account->getAccountNumber(), newName, db);
                    account->setName(newName);
                    account->display();
                } else {
                    std::cout << "No account selected" << std::endl;
                }
                break;
            case 10:
                std::cout << "Goodbye" << std::endl;
                break;
            default:
                std::cout << "Invalid choice" << std::endl;
        }
    }while(choice != 10);

    delete account;
    sqlite3_close(db);
    
    return 0;
}
