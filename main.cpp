#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// MySQL Connection settings
const char* HOST = "localhost";
const char* USER = "root";
const char* PASS = "root";
const char* DB_NAME = "library_db";

MYSQL* conn;

// Function prototypes
bool connectToDB();
void disconnectFromDB();
void addBook();
void addMember();
void borrowBook();
void returnBook();
void viewBooks();
void viewMembers();
void viewTransactions();

// Book class
class Book {
public:
    int id;
    string title;
    string author;
    string isbn;
    int availableQuantity;

    Book(string title, string author, string isbn, int availableQuantity)
        : title(title), author(author), isbn(isbn), availableQuantity(availableQuantity) {}
};

// Member class
class Member {
public:
    int id;
    string name;
    string address;
    string phone;

    Member(string name, string address, string phone)
        : name(name), address(address), phone(phone) {}
};

// Transaction class
class Transaction {
public:
    int id;
    int memberId;
    int bookId;
    string borrowDate;
    string returnDate;

    Transaction(int memberId, int bookId, string borrowDate, string returnDate)
        : memberId(memberId), bookId(bookId), borrowDate(borrowDate), returnDate(returnDate) {}
};

int main() {
    if (!connectToDB()) {
        cout << "Failed to connect to MySQL." << endl;
        return 1;
    }

    int choice;
    do {
        cout << "\n=== Library Management System ===" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Add Member" << endl;
        cout << "3. Borrow Book" << endl;
        cout << "4. Return Book" << endl;
        cout << "5. View Books" << endl;
        cout << "6. View Members" << endl;
        cout << "7. View Transactions" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                addMember();
                break;
            case 3:
                borrowBook();
                break;
            case 4:
                returnBook();
                break;
            case 5:
                viewBooks();
                break;
            case 6:
                viewMembers();
                break;
            case 7:
                viewTransactions();
                break;
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    disconnectFromDB();

    return 0;
}

bool connectToDB() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        cout << "Error initializing MySQL: " << mysql_error(conn) << endl;
        return false;
    }

    if (!mysql_real_connect(conn, HOST, USER, PASS, DB_NAME, 0, NULL, 0)) {
        cout << "Error connecting to MySQL: " << mysql_error(conn) << endl;
        return false;
    }

    cout << "Connected to MySQL database: " << DB_NAME << endl;
    return true;
}

void disconnectFromDB() {
    mysql_close(conn);
    cout << "Disconnected from MySQL." << endl;
}

void addBook() {
    string title, author, isbn;
    int availableQuantity;

    cout << "Enter book title: ";
    getline(cin, title);
    cout << "Enter author name: ";
    getline(cin, author);
    cout << "Enter ISBN: ";
    getline(cin, isbn);
    cout << "Enter available quantity: ";
    cin >> availableQuantity;

    Book newBook(title, author, isbn, availableQuantity);

    stringstream ss;
    ss << "INSERT INTO books (title, author, isbn, available_quantity) VALUES ('"
       << newBook.title << "', '" << newBook.author << "', '" << newBook.isbn << "', " << newBook.availableQuantity << ")";

    string query = ss.str();

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book added successfully." << endl;
    }
}

void addMember() {
    string name, address, phone;

    cout << "Enter member name: ";
    getline(cin, name);
    cout << "Enter address: ";
    getline(cin, address);
    cout << "Enter phone number: ";
    getline(cin, phone);

    Member newMember(name, address, phone);

    stringstream ss;
    ss << "INSERT INTO members (name, address, phone) VALUES ('"
       << newMember.name << "', '" << newMember.address << "', '" << newMember.phone << "')";

    string query = ss.str();

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding member: " << mysql_error(conn) << endl;
    } else {
        cout << "Member added successfully." << endl;
    }
}

void borrowBook() {
    int memberId, bookId;
    string borrowDate, returnDate;

    cout << "Enter member ID: ";
    cin >> memberId;
    cout << "Enter book ID: ";
    cin >> bookId;
    cin.ignore();
    cout << "Enter borrow date (YYYY-MM-DD): ";
    getline(cin, borrowDate);
    cout << "Enter return date (YYYY-MM-DD): ";
    getline(cin, returnDate);

    Transaction newTransaction(memberId, bookId, borrowDate, returnDate);

    stringstream ss;
    ss << "INSERT INTO transactions (member_id, book_id, borrow_date, return_date) VALUES ("
       << newTransaction.memberId << ", " << newTransaction.bookId << ", '"
       << newTransaction.borrowDate << "', '" << newTransaction.returnDate << "')";

    string query = ss.str();

    if (mysql_query(conn, query.c_str())) {
        cout << "Error borrowing book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book borrowed successfully." << endl;
    }
}

void returnBook() {
    int transactionId;

    cout << "Enter transaction ID: ";
    cin >> transactionId;

    stringstream ss;
    ss << "UPDATE transactions SET return_date = NOW() WHERE id = " << transactionId;

    string query = ss.str();

    if (mysql_query(conn, query.c_str())) {
        cout << "Error returning book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book returned successfully." << endl;
    }
}

void viewBooks() {
    if (mysql_query(conn, "SELECT * FROM books")) {
        cout << "Error fetching books: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    cout << "\n=== Books ===" << endl;
    cout << "ID\tTitle\tAuthor\tISBN\tAvailable Quantity" << endl;
    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
    mysql_free_result(res);
}

void viewMembers() {
    if (mysql_query(conn, "SELECT * FROM members")) {
        cout << "Error fetching members: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    cout << "\n=== Members ===" << endl;
    cout << "ID\tName\tAddress\tPhone" << endl;
    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
    mysql_free_result(res);
}

void viewTransactions() {
    if (mysql_query(conn, "SELECT transactions.id, members.name AS member_name, books.title AS book_title, borrow_date, return_date FROM transactions JOIN members ON transactions.member_id = members.id JOIN books ON transactions.book_id = books.id")) {
        cout << "Error fetching transactions: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    cout << "\n=== Transactions ===" << endl;
    cout << "ID\tMember Name\tBook Title\tBorrow Date\tReturn Date" << endl;
    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
    mysql_free_result(res);
}