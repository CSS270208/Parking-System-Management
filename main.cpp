#include <bits/stdc++.h>
#include "helper.h"

using namespace std;

class Vehicle
{
private:
    string plateNumber;
    int vehicleType;
    string ownerName;
    int slot;

public:
    Vehicle(const string &plateNumber,
            const int &vehicleType,
            const string &ownerName)
        : plateNumber(plateNumber),
          vehicleType(vehicleType),
          ownerName(ownerName)
    {
    }

    string getPlateNumber() const
    {
        return plateNumber;
    }

    int getVehicleType() const
    {
        return vehicleType;
    }

    string getOwnerName() const
    {
        return ownerName;
    }

    void setTheSlot(int code)
    {
        slot = code;
    }
};


class ParkingManagementSystem
{
private:
    bool running = false;
    static vector<vector<vector<int>>> binaryMap;

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

        string vehicleTypeString;
        cout << "Enter the Vehicle Type: ";
        cin >> vehicleTypeString;
        toLowerString(vehicleTypeString);
        int vehicleType = VC(vehicleTypeString);

        string ownerName;
        cout << "Enter the Name of Owner: ";
        cin >> ownerName;
        toLowerString(ownerName);



        Vehicle vehicle(plateNumber, vehicleType, ownerName);

        vector<int> compatibleSlots = VehicleToSlotsCode(vehicle.getVehicleType());

        cout << "\nVehicle Details:\n";
        cout << "Plate Number: " << vehicle.getPlateNumber() << '\n';
        cout << "Vehicle Type: " << vehicle.getVehicleType() << '\n';
        cout << "Owner Name: " << vehicle.getOwnerName() << '\n';

        cout << "Compatible Slot Types: ";

        for (int slot : compatibleSlots)
        {
            cout << invertSC(slot) << ' ';
        }

        cout << '\n';

        cout << "Choose a slot  \n";
        // Feature :- add price with the slots
        int ChoosenSlot;
        cin >> ChoosenSlot;
        vehicle.setTheSlot(ChoosenSlot);

        // Feature :- you can add prefence for something and use priority queue in searching
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
            AdminContol();
        }
        else
        {
            cout << "Invalid username or password.\n";
        }
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

vector<vector<vector<int>>> ParkingManagementSystem::binaryMap;

int main()
{
    ParkingManagementSystem parkingSystem;

    parkingSystem.on();

    return 0;
}
