#ifndef BOOKPARKING_H
#define BOOKPARKING_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"

using namespace std;

void ParkingManagementSystem::bookParking()
{
    cout << "\n-> Book parking selected <-\n";

    string plateNumber;

    cout << "Enter plate number: ";
    cin >> plateNumber;
    toLowerString(plateNumber);

    if (vehicles.find(plateNumber) != vehicles.end())
    {
        cout << "Vehicle already registered.\n";
        return;
    }

    string vehicleTypeString;

    cout << "Enter vehicle type: ";
    cin >> vehicleTypeString;
    toLowerString(vehicleTypeString);

    int vehicleType = VC(vehicleTypeString);

    if (vehicleType == -1)
    {
        cout << "Invalid vehicle type.\n";
        return;
    }

    string ownerName;

    cout << "Enter owner's name: ";
    cin >> ownerName;
    toLowerString(ownerName);

    Vehicle vehicle(plateNumber, vehicleType, ownerName);

    vector<int> compatibleSlots = VehicleToSlotsCode(vehicle.getVehicleType());

    cout << "\nVehicle Details:\n";
    cout << "Plate Number: " << vehicle.getPlateNumber() << '\n';
    cout << "Vehicle Type: " << invertVC(vehicle.getVehicleType()) << '\n';
    cout << "Owner Name: " << vehicle.getOwnerName() << '\n';

    cout << "Compatible Slot Types: ";

    for (int type : compatibleSlots)
    {
        cout << invertSC(type) << ' ';
    }

    cout << '\n';
    cout << "Choose a slot: ";

    string temp;
    cin >> temp;
    toLowerString(temp);

    int chosenSlot = SC(temp);

    if (find(compatibleSlots.begin(), compatibleSlots.end(), chosenSlot) == compatibleSlots.end())
    {
        cout << "Incompatible slot type.\n";
        return;
    }

    for (int i = 0; i < parkingSlots.size(); i++)
    {
        if (parkingSlots[i].uid.getSlotType() != chosenSlot)
            continue;

        if (parkingSlots[i].occupied)
            continue;

        parkingSlots[i].occupied = true;
        parkingSlots[i].plateNumber = plateNumber;

        vehicle.setTheSlot(i);

        vehicles.emplace(plateNumber, vehicle);

        cout << "\nParking successful.\n";
        cout << "Assigned UID: "
             << parkingSlots[i].uid.toString() << '\n';

        return;
    }

    cout << "No available physical slot of this type.\n";
}

#endif
