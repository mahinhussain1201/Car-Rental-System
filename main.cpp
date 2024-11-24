// to run
// g++ -o car_rental_system main.cpp -I/usr/local/mysql-9.1.0-macos14-arm64/include -L/usr/local/mysql-9.1.0-macos14-arm64/lib -lmysqlclient
// export DYLD_LIBRARY_PATH=/usr/local/mysql-9.1.0-macos14-arm64/lib:$DYLD_LIBRARY_PATH
//./car_rental_system
// if not working use export MYSQL_PASSWORD='<password>' then run /car_rental_system


#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <cstdlib> // for getenv
#include <thread>
#include <sstream>
#include <iomanip>
using namespace std;

const char *HOST = "localhost";
const char *USER = "root";
const char *DB = "car_rent"; // name of db

// ANSI escape codes for colors
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

class Car
{
private:
    string Brand, Model;
    int Serial, Rent;
    bool Avail;

public:
    Car(int serial, string brand, string model, int rent, bool avail)
        : Serial(serial), Brand(brand), Model(model), Rent(rent), Avail(avail) {}

    int getSerial() { return Serial; }
    string getBrand() { return Brand; }
    string getModel() { return Model; }
    int getRent() { return Rent; }
    bool getAvail() { return Avail; }
};

bool isAvail(MYSQL *conn, string toS)
{
    string query = "SELECT Avail FROM cars WHERE Serial = '" + toS + "'";
    if (mysql_query(conn, query.c_str()))
    {
        cerr << RED << "Error: " << mysql_error(conn) << RESET << endl;
        return false;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row)
        {
            int availability = atoi(row[0]);
            mysql_free_result(result);
            return (availability == 1);
        }
        mysql_free_result(result);
    }
    return false;
}

void displayError(const char *message, MYSQL *conn)
{
    cerr << RED << message << ": " << mysql_error(conn) << RESET << endl;
}

void printResultSet(MYSQL_RES *result)
{
    int numFields = mysql_num_fields(result);
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    // Dynamic column widths based on field names and data
    int colWidths[5] = {10, 20, 20, 10, 10};

    // Print headers
    cout << BOLD;
    for (int i = 0; i < numFields; i++)
    {
        cout << left << setw(colWidths[i]) << fields[i].name;
    }
    cout << RESET << endl;

    // Separator
    cout << YELLOW << string(70, '-') << RESET << endl;

    // Print rows
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        for (int i = 0; i < numFields; i++)
        {
            if (i == 4) // Availability column
                cout << left << setw(colWidths[i]) << (atoi(row[i]) ? "Yes" : "No");
            else
                cout << left << setw(colWidths[i]) << (row[i] ? row[i] : "NULL");
        }
        cout << endl;
    }
}

void markCarAsAvailable(MYSQL *conn, int serial)
{
    stringstream ss;
    ss << serial;
    string serialStr = ss.str();

    string query = "UPDATE cars SET Avail=1 WHERE Serial='" + serialStr + "'";
    if (mysql_query(conn, query.c_str()))
    {
        displayError("Failed to mark car as available", conn);
    }
    else
    {
        cout << GREEN << "Car marked as available successfully!" << RESET << endl;
    }
}

int main()
{
    const char *PW = getenv("MYSQL_PASSWORD");
    if (!PW)
    {
        cerr << "Error: MYSQL_PASSWORD environment variable not set!\n";
        return EXIT_FAILURE;
    }
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, HOST, USER, PW, DB, 3306, NULL, 0))
    {
        displayError("Database connection failed", conn);
        return EXIT_FAILURE;
    }
    cout << GREEN << "Logged in successfully!" << RESET << endl;

    Car car1(1234, "Maruti Suzuki", "Dzire", 400, true);
    Car car2(789, "Maruti Suzuki", "WagonR", 100, true);
    Car car3(000, "Maruti Suzuki", "Alto", 10, false);

    string saveQueries[] = {
        "INSERT INTO cars (Serial, Brand, Model, Rent, Avail) VALUES(1234,'Maruti Suzuki','Dzire',400,true)",
        "INSERT INTO cars (Serial, Brand, Model, Rent, Avail) VALUES(789,'Maruti Suzuki','WagonR',100,true)",
        "INSERT INTO cars (Serial, Brand, Model, Rent, Avail) VALUES(000,'Maruti Suzuki','Alto',10,false)"};

    for (const auto &query : saveQueries)
    {
        mysql_query(conn, query.c_str()); // Ignore duplicate entries
    }

    bool exit = false;

    while (!exit)
    {
        system("clear");
        cout << CYAN << BOLD << "Welcome to Car Rental System" << RESET << endl;
        cout << YELLOW << "****************************" << RESET << endl;

        string getQuery = "SELECT * FROM cars";
        if (mysql_query(conn, getQuery.c_str()))
        {
            displayError("Failed to fetch car data", conn);
        }
        else
        {
            MYSQL_RES *result = mysql_store_result(conn);
            if (result)
            {
                printResultSet(result);
                mysql_free_result(result);
            }
        }

        int choice;
        cout << GREEN << "\n1. Rent a Car" << RESET << endl;
        cout << CYAN << "2. Return a Car" << RESET << endl;
        cout << RED << "0. Exit" << RESET << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int serial;
            cout << "Enter Serial No. to rent: ";
            cin >> serial;

            stringstream ss;
            ss << serial;
            string serialStr = ss.str();

            if (isAvail(conn, serialStr))
            {
                string updateQuery = "UPDATE cars SET Avail=0 WHERE Serial='" + serialStr + "'";
                if (mysql_query(conn, updateQuery.c_str()))
                {
                    displayError("Failed to update car availability", conn);
                }
                else
                {
                    cout << GREEN << "Car rented successfully!" << RESET << endl;
                }
            }
            else
            {
                cout << RED << "Sorry, this car is already booked or does not exist!" << RESET << endl;
            }
            this_thread::sleep_for(chrono::seconds(3));
        }
        else if (choice == 2)
        {
            int serial;
            cout << "Enter Serial No. to return: ";
            cin >> serial;

            markCarAsAvailable(conn, serial);
            this_thread::sleep_for(chrono::seconds(3));
        }
        else if (choice == 0)
        {
            exit = true;
            cout << GREEN << "Thank you for using the Car Rental System. Goodbye!" << RESET << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
        else
        {
            cout << RED << "Invalid choice! Please try again." << RESET << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    mysql_close(conn);
    return EXIT_SUCCESS;
}
