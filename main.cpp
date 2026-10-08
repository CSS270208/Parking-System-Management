#include <bits/stdc++.h>
#include "helper.h"
using namespace std;


class Vehicle
{
private:
    string plateNumber;
    string vehicleType;
    string ownerName;
    string slot;

public:
    Vehicle(const string &plateNumber,const string &vehicleType,const string &ownerName): plateNumber(plateNumber),  vehicleType(vehicleType),ownerName(ownerName) {}

    string getPlateNumber() const
    {
        return plateNumber;
    }

    string getVehicleType() const
    {
        return vehicleType;
    }

    string getOwnerName() const
    {
        return ownerName;
    }
};

class ParkingManagementSystem
{
private:
    bool running = false;
    inline static unordered_map<string, string> adminData{
        {"CSS", "ChanduChai"}};

    void showMenu()
    {
        cout << "\n===== CSS Parking Station =====\n";
        cout << "1. Book a new parking\n";
        cout << "2. Find my vehicle\n";
        cout << "3. Take my vehicle out\n";
        cout << "4. Pay the current amount\n";
        cout << "5. Admin Control\n";
        cout << "6. Turn off\n";
        cout << "Choose an option: ";
    }

    void bookParking()
    {
        cout << "\n-> Book parking selected <-\n";

        string plateNumber;
        cout << "Enter the plate Number of Vehicle: ";
        cin >> plateNumber;
        toLowerString(plateNumber);

        string vehicleType;
        cout << "Enter the Vehicle Type: ";
        cin >> vehicleType;
        toLowerString(vehicleType);

        string ownerName;
        cout << "Enter the Name of Owner: ";
        cin >> ownerName;
        toLowerString(ownerName);

        Vehicle(plateNumber,vehicleType,ownerName);
    }

    void findVehicle()
    {
        cout << "\n-> Find vehicle selected <-\n";
    }

    void takeVehicleOut()
    {
        cout << "\n-> Take vehicle out selected <-\n";
    }

    void payAmount()
    {
        cout << "\n-> Payment selected <-\n";
    }

    void admin()
    {
        string username;
        string password;

        cout << "\nUsername: ";
        cin >> username;

        cout << "Password: ";
        cin >> password;

        if (adminData.find(username) != adminData.end() &&
            adminData[username] == password)
        {
            cout << "Admin login successful.\n";
        }
        else
        {
            cout << "Invalid username or password.\n";
        }

        AdminContol();
    }

    void AdminContol()
    {

    }

public:
    void on()
    {
        running = true;

        cout << "Welcome to CSS Parking Station!\n";

        while (running)
        {
            showMenu();

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                bookParking();
                break;

            case 2:
                findVehicle();
                break;

            case 3:
                takeVehicleOut();
                break;

            case 4:
                payAmount();
                break;

            case 5:
                admin();
                break;

            case 6:
                off();
                break;

            default:
                cout << "Please choose a valid option.\n";
            }
        }
    }

    void off()
    {
        running = false;
        cout << "Parking system turned off.\n";
    }
};

int main()
{
    ParkingManagementSystem parkingSystem;

    parkingSystem.on();

    return 0;
}
