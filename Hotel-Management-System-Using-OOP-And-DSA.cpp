/*
    HOTEL MANAGEMENT SYSTEM
    -----------------------
    Features:
    - Add and manage customers and rooms
    - Assign rooms and handle checkout
    - Persistent storage using text files
    - Sorting and searching for efficient access

    Author: <Your Name>
*/

#include <bits/stdc++.h>
using namespace std;

/*-------------------------------------------------
  Abstract Base Class: Person
--------------------------------------------------*/
class Person {
public:
    string name;
    int age;
    string phn_no;

    // Displays information of a person (implemented in derived classes)
    virtual void display_info() const = 0;

    virtual ~Person() {}
};

/*-------------------------------------------------
  Class: Customer
--------------------------------------------------*/
class Customer : public Person {
public:
    int customer_id;
    int room_id;

    Customer() {
        room_id = -1; // -1 indicates no room assigned
    }

    Customer(string name, int age, string phn, int id) {
        this->name = name;
        this->age = age;
        this->phn_no = phn;
        customer_id = id;
        room_id = -1;
    }

    // Displays customer details
    void display_info() const override {
        cout << "ID: " << customer_id
             << " | Name: " << name
             << " | Age: " << age
             << " | Phone: " << phn_no
             << " | Room: ";

        if (room_id == -1) cout << "None\n";
        else cout << room_id << "\n";
    }

    string getName() const { return name; }
    int getAge() const { return age; }
    string getPhone() const { return phn_no; }
};

/*-------------------------------------------------
  Class: Room
--------------------------------------------------*/
class Room {
private:
    double price;

public:
    int Room_id;
    string type;
    int status;   // -1 = Available, 0 = Occupied

    Room() {
        status = -1;
    }

    Room(int id, string t, double p) {
        Room_id = id;
        type = t;
        price = p;
        status = -1;
    }

    void setPrice(double p) { price = p; }
    double getPrice() const { return price; }

    // Displays room details
    void display() const {
        cout << "Room ID: " << Room_id
             << " | Type: " << type
             << " | Price: " << price
             << " | Status: "
             << (status == -1 ? "Available" : "Occupied") << "\n";
    }
};

/*-------------------------------------------------
  Sorting Helpers
--------------------------------------------------*/
bool compareCustomers(const Customer &a, const Customer &b) {
    return a.customer_id < b.customer_id;
}

bool compareRooms(const Room &a, const Room &b) {
    if (a.type == b.type)
        return a.Room_id < b.Room_id;
    return a.type < b.type;
}

/*-------------------------------------------------
  Class: Hotel
  Handles all business logic
--------------------------------------------------*/
class Hotel {
public:
    vector<Customer> customers;
    vector<Room> rooms;

    // Sort customer list by ID
    void sort_customers() {
        if (!customers.empty())
            sort(customers.begin(), customers.end(), compareCustomers);
    }

    // Sort rooms by type then ID
    void sort_rooms() {
        if (!rooms.empty())
            sort(rooms.begin(), rooms.end(), compareRooms);
    }

    // Load rooms from file
    void load_rooms() {
        ifstream fin("rooms.txt");
        if (!fin) {
            cout << "rooms.txt not found. Starting fresh.\n";
            return;
        }

        rooms.clear();
        int id, status;
        string type;
        double price;

        while (fin >> id >> type >> price >> status) {
            Room r(id, type, price);
            r.status = status;
            rooms.push_back(r);
        }

        fin.close();
        sort_rooms();
    }

    // Load customers from file
    void load_customers() {
        ifstream fin("customers.txt");
        if (!fin) {
            cout << "customers.txt not found. Starting fresh.\n";
            return;
        }

        customers.clear();
        int id, age, room_id;
        string name, ph;

        while (fin >> id >> name >> age >> ph >> room_id) {
            Customer c(name, age, ph, id);
            c.room_id = room_id;
            customers.push_back(c);
        }

        fin.close();
        sort_customers();
    }

