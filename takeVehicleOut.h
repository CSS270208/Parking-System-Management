#ifndef TAKEVEHICLEOUT_H
#define TAKEVEHICLEOUT_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"

using namespace std;

void resetInfo(int slotIndex, vector<ParkingSlot> &parkingSlots);

void ParkingManagementSystem::takeVehicleOut()
{
    cout << "\n-> Take Vehicle Out Selected <-\n";
    cout << "Enter your vehicle number: ";

    string plateNumber;

    cin >> plateNumber;

    toLowerString(plateNumber);

    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle not found.\n";
        return;
    }

    int index = -1;

    for (int i = 0;
         i < static_cast<int>(parkingSlots.size());
         i++)
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
        cout << "No active parking session found.\n";
        return;
    }

    ParkingSlot &slot = parkingSlots[index];

    if (!slot.paid)
    {
        cout << "Payment is required before exiting.\n";
        cout << "Please use option 4 to pay the amount.\n";
        return;
    }

    string outTime = getCurrentTime();

    // Save the completed session before releasing the slot.
    bool historySaved = FileManager::appendHistory(
        plateNumber,
        slot.uid,
        slot.inTime,
        outTime);

    if (!historySaved)
    {
        cout << "Unable to save parking history.\n";
        cout << "Vehicle has not been removed.\n";
        return;
    }

    cout << "\n===== Vehicle Details =====\n";

    cout << "Plate Number : "
         << it->second.getPlateNumber() << '\n';

    cout << "Owner Name   : "
         << it->second.getOwnerName() << '\n';

    cout << "Vehicle Type : "
         << invertVC(it->second.getVehicleType()) << '\n';

    cout << "Floor        : "
         << slot.uid.getFloor() << '\n';

    cout << "Slot Number  : "
         << slot.uid.getSlotNumber() << '\n';

    cout << "Entry Time   : "
         << slot.inTime << '\n';

    cout << "Exit Time    : "
         << outTime << '\n';

    // Release the physical slot.
    slot.occupied = false;
    slot.plateNumber.clear();
    slot.inTime.clear();
    slot.outTime = outTime;
    slot.amountToPay = 0.0;
    slot.paid = false;

    // The vehicle remains registered.
    it->second.setTheSlot(-1);

    // Persist the updated parking state.
    bool vehiclesSaved =
        FileManager::saveVehicles(vehicles);

    bool sessionsSaved =
        FileManager::saveActiveSessions(parkingSlots);

    if (!vehiclesSaved || !sessionsSaved)
    {
        cout << "Warning: Failed to save the latest parking state.\n";
    }

    cout << "\nThanks for coming! Be sure to visit again.\n";
}

void resetInfo(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    parkingSlots[slotIndex].occupied = false;
    parkingSlots[slotIndex].plateNumber = "";
    parkingSlots[slotIndex].inTime = "";
    parkingSlots[slotIndex].amountToPay = 0;
    parkingSlots[slotIndex].paid = false;

    cout << "\nThanks for coming! Be sure to visit again.\n";
}

#endif
