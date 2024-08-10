#include <iostream> // For input-output stream
#include <map> // For map container
#include <unordered_map> // For unordered_map container
#include <limits> // For numeric limits
#include <queue> // For queue container
#include <iomanip> // For I/O manipulators (e.g., setw)
#include <vector> // For vector container
#include <stack> // For stack container
#include <fstream> // For file stream
#include <algorithm> // For standard algorithms (e.g., find)
#include <string> // For string class

using namespace std; // Using standard namespace

// Class representing a User
class User {
public:
    string username; // Username of the user
    string contact; // Contact number of the user
    string password; // Password of the user

    User() {};

    // Function to save user data to a file
    void save(ofstream& out) const {
        out << username << endl << contact << endl << password << endl; // Write username, contact, and password to the file
    }

    // Function to load user data from a file
    void load(ifstream& in) {
        getline(in, username); // Read username from the file
        getline(in, contact); // Read contact from the file
        getline(in, password); // Read password from the file
    }
};

// Function to load users from a file
unordered_map<string, User> loadUsers() {
    unordered_map<string, User> users; // Map to store users
    ifstream in("users.txt"); // Open the users.txt file
    if (in.is_open()) { // Check if file is open
        while (!in.eof()) { // Loop until end of file
            User user; // Create a User object
            user.load(in); // Load user data from file
            if (user.username.empty()) break; // Avoid adding empty users
            users[user.username] = user; // Add user to the map
        }
        in.close(); // Close the file
    }
    return users; // Return the map of users
}

// Function to save users to a file
void saveUsers(unordered_map<string, User> users) {
    ofstream out("users.txt"); // Open the users.txt file
    for (const auto & user : users) {
        User currentUser = user.second; // Get the current user object
        currentUser.saveToFile(out); // Save the current user's data to the file
    }
    out.close(); // Close the file
}

// Class representing an admin
class admin {
private:
    string const username = "admin"; // Admin username
    string const password = "admin"; // Admin password
public:
    // Function to get the admin username
    string getUsername() const {
        return username; // Return the admin username
    }

    // Function to get the admin password
    string getPassword() const {
        return password; // Return the admin password
    }
};
// Class representing table management in a restaurant
class Table {
private:
    // Map to store table capacities and their counts
    map<int, int> tableCount = {
        {2, 3}, // 3 tables for 2 people
        {4, 4}, // 4 tables for 4 people
        {6, 2}  // 2 tables for 6 people
    };

    // Structure to store order details in a linked list
    struct OrderNode {
        int itemId; // ID of the item
        int count; // Count of the item
        OrderNode* next; // Pointer to the next order
        OrderNode(int id, int cnt) : itemId(id), count(cnt), next(nullptr) {} // Constructor
    };


    // Unordered map to store order history for each table
    unordered_map<int, OrderNode*> orderHistory;
    // Stack to store actions for undo functionality
    stack<Action> undoStack;
    // Stack to store actions for redo functionality
    stack<Action> redoStack;

    // Structure to store actions for undo/redo functionality
    struct Action {
        int tableId; // ID of the table
        string actionType; // Action type: "allocate" or "deallocate"
        int people; // Number of people involved in the action
    };


public:
    // Unordered map to store table status (availability and number of people)
    unordered_map<int, pair<bool, int>> tableStatus = {
        {21, {true, 0}}, // Table 21: available and 0 people
        {22, {true, 0}}, 
        {23, {true, 0}},
        {41, {true, 0}}, 
        {42, {true, 0}}, 
        {43, {true, 0}}, 
        {44, {true, 0}}, 
        {61, {true, 0}}, 
        {62, {true, 0}}, 
    };

    // Queues to manage waiting lists for tables with different capacities
    queue<int> waiting2; // Queue for 2-person tables
    queue<int> waiting4; // Queue for 4-person tables
    queue<int> waiting6; // Queue for 6-person tables

    // Vector to store allocated tables
    vector<int> allocatedTables;

    // Function to allocate a table
    void allocation() {
        int people, tableId = 0;
        cout << "Enter number of people: ";
        cin >> people; // Get the number of people
        if (people > 6) { // If number of people is more than 6
            cout << "Sorry we don't have that capacity" << endl;
            system("pause"); // Pause the system
            return;
        }

        // Find an available table for the given number of people
        
        for (auto iter : tableCount) {               // Iterate over tableCount map entries
            
            if (iter.first >= people && iter.second) {       // If table size fits and is available
                // Generate unique table ID
                tableId = iter.first * 10 + iter.second;
                // Decrement available table count
                tableCount[iter.first]--;
                // Mark table as allocated and set number of people
                tableStatus[tableId].first = false;
                tableStatus[tableId].second = people;
                // Add table ID to allocated tables list
                allocatedTables.push_back(tableId);
                // Push allocation action to undo stack
                undoStack.push ({ tableId, "allocate", people}); // Store the number of people
                // Clear redo stack
                while (!redoStack.empty()) redoStack.pop();
                // Break out of loop
                break;
            }
        }
        

        // If a table is allocated
        if (tableId != 0) {
            cout << "You have been allocated table id: " << tableId << endl;
        }
        else { // If no table is available, add to waiting list
            int custId;
            cout << "Tables are full, you are in waiting!" << endl;
            cout << "Enter your ID: ";
            cin >> custId; // Get customer ID
            if (people <= 2) waiting2.push(custId);
            else if (people <= 4) waiting4.push(custId);
            else if (people <= 6) waiting6.push(custId);
            cout << "Thank you, just wait for a while!" << endl;
        }

        system("pause"); // Pause the system
        return;
    }

