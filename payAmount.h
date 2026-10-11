#ifndef PAYAMOUNT_H
#define PAYAMOUNT_H

#include <bits/stdc++.h>

#include "helper.h"
#include "uid.h"
#include "mapping.h"

using namespace std;

int hourCalculator(string inTime, string currentTime)
{
    long long totalHours = 0;

    int inD = stoi(inTime.substr(0, 2));
    int inM = stoi(inTime.substr(3, 2));
    int inY = stoi(inTime.substr(6, 4));
    int inH = stoi(inTime.substr(11, 2));
    int inMin = stoi(inTime.substr(14, 2));

    int curD = stoi(currentTime.substr(0, 2));
    int curM = stoi(currentTime.substr(3, 2));
    int curY = stoi(currentTime.substr(6, 4));
    int curH = stoi(currentTime.substr(11, 2));
    int curMin = stoi(currentTime.substr(14, 2));

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31};

    for (int m = inM; m <= 12; m++)
    {
        int days = daysInMonth[m - 1];

        if (m == 2 &&
            (inY % 400 == 0 ||
             (inY % 4 == 0 && inY % 100 != 0)))
        {
            days = 29;
        }

        if (m == inM)
            totalHours += (days - inD) * 24LL;
        else
            totalHours += days * 24LL;
    }

    for (int y = inY + 1; y < curY; y++)
    {
        if (y % 400 == 0 ||
            (y % 4 == 0 && y % 100 != 0))
        {
            totalHours += 366LL * 24;
        }
        else
        {
            totalHours += 365LL * 24;
        }
    }

    for (int m = 1; m < curM; m++)
    {
        int days = daysInMonth[m - 1];

        if (m == 2 &&
            (curY % 400 == 0 ||
             (curY % 4 == 0 && curY % 100 != 0)))
        {
            days = 29;
        }

        totalHours += days * 24LL;
    }

    totalHours += (curD - 1) * 24LL;

    totalHours += curH - inH;
    totalHours += (curMin - inMin) / 60;

    if (curY == inY && curM == inM && curD == inD)
    {
        totalHours = curH - inH;
        totalHours += (curMin - inMin) / 60;
    }

    return totalHours;
}

double totalBill(UID uid, int totalHours)
{
    int slotType = uid.getSlotType();

    int firstHourRate;
    int additionalHourRate;
    int dailyMaximum;

    if (slotType == 0)
    {
        firstHourRate = 10;
        additionalHourRate = 5;
        dailyMaximum = 60;
    }
    else if (slotType == 1)
    {
        firstHourRate = 30;
        additionalHourRate = 20;
        dailyMaximum = 300;
    }
    else if (slotType == 2)
    {
        firstHourRate = 60;
        additionalHourRate = 40;
        dailyMaximum = 600;
    }
    else if (slotType == 3)
    {
        firstHourRate = 30;
        additionalHourRate = 20;
        dailyMaximum = 300;
    }
    else
    {
        return -1;
    }

    if (totalHours < 0)
        return -1;

    if (totalHours == 0)
        totalHours = 1;

    long long fullDays = totalHours / 24;
    int remainingHours = totalHours % 24;

    double bill = fullDays * dailyMaximum;

    if (remainingHours > 0)
    {
        double remainingBill = firstHourRate;

        if (remainingHours > 1)
        {
            remainingBill +=
                (remainingHours - 1) * additionalHourRate;
        }

        bill += min(remainingBill, (double)dailyMaximum);
    }

    return bill;
}

void paymentOption(int slotIndex, vector<ParkingSlot> &parkingSlots);
void pay(int slotIndex, vector<ParkingSlot> &parkingSlots);

void ParkingManagementSystem::payAmount()
{
    cout << "Enter your car number\n";

    string plateNumber;
    cin >> plateNumber;

    toLowerString(plateNumber);

    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle Not Found\n";
        return;
    }

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

    if (parkingSlots[slotIndex].paid)
    {
        cout << "Payment already completed.\n";
        return;
    }

    UID uid = parkingSlots[slotIndex].uid;

    string currentTime = getCurrentTime();

    int totalHours = hourCalculator(
        parkingSlots[slotIndex].inTime,
        currentTime);

    parkingSlots[slotIndex].amountToPay = totalBill(uid, totalHours);

    if (parkingSlots[slotIndex].amountToPay < 0)
    {
        cout << "Unable to calculate parking bill.\n";
        return;
    }

    cout << "\n---------- PARKING BILL ----------\n";
    cout << "Vehicle Number : " << plateNumber << '\n';
    cout << "Parking Hours  : " << totalHours << '\n';
    cout << "Amount         : Rs. "
         << parkingSlots[slotIndex].amountToPay << '\n';
    cout << "----------------------------------\n\n";

    paymentOption(slotIndex, parkingSlots);
}

