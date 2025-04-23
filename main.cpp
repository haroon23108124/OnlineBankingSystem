#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>

using namespace std;

class User {
private:
    string username;
    string password;
    float balance;

public:
    User() : username(""), password(""), balance(0.0) {}
    User(const string& uname, const string& pwd, float bal) : username(uname), password(pwd), balance(bal) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    float getBalance() const { return balance; }

    void deposit(float amount) {
        balance += amount;
        updateBalanceInFile("bankdata.txt");
    }

    bool withdraw(float amount) {
        if (balance >= amount) {
            balance -= amount;
            updateBalanceInFile("bankdata.txt");
            return true;
        }
        return false;
    }

    void displayBalance() const {
        cout << "Current balance: " << balance << endl;
    }

    void updateBalanceInFile(const string& filename) {
        ifstream inFile(filename);
        ofstream tempFile("temp.txt");

        string uname, pwd;
        float bal;

        while (inFile >> uname >> pwd >> bal) {
            if (uname == username && pwd == password) {
                tempFile << uname << " " << pwd << " " << balance << endl;
            } else {
                tempFile << uname << " " << pwd << " " << bal << endl;
            }
        }

        inFile.close();
        tempFile.close();

        remove(filename.c_str());
        rename("temp.txt", filename.c_str());
    }
};

class Bank {
private:
    string filename;
    User users[100]; 
    int userCount;  
    stack<string> transactionHistory;
    queue<pair<string, float>> undoQueue;

public:
    Bank(const string& fname) : filename(fname), userCount(0) {
        loadUsersFromFile();
    }

    bool isAdminLogin(const string& username, const string& password) const {
        return username == "admin" && password == "admin123";
    }

    void createUser() {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        for (int i = 0; i < userCount; i++) {
            if (users[i].getUsername() == username) {
                cout << "Username already exists." << endl;
                return;
            }
        }

        users[userCount++] = User(username, password, 0.0); 
        saveUsersToFile();
        cout << "Account created successfully!\n";
    }

    bool loginUser(const string& username, const string& password) const {
        for (int i = 0; i < userCount; i++) {
            if (users[i].getUsername() == username && users[i].getPassword() == password) {
                return true;
            }
        }
        return false;
    }

