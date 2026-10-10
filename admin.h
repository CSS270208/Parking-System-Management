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

        if (it != system.adminData.end() && it->second == password)
        {
            cout << "Admin login successful.\n";

            control(system);
        }
        else
        {
            cout << "Invalid username or password.\n";
        }
    }

    static void control(ParkingManagementSystem &system)
    {
        int available = 0;
        int occupied = 0;

        for (const ParkingSlot &slot : system.parkingSlots)
        {
            if (slot.occupied)
            {
                occupied++;
            }
            else
            {
                available++;
            }
        }

        cout << "\n===== Admin Dashboard =====\n";

        cout << "Total slots: "
             << system.parkingSlots.size() << '\n';

        cout << "Available slots: "
             << available << '\n';

        cout << "Occupied slots: "
             << occupied << '\n';
    }
};

inline void ParkingManagementSystem::admin()
{
    Admin::login(*this);
}

#endif
