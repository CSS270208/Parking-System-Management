#ifndef FINDVEHICLE_H
#define FINDVEHICLE_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"

using namespace std;

void ParkingManagementSystem::findVehicle()
{
    cout << "\nEnter your vehicle Number: ";

    string plateNumber;
    cin >> plateNumber;

    toLowerString(plateNumber);

    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle Not Found\n";
        return;
    }

    Vehicle &vehicle = it->second;

    int index = -1;

    for (int i = 0; i < (int)parkingSlots.size(); i++)
    {
        if (parkingSlots[i].occupied &&
            parkingSlots[i].plateNumber == plateNumber)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        cout << "Vehicle is registered but no occupied parking slot was found.\n";
        return;
    }

    const UID &uid = parkingSlots[index].uid;
    const ParkingSlot &slot = parkingSlots[index];

    cout << "\n===== Vehicle Details =====\n";

    cout << "Plate Number : "
         << vehicle.getPlateNumber() << '\n';

    cout << "Owner Name   : "
         << vehicle.getOwnerName() << '\n';

    cout << "Vehicle Type : "
         << invertVC(vehicle.getVehicleType())
         << " (" << vehicle.getVehicleType() << ")\n";

    cout << "Slot Index   : " << index << '\n';

    cout << "\n===== Parking Slot Details =====\n";

    cout << "Floor        : "
         << uid.getFloor() << '\n';

    cout << "Slot Type    : "
         << invertSC(uid.getSlotType())
         << " (" << uid.getSlotType() << ")\n";

    cout << "Slot Number  : "
         << uid.getSlotNumber() << '\n';

    cout << "Entry Time   : "
         << slot.inTime << '\n';

    if (!slot.lastPaymentTime.empty())
    {
        cout << "Last Payment : "
             << slot.lastPaymentTime << '\n';

        cout << "Grace Ends   : "
             << addOneHour(slot.lastPaymentTime) << '\n';
    }
}

#endif