    // Function to check if a table ID is valid and allocated
    bool checkValidtion(int tableno) {
        if (tableStatus.find(tableno) == tableStatus.end()) // Check if table ID exists
            return false;
        else if (tableStatus[tableno].first == true) // Check if table is allocated
            return false;
        return true; // Table is valid and allocated
    }

    // Function to deallocate a table
    void deallocate(int tableId) {
        // Check if table ID is valid
        if (checkValidtion(tableId)) {
            // Increment the count of available tables of this size
            tableCount[tableId / 10]++;
            // Mark table as deallocated and reset number of people
            tableStatus[tableId].first = true;
            tableStatus[tableId].second = 0;

            // Remove the table ID from the allocated tables list
            auto it = std:: find(allocatedTables.begin(), allocatedTables.end(), tableId);
            if (it != allocatedTables.end()) allocatedTables.erase(it);

            // Push deallocation action to undo stack
            undoStack.push({ tableId, "deallocate" });
    

            while (!redoStack.empty()) redoStack.pop(); // Clear redo stack

            cout << "Table " << tableId << " has been deallocated." << endl;
        }
        else {
            cout << "Invalid table ID or table is not allocated." << endl;
        }
        system("pause"); // Pause the system
    }
    
    void undo() {
        if (!undoStack.empty()) {
            Action lastAction = undoStack.top();
            undoStack.pop();
            redoStack.push(lastAction);

            if (lastAction.actionType == "allocate") {
                deallocate (lastAction.tableId); // Deallocate the table
            }
            else if (lastAction.actionType == "deallocate") {
                allocation(lastAction.people); // Allocate the table with the previous number of people
            }
            // Print message indicating successful undo
            cout << "Undo successfully performed." << endl;
        }
        else {
            cout << "No actions to undo." << endl;
        }
    }

    // Function to redo the last undone action
    void redo() {
        if (!redoStack.empty()) {
            Action lastAction = redoStack.top();
            redoStack.pop();
            undoStack.push(lastAction);

            if (lastAction.actionType == "deallocate") {
                deallocate(lastAction.tableId); // Deallocate the table
            }
            else if (lastAction.actionType == "allocate") {
                allocation(lastAction.people); // Allocate the table with the previous number of people
            }
            cout << "Redo successfully performed." << endl;
        }
        else {
            cout << "No actions to redo." << endl;
        }
    }

    // Function to display all allocated tables
    void displayAllocatedTables() {
        cout << "Allocated Tables: ";
        for (int table : allocatedTables) {
            cout << table << " ";
        }
        cout << endl;
    }

    // Function to add an order to a table's order history
    void addOrder(int tableId, int itemId, int count) {
        OrderNode* newNode = new OrderNode(itemId, count); // Create new order node
        newNode->next = orderHistory[tableId]; // Link the new node to the existing orders
        orderHistory[tableId] = newNode; // Update order history
    }

    // Function to print the order history for a table
    void printOrderHistory(int tableId) {
        OrderNode* current = orderHistory[tableId]; // Get the head of the order list
        while (current) { // Traverse the order list
            cout << "Item ID: " << current->itemId << ", Count: " << current->count << endl;
            current = current->next; // Move to the next order
        }
    }

    // Function to clear the order history for a table
    void clearOrderHistory(int tableId) {
        OrderNode* current = orderHistory[tableId]; // Get the head of the order list
        while (current) { // Traverse the order list
            OrderNode* toDelete = current; // Node to be deleted
            current = current->next; // Move to the next order
            delete toDelete; // Delete the current node
        }
        orderHistory[tableId] = nullptr; // Reset the order history
    }
};
class OrderingAndBilling : public Table {
public:
    User* currentUser; // Pointer to the current user
    unordered_map<int, unordered_map<int, int>> viewBill; // Map to store the bill details for each table
    //Outer map ka key table ID hai aur inner map ka key-value pairs item ID aur uski quantity hai.
    // Menu map with item ID, item name, and item price
    map<int, pair<string, int>> menu{
        {1, {"Spaghetti", 250}},
        {2, {"Fried fish", 280}},
        {3, {"Triffle", 350}},
        {4, {"Pizza small", 550}},
        {5, {"Burger", 260}},
        {6, {"Chicken biryani", 300}},
        {7, {"Fries", 80}},
        {8, {"Naan", 18}},
        {9, {"Curry", 210}},
        {10, {"Nuggets", 250}},
        {11, {"Chai", 50}},
        {12, {"Coffee", 250}}
    };

