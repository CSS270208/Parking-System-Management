#ifndef PAYAMOUNT_H
#define PAYAMOUNT_H

#include <bits/stdc++.h>
using namespace std;

time_t convertTime(string value)
{
    tm timeInfo = {};

    istringstream ss(value);
    ss >> get_time(&timeInfo, "%d-%m-%Y %H:%M:%S");

    if (ss.fail())
        return (time_t)-1;

    timeInfo.tm_isdst = -1;

    return mktime(&timeInfo);
}

string addOneHour(string value)
{
    time_t timeValue = convertTime(value);

    if (timeValue == (time_t)-1)
        return "";

    timeValue += 3600;

    tm *timeInfo = localtime(&timeValue);

    if (timeInfo == nullptr)
        return "";

    stringstream ss;
    ss << put_time(timeInfo, "%d-%m-%Y %H:%M:%S");

    return ss.str();
}

int hourCalculator(string inTime, string currentTime)
{
    time_t start = convertTime(inTime);
    time_t finish = convertTime(currentTime);

    if (start == (time_t)-1 ||
        finish == (time_t)-1 ||
        finish < start)
    {
        return -1;
    }

    double seconds = difftime(finish, start);

    int hours = (int)ceil(seconds / 3600.0);

    return hours;
}

double totalBill(UID uid, int totalHours)
{
    if (totalHours < 0)
        return -1;

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

    if (totalHours == 0)
        totalHours = 1;

    int fullDays = totalHours / 24;
    int remainingHours = totalHours % 24;

    double bill = (double)fullDays * dailyMaximum;

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

void pay(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    if (slotIndex < 0 ||
        slotIndex >= (int)parkingSlots.size())
    {
        cout << "Invalid parking slot.\n";
        return;
    }

    ParkingSlot &slot = parkingSlots[slotIndex];

    if (!slot.occupied)
    {
        cout << "Parking slot is not occupied.\n";
        return;
    }

    cout << "\nAmount to pay: Rs. "
         << fixed << setprecision(2)
         << slot.amountToPay << '\n';

    cout << "Confirm payment? (1 = Yes, 0 = No): ";

    int choice;
    cin >> choice;

    if (choice != 1)
    {
        cout << "Payment cancelled.\n";
        return;
    }

    string paymentTime = getCurrentTime();

    bool recorded = FileManager::appendPayment(
        slot.plateNumber,
        slot.uid,
        slot.inTime,
        paymentTime,
        slot.amountToPay);

    if (!recorded)
    {
        cout << "Payment could not be recorded.\n";
        return;
    }

    slot.paid = true;
    slot.lastPaymentTime = paymentTime;

    if (!FileManager::saveActiveSessions(parkingSlots))
    {
        cout << "Warning: Could not save the updated parking status.\n";
        cout << "The payment record has been saved.\n";
    }

    cout << "\nPayment successful!\n";
    cout << "Payment Time: " << paymentTime << '\n';
    cout << "You have one hour to take your vehicle out.\n";
    cout << "If you stay longer, additional charges will apply.\n";
}

void paymentOption(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    cout << "\n1. Pay the amount\n";
    cout << "2. Go Back\n";
    cout << "Choose an option: ";

    int choice;
    cin >> choice;

    if (choice == 1)
    {
        pay(slotIndex, parkingSlots);
    }
    else if (choice == 2)
    {
        return;
    }
    else
    {
        cout << "Invalid choice.\n";
    }
}

void ParkingManagementSystem::payAmount()
{
    cout << "\nEnter your vehicle number: ";

    string plateNumber;
    cin >> plateNumber;

    toLowerString(plateNumber);

    payAmount(plateNumber);
}

void ParkingManagementSystem::payAmount(string plateNumber)
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

    string currentTime = getCurrentTime();
    string billingStart;

    if (!slot.lastPaymentTime.empty())
    {
        time_t lastPayment = convertTime(slot.lastPaymentTime);
        time_t current = convertTime(currentTime);

        if (lastPayment == (time_t)-1 ||
            current == (time_t)-1 ||
            current < lastPayment)
        {
            cout << "Unable to calculate the parking duration.\n";
            return;
        }

        double seconds = difftime(current, lastPayment);

        if (seconds <= 3600)
        {
            if (slot.paid)
            {
                cout << "\nPayment already completed.\n";
                cout << "Last Payment Time: "
                     << slot.lastPaymentTime << '\n';
                cout << "Grace Period Ends: "
                     << addOneHour(slot.lastPaymentTime) << '\n';
                cout << "You can leave within the grace period.\n";
                return;
            }
        }

        billingStart = addOneHour(slot.lastPaymentTime);

        if (billingStart.empty())
        {
            cout << "Unable to calculate the next billing time.\n";
            return;
        }

        slot.paid = false;
    }
    else
    {
        billingStart = slot.inTime;
    }

    time_t billingTime = convertTime(billingStart);
    time_t now = convertTime(currentTime);

    if (billingTime == (time_t)-1 ||
        now == (time_t)-1 ||
        now < billingTime)
    {
        cout << "The grace period has not ended yet.\n";
        return;
    }

    int totalHours = hourCalculator(billingStart, currentTime);

    if (totalHours < 0)
    {
        cout << "Unable to calculate parking duration.\n";
        return;
    }

    double amount = totalBill(slot.uid, totalHours);

    if (amount < 0)
    {
        cout << "Unable to calculate the bill.\n";
        return;
    }

    slot.amountToPay = amount;

    cout << "\n========== PARKING BILL ==========\n";
    cout << "Vehicle Number : " << plateNumber << '\n';
    cout << "Original Entry : " << slot.inTime << '\n';

    if (!slot.lastPaymentTime.empty())
    {
        cout << "Last Payment   : "
             << slot.lastPaymentTime << '\n';

        cout << "Billing Starts : "
             << billingStart << '\n';
    }
    else
    {
        cout << "Billing Starts : "
             << slot.inTime << '\n';
    }

    cout << "Current Time   : " << currentTime << '\n';
    cout << "Billable Hours : " << totalHours << '\n';
    cout << "Slot Type      : "
         << invertSC(slot.uid.getSlotType()) << '\n';

    cout << "Amount         : Rs. "
         << fixed << setprecision(2)
         << slot.amountToPay << '\n';

    cout << "==================================\n";

    if (!FileManager::saveActiveSessions(parkingSlots))
    {
        cout << "Warning: Could not save the calculated bill.\n";
    }

    paymentOption(index, parkingSlots);
}

