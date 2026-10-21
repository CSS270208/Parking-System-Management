#include <bits/stdc++.h>

#include "helper.h"
#include "mapping.h"
#include "uid.h"

using namespace std;

void print3DMatrix(const vector<vector<vector<int>>> &matrix);
void showAllBillSchemes();
void showBillScheme(int slotType);

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
    Vehicle(string plate, int type, string owner)
        : plateNumber(plate), vehicleType(type), ownerName(owner)
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

    int getSlot() const
    {
        return slot;
    }

    void setTheSlot(int code)
    {
        slot = code;
    }
};

#include "FileManager.h"

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
        cout << "4. See Parking Cost\n";
        cout << "5. Pay the current amount\n";
        cout << "6. Admin Control\n";
        cout << "7. Turn off\n";
        cout << "Choose an option: ";
    }

    void bookParking();
    void findVehicle();

    void takeVehicleOut();
    void takeVehicleOut(string plateNumber);

    void payAmount();
    void payAmount(string plateNumber);

    void admin();

    void loadPersistentData()
    {
        if (!FileManager::loadVehicles(vehicles))
        {
            cerr << "Warning: Could not load registered vehicles.\n";
        }

        if (!FileManager::loadActiveSessions(parkingSlots, vehicles))
        {
            cerr << "Warning: Could not restore active sessions.\n";
        }
    }

    void savePersistentData()
    {
        if (!FileManager::saveVehicles(vehicles))
        {
            cerr << "Warning: Could not save vehicles.\n";
        }

        if (!FileManager::saveActiveSessions(parkingSlots))
        {
            cerr << "Warning: Could not save active sessions.\n";
        }
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
             << parkingSlots.size() << endl;

        loadPersistentData();

        int occupied = 0;

        for (const ParkingSlot &slot : parkingSlots)
        {
            if (slot.occupied)
                occupied++;
        }

        cout << "Restored active sessions: "
             << occupied << endl;

        running = true;

        cout << "Welcome to CSS Parking Station!\n";

        while (running)
        {
            showMenu();

            int choice;

            if (!(cin >> choice))
            {
                if (cin.eof())
                    break;

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Invalid input. Enter a number.\n";
                continue;
            }

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
                showAllBillSchemes();
                break;

            case 5:
                payAmount();
                break;

            case 6:
                admin();
                break;

            case 7:
                off();
                break;

            default:
                cout << "Please choose a valid option.\n";
            }
        }

        savePersistentData();
    }

    void off()
    {
        running = false;
        cout << "Parking system turned off.\n";
    }

    friend class Admin;
    friend class FileManager;
};

#include "bookParking.h"
#include "payAmount.h"
#include "takeVehicleOut.h"
#include "findVehicle.h"
#include "admin.h"

void print3DMatrix(
    const vector<vector<vector<int>>> &matrix)
{
    for (int f = 0; f < (int)matrix.size(); f++)
    {
        cout << "\nFloor " << f + 1 << ":\n";

        for (int r = 0; r < (int)matrix[f].size(); r++)
        {
            for (int c = 0; c < (int)matrix[f][r].size(); c++)
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