    // Function to order food
    void orderfood() {
        int tableId;
        cout << "Enter table id: ";
        cin >> tableId; // Get the table ID
        cout << "-----------MENU-----------" << endl;
        cout << left << setw(5) << "S.No" << setw(30) << "Item" << setw(10) << "Price" << endl;
        for (auto iter : menu) { // Display the menu
            cout << left << setw(5) << iter.first << setw(30) << iter.second.first << setw(10) << iter.second.second << endl;
        }

        int sNo, count;
        cout << "Select the Serial number to order: ";
        cin >> sNo; // Get the serial number of the item
        cout << "Enter the count of item you want: ";
        cin >> count; // Get the count of the item

        if (menu.find(sNo) != menu.end()) { // If item exists in the menu
            viewBill[tableId][sNo] += count; // Add the item to the bill
            addOrder(tableId, sNo, count); // Add to order history
            saveOrderHistory(); // Save the order history immediately after adding an order
        }
        else { // If item does not exist in the menu
            cout << "Invalid selection!" << endl;
        }

        system("pause"); // Pause the system
    }

    // Function to generate the bill for a table
    void generateBill(int tableId) {
        int totalBill = 0;
        cout << "Table ID: " << tableId << endl;
        cout << left << setw(5) << "S.No" << setw(30) << "Item" << setw(10) << "Price" << setw(10) << "Count" << setw(20) << "Total" << endl;
        for (auto iter : viewBill[tableId]) { // Iterate through the bill items
            cout << left << setw(5) << iter.first << setw(30) << menu[iter.first].first << setw(10) << menu[iter.first].second << setw(5) << iter.second << setw(10) << menu[iter.first].second * iter.second << endl;
            totalBill += menu[iter.first].second * iter.second; // Calculate the total bill
        }
        cout << "TOTAL BILL: " << totalBill << endl;

        saveOrderHistory(); // Save the order history after generating bill
    }

    // Function to view the bill for a table
    void viewTableBill(int tableId) {
        cout << "Bill for table " << tableId << ":" << endl;
        if (viewBill.find(tableId) != viewBill.end()) { // If table has orders
            generateBill(tableId); // Generate the bill
        }
        else { // If no orders found for the table
            cout << "No orders found for this table." << endl;
        }
    }

    // Function to save the order history to a file
    void saveOrderHistory() {
        if (!currentUser) return; // If no current user, return

        ofstream out(currentUser->username + "_history.txt"); // Open the file with the user's name
        for (auto& tableOrder : viewBill) { // Iterate through each table's order details
            for (auto& order : tableOrder.second) { // Iterate through each item's count in the order details
                out << tableOrder.first << " " << order.first << " " << order.second << endl; // Save table ID, item ID, and count
            }
        }
        out.close(); // Close the file
    }

    // Function to load the order history from a file
   for (auto& tableOrder : viewBill) { // Iterate through each table's order details
       for (auto& order : tableOrder.second) { // Har item ka count iterate karo
           out << tableOrder.first << " " << order.first << " " << order.second << endl; // Table ID, item ID, aur count save karo
       }
   }
   out.close(); // File ko close kar do
}
void loadOrderHistory(const string& username) {
    ifstream in(username + "_history.txt"); // File open karo user ke naam se
    if (!in.is_open()) return; // Agar file nahi khul rahi to return kar do

    int tableId, itemId, count;
    while (in >> tableId >> itemId >> count) { // File se table ID, item ID, aur count read karo
        viewBill[tableId][itemId] += count; // `viewBill` mein count ko update karo
    }
    in.close(); // File ko close kar do

}

    // Function to print order history for the current user
    void printOrderHistory() {
        if (!currentUser) return; // If no current user, return

        loadOrderHistory(currentUser->username); // Load order history for the current user
        cout << "Order history for " << currentUser->username << ":" << endl;
        for (auto& tableOrder : viewBill) { // Iterate through each table's order details
            cout << "Table " << tableOrder.first << ":" << endl;
            for (auto& order : tableOrder.second) { // Iterate through each item's count in the order details
                cout << "  Item " << order.first << ": " << order.second << " times" << endl; // Print item and its count
            }
        }
    }
};

// Global variable to store users loaded from file
unordered_map<string, User> users = loadUsers();
// Pointer to the current logged-in user
User* currentUser = nullptr;

