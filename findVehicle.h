#ifndef FINDVEHICLE_H
#define FINDVEHICLE_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"

using namespace std;

void ParkingManagementSystem::findVehicle()
{
    cout << "\nEnter your vehcile Number : ";
    string plateNumber;
    cin >> plateNumber;

    toLowerString(plateNumber);

    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle Not Found\n";
        return;
    }

    Vehicle &vehicle = (*it).second;

    int slotIndex = -1;

    for (int i = 0; i < parkingSlots.size(); i++)
    {
        if (parkingSlots[i].occupied && parkingSlots[i].plateNumber == plateNumber)
        {
            slotIndex = i;
            break;
        }
    }

    if (slotIndex == -1)
    {
        cout << "Vehicle is registered but no occupied parking slot was found.\n";
        return;
    }

    const UID &uid = parkingSlots[slotIndex].uid;

    cout << "\n===== Vehicle Details =====\n";
    cout << "Plate Number : " << vehicle.getPlateNumber() << '\n';
    cout << "Owner Name   : " << vehicle.getOwnerName() << '\n';
    cout << "Vehicle Type : " << vehicle.getVehicleType() << '\n';
    cout << "Slot Index   : " << slotIndex << '\n';

    cout << "\n===== Parking Slot Details =====\n";
    cout << "Floor        : " << uid.getFloor() << '\n';
    cout << "Slot Type    : " << uid.getSlotType() << '\n';
    cout << "Slot Number  : " << uid.getSlotNumber() << '\n';
}

#endif
