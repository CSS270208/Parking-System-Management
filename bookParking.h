#ifndef BOOKPARKING_H
#define BOOKPARKING_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"
#include "payAmount.h"

using namespace std;

void ParkingManagementSystem::bookParking()
{
    cout << "\n-> Book Parking Selected <-\n";

    string plateNumber;

    cout << "Enter plate number: ";
    cin >> plateNumber;
    toLowerString(plateNumber);

    // Check whether the vehicle is already registered.
    if (vehicles.find(plateNumber) != vehicles.end())
    {
        cout << "Vehicle already registered.\n";
        return;
    }

    int choice;

    cout << "\nChoose Booking Method:\n";
    cout << "1. Book by Vehicle Type\n";
    cout << "2. Book by Slot Dimensions\n";
    cout << "Enter choice: ";
    cin >> choice;

    string vehicleTypeString;
    string ownerName;

    int vehicleType = -1;
    int chosenSlot = -1;

    switch (choice)
    {
    case 1:
    {
        cout << "\nEnter vehicle type (Bike/Car/Truck/EV): ";
        cin >> vehicleTypeString;
        toLowerString(vehicleTypeString);

        vehicleType = VC(vehicleTypeString);

        if (vehicleType == -1)
        {
            cout << "Invalid vehicle type.\n";
            return;
        }

        vector<int> compatibleSlots =
            VehicleToSlotsCode(vehicleType);

        cout << "\nCompatible Slot Types:\n";

        for (int type : compatibleSlots)
        {
            cout << invertSC(type) << ' ';
        }

        cout << "\nEnter slot dimensions: ";

        string temp;
        cin >> temp;
        toLowerString(temp);

        chosenSlot = SC(temp);

        if (find(compatibleSlots.begin(),
                 compatibleSlots.end(),
                 chosenSlot) == compatibleSlots.end())
        {
            cout << "Incompatible slot dimensions.\n";
            return;
        }

        break;
    }

    case 2:
    {
        cout << "\nAvailable Slot Types:\n";

        vector<int> availableTypes;

        for (const auto &slot : parkingSlots)
        {
            int type = slot.uid.getSlotType();

            if (!slot.occupied &&
                find(availableTypes.begin(),
                     availableTypes.end(),
                     type) == availableTypes.end())
            {
                availableTypes.push_back(type);
            }
        }

        if (availableTypes.empty())
        {
            cout << "No parking slots are currently available.\n";
            return;
        }

        for (int type : availableTypes)
        {
            cout << invertSC(type) << ' ';
        }

        cout << "\nEnter slot dimensions: ";

        string temp;
        cin >> temp;
        toLowerString(temp);

        chosenSlot = SC(temp);

        if (find(availableTypes.begin(),
                 availableTypes.end(),
                 chosenSlot) == availableTypes.end())
        {
            cout << "Invalid or unavailable slot dimensions.\n";
            return;
        }

        vehicleType = -1;

        break;
    }

    default:
    {
        cout << "Invalid booking choice.\n";
        return;
    }
    }

    // Display the billing scheme for the selected slot.
    showBillScheme(chosenSlot);

    // Get the owner's name.
    cout << "\nEnter owner's name: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, ownerName);

    toLowerString(ownerName);

    if (ownerName.empty())
    {
        cout << "Owner name cannot be empty.\n";
        return;
    }

    // Create the vehicle object.
    Vehicle vehicle(plateNumber, vehicleType, ownerName);

    cout << "\nVehicle Details:\n";

    cout << "Plate Number: "
         << vehicle.getPlateNumber() << '\n';

    if (vehicleType != -1)
    {
        cout << "Vehicle Type: "
             << invertVC(vehicle.getVehicleType()) << '\n';
    }
    else
    {
        cout << "Vehicle Type: Not specified\n";
    }

    cout << "Owner Name: "
         << vehicle.getOwnerName() << '\n';

    cout << "Selected Slot Type: "
         << invertSC(chosenSlot) << '\n';

    // Find and assign the first available physical slot.
    for (int i = 0; i < parkingSlots.size(); i++)
    {
        if (parkingSlots[i].uid.getSlotType() != chosenSlot)
            continue;

        if (parkingSlots[i].occupied)
            continue;

        parkingSlots[i].occupied = true;
        parkingSlots[i].plateNumber = plateNumber;

        parkingSlots[i].inTime = getCurrentTime();
        parkingSlots[i].amountToPay = 0.0;

        vehicle.setTheSlot(i);

        vehicles.emplace(plateNumber, vehicle);

        cout << "\nParking successful.\n";

        cout << "Assigned UID: "
             << parkingSlots[i].uid.toString() << '\n';

        cout << "Entry Time: "
             << parkingSlots[i].inTime << '\n';

        return;
    }

    cout << "No available physical slot of this type.\n";
}

#endif
