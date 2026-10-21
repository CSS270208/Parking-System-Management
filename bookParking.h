#ifndef BOOKPARKING_H
#define BOOKPARKING_H

#include <bits/stdc++.h>
using namespace std;

void ParkingManagementSystem::bookParking()
{
    cout << "\n-> Book Parking Selected <-\n";

    string plateNumber;

    cout << "Enter plate number: ";
    cin >> plateNumber;

    toLowerString(plateNumber);

    auto vehicleIt = vehicles.find(plateNumber);

    bool alreadyRegistered = (vehicleIt != vehicles.end());

    for (int i = 0; i < (int)parkingSlots.size(); i++)
    {
        if (parkingSlots[i].occupied &&
            parkingSlots[i].plateNumber == plateNumber)
        {
            cout << "Vehicle is already parked.\n";
            return;
        }
    }

    int vehicleType = -1;
    int chosenSlot = -1;
    string ownerName;

    if (alreadyRegistered)
    {
        vehicleType = vehicleIt->second.getVehicleType();
        ownerName = vehicleIt->second.getOwnerName();

        cout << "Registered vehicle found.\n";
        cout << "Owner: " << ownerName << '\n';
    }

    int choice;

    cout << "\nChoose Booking Method:\n";
    cout << "1. Book by Vehicle Type\n";
    cout << "2. Book by Slot Dimensions\n";
    cout << "Enter choice: ";

    cin >> choice;

    switch (choice)
    {
    case 1:
    {
        if (!alreadyRegistered)
        {
            cout << "\nSelect Vehicle Type:\n";
            cout << "1. Bike\n";
            cout << "2. Car\n";
            cout << "3. Truck\n";
            cout << "4. EV\n";
            cout << "Enter choice: ";

            int typeChoice;
            cin >> typeChoice;

            switch (typeChoice)
            {
            case 1:
                vehicleType = VC("bike");
                break;

            case 2:
                vehicleType = VC("car");
                break;

            case 3:
                vehicleType = VC("truck");
                break;

            case 4:
                vehicleType = VC("ev");
                break;

            default:
                cout << "Invalid vehicle type.\n";
                return;
            }
        }

        vector<int> compatibleSlots = VehicleToSlotsCode(vehicleType);

        cout << "\nCompatible Slot Types:\n";

        for (int i = 0; i < (int)compatibleSlots.size(); i++)
        {
            cout << i + 1 << ". "
                 << invertSC(compatibleSlots[i]) << '\n';
        }

        cout << "Enter slot choice: ";

        int slotChoice;
        cin >> slotChoice;

        if (slotChoice < 1 ||
            slotChoice > (int)compatibleSlots.size())
        {
            cout << "Invalid slot choice.\n";
            return;
        }

        chosenSlot = compatibleSlots[slotChoice - 1];

        break;
    }

    case 2:
    {
        vector<int> availableTypes;

        for (int i = 0; i < (int)parkingSlots.size(); i++)
        {
            int type = parkingSlots[i].uid.getSlotType();

            if (!parkingSlots[i].occupied &&
                find(
                    availableTypes.begin(),
                    availableTypes.end(),
                    type) == availableTypes.end())
            {
                availableTypes.push_back(type);
            }
        }

        if (availableTypes.empty())
        {
            cout << "No parking slots are available.\n";
            return;
        }

        cout << "\nAvailable Slot Types:\n";

        for (int i = 0; i < (int)availableTypes.size(); i++)
        {
            cout << i + 1 << ". "
                 << invertSC(availableTypes[i]) << '\n';
        }

        cout << "Enter slot choice: ";

        int slotChoice;
        cin >> slotChoice;

        if (slotChoice < 1 ||
            slotChoice > (int)availableTypes.size())
        {
            cout << "Invalid slot choice.\n";
            return;
        }

        chosenSlot = availableTypes[slotChoice - 1];

        if (!alreadyRegistered)
        {
            cout << "\nSelect Vehicle Type:\n";
            cout << "1. Bike\n";
            cout << "2. Car\n";
            cout << "3. Truck\n";
            cout << "4. EV\n";
            cout << "Enter choice: ";

            int typeChoice;
            cin >> typeChoice;

            switch (typeChoice)
            {
            case 1:
                vehicleType = VC("bike");
                break;

            case 2:
                vehicleType = VC("car");
                break;

            case 3:
                vehicleType = VC("truck");
                break;

            case 4:
                vehicleType = VC("ev");
                break;

            default:
                cout << "Invalid vehicle type.\n";
                return;
            }
        }

        break;
    }

    default:
        cout << "Invalid booking choice.\n";
        return;
    }

    showBillScheme(chosenSlot);

    if (!alreadyRegistered)
    {
        cout << "\nEnter owner's name: ";

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, ownerName);

        if (ownerName.empty())
        {
            cout << "Owner name cannot be empty.\n";
            return;
        }
    }

    int index = -1;

    for (int i = 0; i < (int)parkingSlots.size(); i++)
    {
        if (!parkingSlots[i].occupied &&
            parkingSlots[i].uid.getSlotType() == chosenSlot)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        cout << "No available physical slot of this type.\n";
        return;
    }

    if (!alreadyRegistered)
    {
        Vehicle vehicle(plateNumber, vehicleType, ownerName);
        vehicle.setTheSlot(index);

        vehicles.insert(make_pair(plateNumber, vehicle));
    }
    else
    {
        vehicles.find(plateNumber)->second.setTheSlot(index);
    }

    ParkingSlot &slot = parkingSlots[index];

    slot.occupied = true;
    slot.paid = false;
    slot.plateNumber = plateNumber;
    slot.inTime = getCurrentTime();
    slot.outTime.clear();
    slot.amountToPay = 0.0;
    slot.lastPaymentTime.clear();

    bool vehiclesSaved = FileManager::saveVehicles(vehicles);
    bool sessionsSaved = FileManager::saveActiveSessions(parkingSlots);

    if (!vehiclesSaved || !sessionsSaved)
    {
        cout << "Warning: Could not save all parking data.\n";
    }

    cout << "\nParking successful!\n";
    cout << "Plate Number: " << plateNumber << '\n';
    cout << "Owner Name: " << ownerName << '\n';
    cout << "Slot Type: " << invertSC(chosenSlot) << '\n';
    cout << "Assigned UID: " << slot.uid.toString() << '\n';
    cout << "Entry Time: " << slot.inTime << '\n';
}

#endif
