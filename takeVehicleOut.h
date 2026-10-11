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

    cout << "\nEnter your vehicle Number : ";
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

    int slotIndex = -1;

    for (int i = 0; i < parkingSlots.size(); i++)
    {
        if (parkingSlots[i].occupied &&
            parkingSlots[i].plateNumber == plateNumber)
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

    if (!parkingSlots[slotIndex].paid)
    {
        while (true)
        {
            cout << "\nYou need to pay before taking out your vehicle.\n";
            cout << "1. Pay the Amount\n";
            cout << "2. Go Back to Main Menu\n";
            cout << "Choose an option: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                payAmount(plateNumber);

                if (parkingSlots[slotIndex].paid)
                    break;

                continue;

            case 2:
                return;

            default:
                cout << "Please choose a valid option.\n";
                continue;
            }

            break;
        }
    }

    if (!parkingSlots[slotIndex].paid)
    {
        cout << "Payment is required before taking out your vehicle.\n";
        return;
    }

    cout << "\n===== Vehicle Details =====\n";
    cout << "Plate Number : " << vehicle.getPlateNumber() << '\n';
    cout << "Owner Name   : " << vehicle.getOwnerName() << '\n';
    cout << "Vehicle Type : " << vehicle.getVehicleType() << '\n';
    cout << "Slot Index   : " << slotIndex << '\n';

    resetInfo(slotIndex, parkingSlots);
    vehicle.setTheSlot(-1);
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