// Function to register a new user
void registerUser() {
    string username, contact, password;
    cout << "Enter username: ";
    cin >> username; // Get username
    cout << "Enter contact number: ";
    cin >> contact; // Get contact number
    cout << "Enter password: ";
    cin >> password; // Get password

    users[username] = User(username, contact, password); // Create a new user and add to the users map
    saveUsers(users); // Save the users to the file
    cout << "Registration successful!" << endl; // Registration success message
}

// Function to login a user
bool loginUser() {
    string username, password;
    cout << "Enter username: ";
    cin >> username; // Get username
    cout << "Enter password: ";
    cin >> password; // Get password

    auto it = users.find(username); // Find user in the users map
    if (it != users.end() && it->second.password == password) { // Check if user exists and password matches
        currentUser = &it->second; // Set the current user
        return true; // Login successful
    }
    return false; // Login failed
}
int main() {
    admin a; // Create an instance of the admin class
    Table t; // Create an instance of the Table class
    OrderingAndBilling OB; // Create an instance of the OrderingAndBilling class

    int loginOption = 0; // Variable to store user's login choice

    // Loop until user chooses to exit
    do {
        // Display login options
        cout << "===============================" << endl;
        cout << "1) Admin" << endl;
        cout << "2) User" << endl;
        cout << "3) Register" << endl;
        cout << "4) Exit" << endl;
        cout << "===============================" << endl;
        cin >> loginOption; // Get user's choice

        system("pause"); // Pause the system

        // Perform actions based on user's choice
        switch (loginOption) {
        case 1: { // Admin login
            string username;
            string password;
            cout << "Enter username: ";
            cin >> username; // Get admin's username
            cout << "Enter password: ";
            cin >> password; // Get admin's password
            if (username == a.getUsername() && password == a.getPassword()) { // If username and password match
                int adminOption;
                do {
                    // Display admin options
                    cout << "===============================" << endl;
                    cout << "1) Table allocation" << endl;
                    cout << "2) Table deallocation" << endl;
                    cout << "3) Undo" << endl;
                    cout << "4) Redo" << endl;
                    cout << "5) View Registered Users" << endl;
                    cout << "6) View allocated tables" << endl;
                    cout << "7) Exit" << endl;
                    cout << "===============================" << endl;
                    cin >> adminOption;
                    switch (adminOption) {
                    case 1:
                        t.allocation(); // Allocate table
                        break;
                    case 2: {
                        int tableId;
                        cout << "Enter table id: ";
                        cin >> tableId; // Get table ID for deallocation
                        t.deallocate(tableId); // Deallocate table
                        break;
                    }
                    case 3:
                        t.undo(); // Undo last action
                        break;
                    case 4:
                        t.redo(); // Redo last undone action
                        break;
                    case 5:
                        // Display registered users
                        for (const auto& user : users) {
                            cout << "Username: " << user.first << ", Contact: " << user.second.contact << endl;
                        }
                        system("pause");
                        break;
                    case 6:
                        t.displayAllocatedTables(); // Display allocated tables
                        system("pause");
                        break;
                    case 7:
                        break;
                    default:
                        cout << "Invalid option" << endl;
                    }
                } while (adminOption != 6); // Continue until admin chooses to exit
            }
            else {
                cout << "Invalid username or password" << endl;
                system("pause");
            }
            break;
        }
        case 2: // User login
            if (loginUser()) { // If login successful
                OB.currentUser = currentUser;  // Set current user for OrderingAndBilling instance
                int userOption;
                do {
                    // Display user options
                    cout << "===============================" << endl;
                    cout << "1) Order Food" << endl;
                    cout << "2) View Table Bill" << endl;
                    cout << "3) View Order History" << endl;
                    cout << "4) Exit" << endl;
                    cout << "===============================" << endl;
                    cin >> userOption;
                    switch (userOption) {
                    case 1:
                        OB.orderfood(); // Order food
                        break;
                    case 2: {
                        int tableId;
                        cout << "Enter table id: ";
                        cin >> tableId; // Get table ID for viewing bill
                        OB.viewTableBill(tableId); // View table bill
                        break;
                    }
                    case 3:
                        OB.printOrderHistory(); // Print order history
                        system("pause");
                        break;
                    case 4:
                        break;
                    default:
                        cout << "Invalid option" << endl;
                    }
                } while (userOption != 4); // Continue until user chooses to exit
            }
            else {
                cout << "Invalid username or password" << endl;
                system("pause");
            }
            break;
        case 3:
            registerUser(); // Register a new user
            break;
        case 4:
            break; // Exit the program
        default:
            cout << "Invalid option" << endl;
        }
    } while (loginOption != 4); // Continue until user chooses to exit

    return 0;
}