void ParkingManagementSystem::payAmount(string plateNumber)
{
    auto it = vehicles.find(plateNumber);

    if (it == vehicles.end())
    {
        cout << "Vehicle Not Found\n";
        return;
    }

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

    if (parkingSlots[slotIndex].paid)
    {
        cout << "Payment already completed.\n";
        return;
    }

    UID uid = parkingSlots[slotIndex].uid;

    string currentTime = getCurrentTime();

    int totalHours = hourCalculator(
        parkingSlots[slotIndex].inTime,
        currentTime);

    parkingSlots[slotIndex].amountToPay = totalBill(uid, totalHours);

    if (parkingSlots[slotIndex].amountToPay < 0)
    {
        cout << "Unable to calculate parking bill.\n";
        return;
    }

    cout << "\n---------- PARKING BILL ----------\n";
    cout << "Vehicle Number : " << plateNumber << '\n';
    cout << "Parking Hours  : " << totalHours << '\n';
    cout << "Amount         : Rs. "
         << parkingSlots[slotIndex].amountToPay << '\n';
    cout << "----------------------------------\n\n";

    paymentOption(slotIndex, parkingSlots);
}

void paymentOption(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    cout << "\n1. Pay the amount\n";
    cout << "2. <- Go Back\n";

    int x;
    cin >> x;

    switch (x)
    {
    case 1:
        pay(slotIndex, parkingSlots);
        break;

    case 2:
        return;

    default:
        cout << "Please choose a valid option.\n";
        paymentOption(slotIndex, parkingSlots);
        break;
    }
}

void pay(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    ParkingSlot &slot = parkingSlots[slotIndex];

    if (!slot.occupied)
    {
        cout << "Parking slot is not occupied.\n";
        return;
    }

    if (slot.paid)
    {
        cout << "Payment already completed.\n";
        return;
    }

    cout << "\nAmount to pay: Rs. " << slot.amountToPay << '\n';
    cout << "Confirm payment? (1 = Yes, 0 = No): ";

    int choice;
    cin >> choice;

    if (choice != 1)
    {
        cout << "Payment cancelled.\n";
        return;
    }

    slot.paid = true;

    cout << "Payment successful!\n";
    cout << "You can now take your vehicle out.\n";
}

void showBillScheme(int slotType)
{
    cout << "\n========== PARKING BILL SCHEME ==========\n";

    switch (slotType)
    {
    case 0:
        cout << "Slot Type: " << invertSC(slotType) << '\n';
        cout << "First Hour: Rs. 10\n";
        cout << "Each Additional Hour: Rs. 5\n";
        cout << "Maximum Per 24 Hours: Rs. 60\n";
        break;

    case 1:
        cout << "Slot Type: " << invertSC(slotType) << '\n';
        cout << "First Hour: Rs. 30\n";
        cout << "Each Additional Hour: Rs. 20\n";
        cout << "Maximum Per 24 Hours: Rs. 300\n";
        break;

    case 2:
        cout << "Slot Type: " << invertSC(slotType) << '\n';
        cout << "First Hour: Rs. 60\n";
        cout << "Each Additional Hour: Rs. 40\n";
        cout << "Maximum Per 24 Hours: Rs. 600\n";
        break;

    case 3:
        cout << "Slot Type: " << invertSC(slotType) << '\n';
        cout << "First Hour: Rs. 30\n";
        cout << "Each Additional Hour: Rs. 20\n";
        cout << "Maximum Per 24 Hours: Rs. 300\n";
        break;

    default:
        cout << "Billing information unavailable.\n";
        break;
    }

    cout << "==========================================\n";
}

void showAllBillSchemes()
{
    cout << "\n";
    cout << "====================================================================\n";
    cout << "          PARKING BILLING SCHEME                \n";
    cout << "====================================================================\n";

    cout << left
         << setw(18) << "Slot Type"
         << setw(18) << "First Hour"
         << setw(18) << "Additional Hour"
         << "Daily Maximum\n";

    cout << "--------------------------------------------------------------------\n";

    cout << left
         << setw(18) << invertSC(0)
         << setw(18) << "Rs. 10"
         << setw(18) << "Rs. 5"
         << "Rs. 60\n";

    cout << left
         << setw(18) << invertSC(1)
         << setw(18) << "Rs. 30"
         << setw(18) << "Rs. 20"
         << "Rs. 300\n";

    cout << left
         << setw(18) << invertSC(2)
         << setw(18) << "Rs. 60"
         << setw(18) << "Rs. 40"
         << "Rs. 600\n";

    cout << left
         << setw(18) << invertSC(3)
         << setw(18) << "Rs. 30"
         << setw(18) << "Rs. 20"
         << "Rs. 300\n";

    cout << "====================================================================\n\n";
}

#endif
