#include <bits/stdc++.h>

#include "helper.h"
#include "mapping.h"
#include "uid.h"

using namespace std;

void print3DMatrix(const vector<vector<vector<int>>> &matrix);

class Vehicle
{
private:
    string plateNumber;
    int vehicleType;
    string ownerName;
    int slot = -1;

public:
    Vehicle(string plate, int type, string owner)
    {
        plateNumber = plate;
        vehicleType = type;
        ownerName = owner;
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

    int getSlot() const
    {
        return slot;
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

    vector<vector<vector<int>>> binaryMap;

    vector<ParkingSlot> parkingSlots;

    unordered_map<string, int> slotIndex;

    unordered_map<string, Vehicle> vehicles;

    inline static unordered_map<string, string> adminData = {
        {"CSS", "ChanduChai"}};

    void showMenu()
    {
        cout << "\n===== CSS Parking Station =====\n";
        cout << "1. Book a new parking\n";
        cout << "2. Find my vehicle\n";
        cout << "3. Take my vehicle out\n";
        cout << "4. Pay the current amount\n";
        cout << "5. Admin Control\n";
        cout << "6. Add/Change Maps\n";
        cout << "7. Turn off\n";
        cout << "Choose an option: ";
    }

    void bookParking();

    void findVehicle()
    {
    }

    void takeVehicleOut()
    {
    }

    void payAmount()
    {
        cout << "\n-> Payment selected <-\n";
    }

    void admin()
    {
        string username, password;

        cout << "\nUsername: ";
        cin >> username;

        cout << "Password: ";
        cin >> password;

        auto it = adminData.find(username);

        if (it != adminData.end() && it->second == password)
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
        int available = 0;
        int occupied = 0;

        for (ParkingSlot &slot : parkingSlots)
        {
            if (slot.occupied)
                occupied++;
            else
                available++;
        }

        cout << "\nTotal slots: " << parkingSlots.size() << '\n';
        cout << "Available slots: " << available << '\n';
        cout << "Occupied slots: " << occupied << '\n';
    }

public:
    void on()
    {
        if (!loadMap(binaryMap))
        {
            cerr << "Failed to load parking maps.\n";
            return;
        }

        scanMap(binaryMap, parkingSlots, slotIndex);

        cout << "\nDetected physical slots: "
             << parkingSlots.size() << '\n';

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
                cout << "Map modification is not implemented yet.\n";
                break;

            case 7:
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

    friend void ParkingManagementSystem::bookParking();
};

#include "bookParking.h"

void print3DMatrix(const vector<vector<vector<int>>> &matrix)
{
    for (int f = 0; f < matrix.size(); f++)
    {
        cout << "\nFloor " << f + 1 << ":\n";

        for (int r = 0; r < matrix[f].size(); r++)
        {
            for (int c = 0; c < matrix[f][r].size(); c++)
            {
                cout << matrix[f][r][c] << ' ';
            }

            cout << '\n';
        }
    }
}

int main()
{
    ParkingManagementSystem parkingSystem;
    parkingSystem.on();

    return 0;
}
