#include <bits/stdc++.h>
#include "helper.h"
#include "mapping.h"

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
    Vehicle(const string &plateNumber,
            int vehicleType,
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
    vector<vector<int>> graphMap;

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
        cout << "6. Add/Change Maps\n";
        cout << "7. Turn off\n";
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

        if (vehicleType == -1)
        {
            cout << "Invalid vehicle type.\n";
            return;
        }

        string ownerName;
        cout << "Enter the Name of Owner: ";
        cin >> ownerName;
        toLowerString(ownerName);

        Vehicle vehicle(plateNumber, vehicleType, ownerName);

        vector<int> compatibleSlots =
            VehicleToSlotsCode(vehicle.getVehicleType());

        cout << "\nVehicle Details:\n";
        cout << "Plate Number: " << vehicle.getPlateNumber() << '\n';
        cout << "Vehicle Type: " << invertVC(vehicle.getVehicleType()) << '\n';
        cout << "Owner Name: " << vehicle.getOwnerName() << '\n';

        cout << "Compatible Slot Types: ";

        for (int slot : compatibleSlots)
        {
            cout << invertSC(slot) << ' ';
        }

        cout << '\n';

        cout << "Choose a slot type code: ";

        int chosenSlot;
        cin >> chosenSlot;

        if (find(compatibleSlots.begin(),
                 compatibleSlots.end(),
                 chosenSlot) == compatibleSlots.end())
        {
            cout << "Incompatible slot type.\n";
            return;
        }

        vehicle.setTheSlot(chosenSlot);

        cout << "Slot type assigned: "
             << invertSC(vehicle.getSlot()) << '\n';

        // Next: Find an available physical slot using binaryMap.
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
    }

public:
    void on()
    {
        if (!loadMap(binaryMap))
        {
            cout << "Failed to load parking maps.\n";
            return;
        }
        //
        print3DMatrix(binaryMap);
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
};

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
