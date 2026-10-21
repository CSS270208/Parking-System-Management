#ifndef ADMIN_H
#define ADMIN_H

#include <bits/stdc++.h>
using namespace std;

class Admin
{
public:
    static void login(ParkingManagementSystem &system)
    {
        string username;
        string password;

        cout << "\nUsername: ";
        cin >> username;

        cout << "Password: ";
        cin >> password;

        auto it = system.adminData.find(username);

        if (it != system.adminData.end() &&
            it->second == password)
        {
            cout << "\nAdmin login successful.\n";
            control(system);
        }
        else
        {
            cout << "Invalid username or password.\n";
        }
    }

    static void control(ParkingManagementSystem &system)
    {
        while (true)
        {
            cout << "\n========== ADMIN CONTROL ==========\n";
            cout << "1. View Parking Dashboard\n";
            cout << "2. View Floor Maps\n";
            cout << "3. View Active Parking Sessions\n";
            cout << "4. View Payment History\n";
            cout << "5. View Vehicle History\n";
            cout << "6. View Registered Vehicles\n";
            cout << "7. Back\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                showDashboard(system);
                break;

            case 2:
                showFloorMaps();
                break;

            case 3:
                showFile(
                    "Data/active_sessions.txt",
                    "ACTIVE PARKING SESSIONS");
                break;

            case 4:
                showFile(
                    "Data/payments.txt",
                    "PAYMENT HISTORY");
                break;

            case 5:
                showFile(
                    "Data/vehicle_history.txt",
                    "VEHICLE HISTORY");
                break;

            case 6:
                showFile(
                    "Data/vehicles.txt",
                    "REGISTERED VEHICLES");
                break;

            case 7:
                return;

            default:
                cout << "Invalid choice.\n";
            }
        }
    }

    static void showDashboard(ParkingManagementSystem &system)
    {
        int available = 0;
        int occupied = 0;

        for (const ParkingSlot &slot : system.parkingSlots)
        {
            if (slot.occupied)
                occupied++;
            else
                available++;
        }

        cout << "\n========== ADMIN DASHBOARD ==========\n";

        cout << "Total slots: "
             << system.parkingSlots.size() << '\n';

        cout << "Available slots: "
             << available << '\n';

        cout << "Occupied slots: "
             << occupied << '\n';

        cout << "=====================================\n";
    }

    static void showFile(string fileName, string title)
    {
        ifstream file(fileName);

        cout << "\n========== " << title << " ==========\n";

        if (!file)
        {
            cout << "Unable to open " << fileName << ".\n";
            cout << "Make sure the Data folder and file exist.\n";
            cout << "=====================================\n";
            return;
        }

        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            cout << line << '\n';
            found = true;
        }

        if (!found)
        {
            cout << "No records found.\n";
        }

        cout << "=====================================\n";
    }

    static int countFloors()
    {
        int floors = 0;

        while (true)
        {
            string fileName =
                "Maps/floor" + to_string(floors + 1) + ".txt";

            ifstream file(fileName);

            if (!file)
                break;

            floors++;
        }

        return floors;
    }

    static void showFloorMaps()
    {
        int floors = countFloors();

        if (floors == 0)
        {
            cout << "\nNo floor maps found in the Maps folder.\n";
            return;
        }

        while (true)
        {
            cout << "\n========== FLOOR MAPS ==========\n";
            cout << "Floors detected: " << floors << '\n';

            for (int i = 1; i <= floors; i++)
            {
                cout << i << ". View Floor " << i << '\n';
            }

            cout << floors + 1 << ". Back\n";
            cout << "Choose a floor: ";

            int choice;
            cin >> choice;

            if (choice == floors + 1)
                return;

            if (choice < 1 || choice > floors)
            {
                cout << "Invalid floor choice.\n";
                continue;
            }

            showMap(choice);
        }
    }

    static void showMap(int floor)
    {
        string fileName =
            "Maps/floor" + to_string(floor) + ".txt";

        ifstream file(fileName);

        if (!file)
        {
            cout << "Unable to open " << fileName << '\n';
            return;
        }

        cout << "\n========== FLOOR " << floor << " ==========\n";
        cout << "Legend: . = Empty space, # = Parking space\n\n";

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);
            int value;

            while (ss >> value)
            {
                if (value == 0)
                    cout << ". ";
                else if (value == 1)
                    cout << "# ";
                else
                    cout << "? ";
            }

            cout << '\n';
        }

        cout << "\n================================\n";
    }
};

inline void ParkingManagementSystem::admin()
{
    Admin::login(*this);
}

#endif