    User* findUser(const string& username, const string& password) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].getUsername() == username && users[i].getPassword() == password) {
                return &users[i];
            }
        }
        return nullptr;
    }

    void searchAccount() {
        string username;
        cout << "Enter username to search: ";
        cin >> username;

        for (int i = 0; i < userCount; i++) {
            if (users[i].getUsername() == username) {
                cout << "Account found:\n";
                cout << "Username: " << users[i].getUsername() << endl;
                cout << "Balance: " << users[i].getBalance() << endl;
                return;
            }
        }

        cout << "Account not found." << endl;
    }

   
    void displaySortedBalances() {
        pair<string, float> balances[100];  


        for (int i = 0; i < userCount; i++) {
            balances[i] = {users[i].getUsername(), users[i].getBalance()};
        }


        for (int i = 0; i < userCount - 1; i++) {
            for (int j = 0; j < userCount - 1 - i; j++) {
                if (balances[j].second > balances[j + 1].second) {
                    swap(balances[j], balances[j + 1]);
                }
            }
        }

        cout << "\nAccount Balances (Sorted):\n";
        if (userCount == 0) {
            cout << "No accounts found.\n";
        } else {
            for (int i = 0; i < userCount; i++) {
                cout << "Username: " << balances[i].first << ", Balance: " << balances[i].second << endl;
            }
        }
    }

    void showMenu(bool isAdmin) {
        int choice;
        string username, password;

        do {
            if (isAdmin) {
                cout << "\nAdmin Menu:\n";
                cout << "1. Search for a specific account\n";
                cout << "2. Display sorted balances\n"; 
                cout << "3. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice) {
                    case 1: searchAccount(); break;
                    case 2: displaySortedBalances(); break; 
                    case 3: return;
                    default: cout << "Invalid choice.\n";
                }
            } else {
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;

                User* user = findUser(username, password);

                if (user) {
                    cout << "\nUser Menu:\n";
                    cout << "1. Cash Withdraw\n2. Online Transfer\n3. Mobile Packages\n4. Government Fee or Tax\n5. Cash Deposit\n6. View Transaction History\n7. Undo Transaction\n8. Display Balance\n9. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    switch (choice) {
                         case 1: {
                            float amount;
                            cout << "Enter amount to withdraw: ";
                            cin >> amount;
                            if (user->withdraw(amount)) {
                                transactionHistory.push("Withdrawal: $" + to_string(amount));
                                undoQueue.push(make_pair("Withdrawal", amount));
                                cout << "Withdrawal successful!" << endl;
                                
                            } else {
                                cout << "Insufficient balance!" << endl;
                            }
                            user->displayBalance();
                            break;
                        }
                        
                        case 2: { 
                            string recipientUsername;
                            float amount;
                            cout << "Enter recipient's username: ";
                            cin >> recipientUsername;
                            cout << "Enter amount to transfer: ";
                            cin >> amount;

                            User* recipient = nullptr;
                            for (int i = 0; i < userCount; i++) {
                                if (users[i].getUsername() == recipientUsername) {
                                    recipient = &users[i];
                                    break;
                                }
                            }

                            if (recipient) {
                                if (user->withdraw(amount)) {
                                    recipient->deposit(amount);
                                    transactionHistory.push("Transfer to " + recipientUsername + ": $" + to_string(amount));
                                    undoQueue.push(make_pair("Transfer Out", amount));
                                    cout << "Transfer successful!" << endl;
                                } else {
                                    cout << "Insufficient balance for transfer!" << endl;
                                }
                            } else {
                                cout << "Recipient username not found!" << endl;
                            }
                            user->displayBalance();
                            break;
                        }
                        case 3: { 
                            cout << "Available Mobile Packages:\n";
                            cout << "1. Package A: $10 (1GB Data)\n";
                            cout << "2. Package B: $20 (3GB Data)\n";
                            cout << "3. Package C: $30 (5GB Data)\n";
                            int packageChoice;
                            cout << "Enter package choice: ";
                            cin >> packageChoice;

                            float packageCost = 0;
                            switch (packageChoice) {
                                case 1: packageCost = 10; break;
                                case 2: packageCost = 20; break;
                                case 3: packageCost = 30; break;
                                default: cout << "Invalid package choice.\n"; break;
                            }

                            if (packageCost > 0) {
                                if (user->withdraw(packageCost)) {
                                    transactionHistory.push("Purchased Mobile Package: $" + to_string(packageCost));
                                    undoQueue.push(make_pair("Mobile Package", packageCost));
                                    cout << "Package purchased successfully!" << endl;
                                } else {
                                    cout << "Insufficient balance to purchase package." << endl;
                                }
                            }
                            user->displayBalance();
                            break;
                        }
case 4: {
                        float fee;
                        cout << "Enter government fee or tax amount: ";
                        cin >> fee;
                        if (user->withdraw(fee)) {
                            transactionHistory.push("Government Fee/Tax: $" + to_string(fee));
                            undoQueue.push(make_pair("Fee/Tax", fee)); 
                            cout << "Government fee or tax paid successfully!" << endl;
                        } else {
                            cout << "Insufficient balance to pay government fee or tax." << endl;
                        }
                        break;
                    }
                        
                        case 5: {
                            float amount;
                            cout << "Enter amount to deposit: ";
                            cin >> amount;
                            user->deposit(amount);
                            transactionHistory.push("Deposit: $" + to_string(amount));
                            undoQueue.push(make_pair("Deposit", amount));
                            cout << "Deposit successful!" << endl;
                            user->displayBalance();
                            break;
                        }
                        case 6: {
                            stack<string> tempStack = transactionHistory;
                            cout << "Transaction History:\n";
                            if (tempStack.empty()) {
                                cout << "No transactions yet.\n";
                            } else {
                                while (!tempStack.empty()) {
                                    cout << tempStack.top() << endl;
                                    tempStack.pop();
                                }
                            }
                            
                            break;
                        }
                        case 7: {
                            if (!undoQueue.empty()) {
                                pair<string, float> lastTransaction = undoQueue.front();
                                undoQueue.pop();

                                if (lastTransaction.first == "Withdrawal") {
                                    user->deposit(lastTransaction.second);
                                    transactionHistory.push("Undo Withdrawal: $" + to_string(lastTransaction.second));
                                    cout << "Withdrawal undone successfully!" << endl;
                                } else if (lastTransaction.first == "Deposit") {
                                    user->withdraw(lastTransaction.second);
                                    transactionHistory.push("Undo Deposit: $" + to_string(lastTransaction.second));
                                    cout << "Deposit undone successfully!" << endl;
                                }
                            } else {
                                cout << "No transactions to undo." << endl;
                            }
                            
                            
                            break;
                        }
                        case 8: {user->displayBalance(); break;}
                        case 9: return;
                        default: cout << "Invalid choice.\n";
                    }
                } else {
                    cout << "Invalid username or password.\n";
                    choice = 0;
                }
                
                
            }
             
        } while (choice !=0);
    }

    void loadUsersFromFile() {
        ifstream inFile(filename.c_str());
        if (!inFile.is_open()) return;

        string uname, pwd;
        float balance;
        while (inFile >> uname >> pwd >> balance) {
            if (userCount < 100) {
                users[userCount++] = User(uname, pwd, balance);
            }
        }
        inFile.close();
    }

    void saveUsersToFile() {
        ofstream file(filename.c_str());
        for (int i = 0; i < userCount; i++) {
            file << users[i].getUsername() << " " << users[i].getPassword() << " " << users[i].getBalance() << endl;
        }
        file.close();
    }
};

int main() {
    Bank bank("bankdata.txt");
    int option;

    do {
        cout << "\nWelcome to Online Banking App!\n";
        cout << "1. Login\n2. Create New Account\n3. Admin Login\n4. Exit\n";
        cout << "Enter your choice: ";
        cin >> option;

        switch (option) {
            case 1: bank.showMenu(false); break;
            case 2: bank.createUser(); break;
            case 3: {
                string adminUsername, adminPassword;
                cout << "Enter admin username: ";
                cin >> adminUsername;
                cout << "Enter admin password: ";
                cin >> adminPassword;
                if (bank.isAdminLogin(adminUsername, adminPassword)) {
                    bank.showMenu(true);
                } else {
                    cout << "Invalid admin credentials.\n";
                }
                break;
            }
            case 4: cout << "Exiting...\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (option != 4);

    return 0;
}