    // Save rooms to file
    void save_rooms() {
        ofstream fout("rooms.txt");
        for (auto &r : rooms)
            fout << r.Room_id << " " << r.type << " "
                 << r.getPrice() << " " << r.status << "\n";
    }

    // Save customers to file
    void save_customers() {
        ofstream fout("customers.txt");
        for (auto &c : customers)
            fout << c.customer_id << " " << c.getName() << " "
                 << c.getAge() << " " << c.getPhone() << " "
                 << c.room_id << "\n";
    }

    // Add a new customer
    void add_customer() {
        int id, age;
        string name, ph;

        cout << "Enter ID: "; cin >> id;
        cout << "Enter Name: "; cin >> name;
        cout << "Enter Age: "; cin >> age;
        cout << "Enter Phone: "; cin >> ph;

        for (const auto &c : customers)
            if (c.customer_id == id) {
                cout << "Duplicate ID not allowed.\n";
                return;
            }

        customers.push_back(Customer(name, age, ph, id));
        sort_customers();
        save_customers();
        cout << "Customer added successfully.\n";
    }

    // Add a new room
    void add_room() {
        int id;
        string type;
        double price;

        cout << "Enter Room ID: "; cin >> id;
        cout << "Enter Room Type: "; cin >> type;
        cout << "Enter Price: "; cin >> price;

        for (const auto &r : rooms)
            if (r.Room_id == id && r.type == type) {
                cout << "Duplicate room.\n";
                return;
            }

        rooms.push_back(Room(id, type, price));
        sort_rooms();
        save_rooms();
        cout << "Room added successfully.\n";
    }

    // Binary search for customer by ID
    int find_customer(int id) const {
        int low = 0, high = customers.size() - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (customers[mid].customer_id == id) return mid;
            else if (customers[mid].customer_id < id) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }

    // Find first available room of given type
    int find_available_room(const string &type) const {
        for (int i = 0; i < rooms.size(); i++)
            if (rooms[i].type == type && rooms[i].status == -1)
                return i;
        return -1;
    }

    // Assign room to customer
    void assign_room() {
        int cid;
        string type;

        cout << "Enter Customer ID: "; cin >> cid;
        cout << "Enter Room Type: "; cin >> type;

        int cidx = find_customer(cid);
        if (cidx == -1 || customers[cidx].room_id != -1) {
            cout << "Invalid operation.\n";
            return;
        }

        int ridx = find_available_room(type);
        if (ridx == -1) {
            cout << "No available room.\n";
            return;
        }

        customers[cidx].room_id = rooms[ridx].Room_id;
        rooms[ridx].status = 0;

        save_customers();
        save_rooms();
        cout << "Room assigned successfully.\n";
    }

    // Checkout customer
    void checkout() {
        int cid;
        cout << "Enter Customer ID: "; cin >> cid;

        int cidx = find_customer(cid);
        if (cidx == -1 || customers[cidx].room_id == -1) {
            cout << "Invalid operation.\n";
            return;
        }

        for (auto &r : rooms)
            if (r.Room_id == customers[cidx].room_id)
                r.status = -1;

        customers[cidx].room_id = -1;
        save_customers();
        save_rooms();
        cout << "Checkout successful.\n";
    }

    // Display all data
    void display_all() const {
        cout << "\n--- Customers ---\n";
        for (const auto &c : customers) c.display_info();

        cout << "\n--- Rooms ---\n";
        for (const auto &r : rooms) r.display();
    }
};

/*-------------------------------------------------
  Main Program
--------------------------------------------------*/
int main() {
    Hotel h;
    h.load_rooms();
    h.load_customers();

    int choice;
    do {
        cout << "\n--- HOTEL MANAGEMENT SYSTEM ---\n";
        cout << "1. Add Customer\n2. Add Room\n3. Assign Room\n4. Checkout\n5. Display All\n0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: h.add_customer(); break;
        case 2: h.add_room(); break;
        case 3: h.assign_room(); break;
        case 4: h.checkout(); break;
        case 5: h.display_all(); break;
        case 0: cout << "Exiting...\n"; break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