void showBillScheme(int slotType)
{
    cout << "\n========== PARKING BILL SCHEME ==========\n";

    if (slotType == 0)
    {
        cout << "Slot Type: Bike\n";
        cout << "First Hour: Rs. 10\n";
        cout << "Additional Hour: Rs. 5\n";
        cout << "Daily Maximum: Rs. 60\n";
    }
    else if (slotType == 1)
    {
        cout << "Slot Type: Compact\n";
        cout << "First Hour: Rs. 30\n";
        cout << "Additional Hour: Rs. 20\n";
        cout << "Daily Maximum: Rs. 300\n";
    }
    else if (slotType == 2)
    {
        cout << "Slot Type: Large\n";
        cout << "First Hour: Rs. 60\n";
        cout << "Additional Hour: Rs. 40\n";
        cout << "Daily Maximum: Rs. 600\n";
    }
    else if (slotType == 3)
    {
        cout << "Slot Type: EV\n";
        cout << "First Hour: Rs. 30\n";
        cout << "Additional Hour: Rs. 20\n";
        cout << "Daily Maximum: Rs. 300\n";
    }
    else
    {
        cout << "Invalid slot type.\n";
    }

    cout << "==========================================\n";
}

void showAllBillSchemes()
{
    cout << "\n========== ALL BILLING SCHEMES ==========\n";

    showBillScheme(0);
    showBillScheme(1);
    showBillScheme(2);
    showBillScheme(3);
}

#endif
