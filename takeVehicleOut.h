#ifndef TAKEVEHICLEOUT_H
#define TAKEVEHICLEOUT_H

#include <bits/stdc++.h>
using namespace std;

void resetInfo(int slotIndex, vector<ParkingSlot> &parkingSlots);

void ParkingManagementSystem::takeVehicleOut()
{
    cout << "\n-> Take Vehicle Out Selected <-\n";
    cout << "Enter your vehicle number: ";

    string plateNumber;
    cin >> plateNumber;

    toLowerString(plateNumber);

    takeVehicleOut(plateNumber);
}

void ParkingManagementSystem::takeVehicleOut(string plateNumber)
{
    toLowerString(plateNumber);

    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle not found.\n";
        return;
    }

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
        cout << "No active parking session found.\n";
        return;
    }

    ParkingSlot &slot = parkingSlots[index];

    bool canExit = false;

    if (slot.paid && !slot.lastPaymentTime.empty())
    {
        time_t lastPayment = convertTime(slot.lastPaymentTime);
        time_t current = convertTime(getCurrentTime());

        if (lastPayment != (time_t)-1 &&
            current != (time_t)-1 &&
            current >= lastPayment)
        {
            double seconds = difftime(current, lastPayment);

            if (seconds <= 3600)
            {
                canExit = true;
            }
        }
    }

    if (!canExit)
    {
        cout << "\nPayment is required before exiting.\n";
        cout << "1. Pay the current amount\n";
        cout << "2. Cancel\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            payAmount(plateNumber);

            if (!slot.occupied ||
                slot.plateNumber != plateNumber)
            {
                return;
            }

            if (!slot.paid)
            {
                cout << "\nPayment was not completed.\n";
                cout << "Your vehicle is still parked.\n";
                return;
            }

            cout << "\nPayment successful. Proceeding with checkout.\n";

            takeVehicleOut(plateNumber);
            return;
        }

        case 2:
            cout << "Vehicle exit cancelled.\n";
            return;

        default:
            cout << "Invalid choice.\n";
            return;
        }
    }

    string outTime = getCurrentTime();

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

    cout << "\n========== VEHICLE DETAILS ==========\n";

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

    cout << "Last Payment : "
         << slot.lastPaymentTime << '\n';

    cout << "=====================================\n";

    slot.occupied = false;
    slot.plateNumber.clear();
    slot.inTime.clear();
    slot.outTime = outTime;
    slot.amountToPay = 0.0;
    slot.paid = false;
    slot.lastPaymentTime.clear();

    it->second.setTheSlot(-1);

    bool vehiclesSaved = FileManager::saveVehicles(vehicles);
    bool sessionsSaved = FileManager::saveActiveSessions(parkingSlots);

    if (!vehiclesSaved || !sessionsSaved)
    {
        cout << "Warning: Failed to save the latest parking state.\n";
    }

    cout << "\nVehicle successfully taken out!\n";
    cout << "The parking slot is now available.\n";
    cout << "Parking history has been preserved.\n";
    cout << "Thanks for coming! Be sure to visit again.\n";
}

void resetInfo(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    if (slotIndex < 0 ||
        slotIndex >= (int)parkingSlots.size())
    {
        return;
    }

    parkingSlots[slotIndex].occupied = false;
    parkingSlots[slotIndex].plateNumber.clear();
    parkingSlots[slotIndex].inTime.clear();
    parkingSlots[slotIndex].outTime.clear();
    parkingSlots[slotIndex].amountToPay = 0.0;
    parkingSlots[slotIndex].paid = false;
    parkingSlots[slotIndex].lastPaymentTime.clear();
}

#endif
