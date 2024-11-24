# Car Rental System - README

## Overview

The **Car Rental System** is a simple command-line application that allows users to interact with a car rental service. It connects to a MySQL database to manage cars and their availability. Users can rent and return cars, and the system updates the availability status in the database accordingly.

The system supports the following features:
- Display available cars with details such as brand, model, rent, and availability.
- Rent a car, marking it as unavailable in the database.
- Return a car, marking it as available in the database.

## Prerequisites

Before running the program, ensure that the following dependencies are installed and set up:

1. **MySQL**: You must have MySQL installed on your system and a database set up for the application. The default database used in this system is called `car_rent`.
2. **C++ Compiler**: Ensure that you have a C++ compiler that supports C++11 or higher.
3. **MySQL C++ Connector**: This application requires the MySQL C++ connector (`mysqlclient`). Make sure it is installed on your system.
   - Installation steps:
     - On macOS, download the MySQL connector from the official MySQL website and extract it to a directory.
     - For Linux or Windows, follow platform-specific instructions to install the MySQL C++ connector.

4. **Environment Variable**: Set the `MYSQL_PASSWORD` environment variable with the MySQL root password, as the program uses this password to connect to the database.

### MySQL Database Setup

The program expects a database called `car_rent` with a table `cars`. Below is the SQL script to create the database and table:

```sql
CREATE DATABASE IF NOT EXISTS car_rent;

USE car_rent;

CREATE TABLE IF NOT EXISTS cars (
    Serial INT PRIMARY KEY,
    Brand VARCHAR(255),
    Model VARCHAR(255),
    Rent INT,
    Avail BOOLEAN
);

-- Add some sample car data

INSERT INTO cars (Serial, Brand, Model, Rent, Avail) VALUES
(1234, 'Maruti Suzuki', 'Dzire', 400, TRUE),
(789, 'Maruti Suzuki', 'WagonR', 100, TRUE),
(000, 'Maruti Suzuki', 'Alto', 10, FALSE);
<img width="789" alt="Screenshot 2024-11-25 at 02 06 13" src="https://github.com/user-attachments/assets/3dbe6bba-2f8f-4979-ba67-59c2c0476a4c">
