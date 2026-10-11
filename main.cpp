#include <bits/stdc++.h>

#include "helper.h"
#include "mapping.h"
#include "uid.h"

using namespace std;

void print3DMatrix(const vector<vector<vector<int>>> &matrix);

class Admin;
class Vehicle;

class Vehicle
{
private:
    string plateNumber;
    int vehicleType;
    string ownerName;
    int slot = -1;

public:
    Vehicle(string plate, int type, string owner) : plateNumber(plate),vehicleType(type),ownerName(owner) {}

    string getPlateNumber() const {
        return plateNumber;
    }

    int getVehicleType() const {
        return vehicleType;
    }

    string getOwnerName() const {
        return ownerName;
    }

    int getSlot() const {
        return slot;
    }

    void setTheSlot(int code) {
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
    void findVehicle();
    void takeVehicleOut();
    void payAmount();
    void payAmount(string);
    void admin();

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

    // Admin can access all private members of this class.
    friend class Admin;

    // Existing operation functions retain their friend access.
    friend void ParkingManagementSystem::bookParking();
    friend void ParkingManagementSystem::findVehicle();
    friend void ParkingManagementSystem::takeVehicleOut();
    friend void ParkingManagementSystem::payAmount();
    friend void ParkingManagementSystem::payAmount(string);
};

// Include implementations after the class definition.
#include "bookParking.h"
#include "takeVehicleOut.h"
#include "findVehicle.h"
#include "payAmount.h"
#include "admin.h"

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
