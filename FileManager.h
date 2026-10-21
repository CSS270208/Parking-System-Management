#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <bits/stdc++.h>
using namespace std;

class FileManager
{
public:
    static bool saveVehicles(
        unordered_map<string, Vehicle> &vehicles)
    {
        ofstream file("Data/vehicles.txt");

        if (!file)
        {
            cout << "Unable to save vehicles.\n";
            return false;
        }

        for (auto it = vehicles.begin(); it != vehicles.end(); it++)
        {
            file << it->first << '|'
                 << it->second.getVehicleType() << '|'
                 << it->second.getOwnerName() << '\n';
        }

        file.close();
        return true;
    }

    static bool loadVehicles(
        unordered_map<string, Vehicle> &vehicles)
    {
        ifstream file("Data/vehicles.txt");

        if (!file)
        {
            ofstream createFile("Data/vehicles.txt");
            return createFile.good();
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            stringstream ss(line);

            string plate;
            string typeString;
            string ownerName;

            getline(ss, plate, '|');
            getline(ss, typeString, '|');
            getline(ss, ownerName);

            if (plate.empty() || typeString.empty())
                continue;

            int type = stoi(typeString);

            if (vehicles.find(plate) == vehicles.end())
            {
                Vehicle vehicle(plate, type, ownerName);
                vehicles.insert(make_pair(plate, vehicle));
            }
        }

        file.close();
        return true;
    }

    static bool saveActiveSessions(
        vector<ParkingSlot> &parkingSlots)
    {
        ofstream file("Data/active_sessions.txt");

        if (!file)
        {
            cout << "Unable to save active parking sessions.\n";
            return false;
        }

        for (int i = 0; i < (int)parkingSlots.size(); i++)
        {
            ParkingSlot &slot = parkingSlots[i];

            if (!slot.occupied)
                continue;

            file << slot.plateNumber << '|'
                 << slot.uid.getFloor() << '|'
                 << slot.uid.getSlotType() << '|'
                 << slot.uid.getSlotNumber() << '|'
                 << slot.inTime << '|'
                 << slot.paid << '|'
                 << slot.amountToPay << '|'
                 << slot.lastPaymentTime << '\n';
        }

        file.close();
        return true;
    }

    static bool loadActiveSessions(
        vector<ParkingSlot> &parkingSlots,
        unordered_map<string, Vehicle> &vehicles)
    {
        ifstream file("Data/active_sessions.txt");

        if (!file)
        {
            ofstream createFile("Data/active_sessions.txt");
            return createFile.good();
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            stringstream ss(line);

            string plate;
            string floorString;
            string typeString;
            string numberString;
            string inTime;
            string paidString;
            string amountString;
            string lastPaymentTime;

            getline(ss, plate, '|');
            getline(ss, floorString, '|');
            getline(ss, typeString, '|');
            getline(ss, numberString, '|');
            getline(ss, inTime, '|');
            getline(ss, paidString, '|');
            getline(ss, amountString, '|');
            getline(ss, lastPaymentTime);

            if (plate.empty() ||
                floorString.empty() ||
                typeString.empty() ||
                numberString.empty() ||
                inTime.empty() ||
                paidString.empty() ||
                amountString.empty())
            {
                continue;
            }

            int floor = stoi(floorString);
            int type = stoi(typeString);
            int number = stoi(numberString);
            int paid = stoi(paidString);
            double amount = stod(amountString);

            int index = -1;

            for (int i = 0; i < (int)parkingSlots.size(); i++)
            {
                UID uid = parkingSlots[i].uid;

                if (uid.getFloor() == floor &&
                    uid.getSlotType() == type &&
                    uid.getSlotNumber() == number)
                {
                    index = i;
                    break;
                }
            }

            if (index == -1)
                continue;

            if (parkingSlots[index].occupied)
                continue;

            auto it = vehicles.find(plate);

            if (it == vehicles.end())
                continue;

            parkingSlots[index].occupied = true;
            parkingSlots[index].plateNumber = plate;
            parkingSlots[index].inTime = inTime;
            parkingSlots[index].amountToPay = amount;
            parkingSlots[index].lastPaymentTime = lastPaymentTime;

            // Old active-session records may not contain lastPaymentTime.
            // Without it, we cannot safely assume the grace period is active.
            parkingSlots[index].paid =
                (paid == 1 && !lastPaymentTime.empty());

            it->second.setTheSlot(index);
        }

        file.close();
        return true;
    }

    static bool appendHistory(
        string plateNumber,
        UID uid,
        string inTime,
        string outTime)
    {
        ofstream file("Data/vehicle_history.txt", ios::app);

        if (!file)
        {
            cout << "Unable to save parking history.\n";
            return false;
        }

        file << plateNumber << '|'
             << uid.getFloor() << '|'
             << uid.getSlotType() << '|'
             << uid.getSlotNumber() << '|'
             << inTime << '|'
             << outTime << '\n';

        file.close();
        return true;
    }

    static bool appendPayment(
        string plateNumber,
        UID uid,
        string inTime,
        string paymentTime,
        double amount)
    {
        ofstream file("Data/payments.txt", ios::app);

        if (!file)
        {
            cout << "Unable to save payment record.\n";
            return false;
        }

        file << fixed << setprecision(2);

        file << plateNumber << '|'
             << uid.getFloor() << '|'
             << uid.getSlotType() << '|'
             << uid.getSlotNumber() << '|'
             << inTime << '|'
             << paymentTime << '|'
             << amount << '\n';

        file.close();
        return true;
    }

    static bool hasPayment(
        string plateNumber,
        UID uid,
        string inTime)
    {
        ifstream file("Data/payments.txt");

        if (!file)
            return false;

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string plate;
            string floorString;
            string typeString;
            string numberString;
            string entryTime;

            getline(ss, plate, '|');
            getline(ss, floorString, '|');
            getline(ss, typeString, '|');
            getline(ss, numberString, '|');
            getline(ss, entryTime, '|');

            if (plate == plateNumber &&
                floorString == to_string(uid.getFloor()) &&
                typeString == to_string(uid.getSlotType()) &&
                numberString == to_string(uid.getSlotNumber()) &&
                entryTime == inTime)
            {
                return true;
            }
        }

        return false;
    }

    static bool hasHistorySession(
        string plateNumber,
        UID uid,
        string inTime)
    {
        ifstream file("Data/vehicle_history.txt");

        if (!file)
            return false;

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string plate;
            string floorString;
            string typeString;
            string numberString;
            string entryTime;

            getline(ss, plate, '|');
            getline(ss, floorString, '|');
            getline(ss, typeString, '|');
            getline(ss, numberString, '|');
            getline(ss, entryTime, '|');

            if (plate == plateNumber &&
                floorString == to_string(uid.getFloor()) &&
                typeString == to_string(uid.getSlotType()) &&
                numberString == to_string(uid.getSlotNumber()) &&
                entryTime == inTime)
            {
                return true;
            }
        }

        return false;
    }
};

#endif
