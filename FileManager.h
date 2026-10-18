#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <bits/stdc++.h>
#include <filesystem>

using namespace std;

class FileManager
{
private:
    static constexpr const char *DATA_DIR = "Data";

    static string vehiclesFile()
    {
        return "Data/vehicles.txt";
    }

    static string sessionsFile()
    {
        return "Data/active_sessions.txt";
    }

    static string historyFile()
    {
        return "Data/vehicle_history.txt";
    }

    static string paymentsFile()
    {
        return "Data/payments.txt";
    }

    static void ensureDataDirectory()
    {
        filesystem::create_directories(DATA_DIR);
    }

public:
    // Registered vehicles
    static bool loadVehicles(
        unordered_map<string, Vehicle> &vehicles);

    static bool saveVehicles(
        const unordered_map<string, Vehicle> &vehicles);

    // Currently parked vehicles
    static bool loadActiveSessions(
        vector<ParkingSlot> &parkingSlots,
        unordered_map<string, Vehicle> &vehicles);

    static bool saveActiveSessions(
        const vector<ParkingSlot> &parkingSlots);

    // Completed parking sessions
    static bool appendHistory(
        const string &plateNumber,
        const UID &uid,
        const string &inTime,
        const string &outTime);

    // Successful payments
    static bool appendPayment(
        const string &plateNumber,
        const UID &uid,
        const string &inTime,
        const string &paymentTime,
        double amount);

    // Search functions
    static void searchHistory(
        const string &plateNumber);

    static void searchPayments(
        const string &plateNumber);
};

// ============================================================
// 1. SAVE REGISTERED VEHICLES
// ============================================================

inline bool FileManager::saveVehicles(
    const unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ofstream file(vehiclesFile());

    if (!file)
    {
        cerr << "Cannot open vehicles.txt for writing.\n";
        return false;
    }

    for (const auto &[plate, vehicle] : vehicles)
    {
        file << plate << '|'
             << vehicle.getVehicleType() << '|'
             << vehicle.getOwnerName() << '\n';
    }

    if (!file)
    {
        cerr << "Failed while saving vehicles.\n";
        return false;
    }

    return true;
}

// ============================================================
// 2. LOAD REGISTERED VEHICLES
// ============================================================

inline bool FileManager::loadVehicles(
    unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ifstream file(vehiclesFile());

    if (!file)
    {
        // The file might not exist on the first run.
        ofstream createFile(vehiclesFile());

        if (!createFile)
        {
            cerr << "Cannot create vehicles.txt.\n";
            return false;
        }

        return true;
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

        if (!getline(ss, plate, '|') ||
            !getline(ss, typeString, '|') ||
            !getline(ss, ownerName))
        {
            cerr << "Invalid vehicle record skipped.\n";
            continue;
        }

        try
        {
            int vehicleType = stoi(typeString);

            if (plate.empty() ||
                vehicleType < -1 ||
                vehicleType > 3 ||
                vehicles.find(plate) != vehicles.end())
            {
                cerr << "Invalid vehicle record skipped.\n";
                continue;
            }

            Vehicle vehicle(
                plate,
                vehicleType,
                ownerName);

            vehicles.emplace(plate, vehicle);
        }
        catch (const exception &)
        {
            cerr << "Invalid vehicle record skipped.\n";
        }
    }

    return !file.bad();
}

// ============================================================
// 3. SAVE ACTIVE PARKING SESSIONS
// ============================================================

inline bool FileManager::saveActiveSessions(
    const vector<ParkingSlot> &parkingSlots)
{
    ensureDataDirectory();

    ofstream file(sessionsFile());

    if (!file)
    {
        cerr << "Cannot open active_sessions.txt.\n";
        return false;
    }

    file << fixed << setprecision(2);

    for (const ParkingSlot &slot : parkingSlots)
    {
        if (!slot.occupied)
            continue;

        file << slot.plateNumber << '|'
             << slot.uid.getFloor() << '|'
             << slot.uid.getSlotType() << '|'
             << slot.uid.getSlotNumber() << '|'
             << slot.inTime << '|'
             << slot.paid << '|'
             << slot.amountToPay << '\n';
    }

    if (!file)
    {
        cerr << "Failed while saving active sessions.\n";
        return false;
    }

    return true;
}

// ============================================================
// 4. RESTORE ACTIVE PARKING SESSIONS
// ============================================================

inline bool FileManager::loadActiveSessions(
    vector<ParkingSlot> &parkingSlots,
    unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ifstream file(sessionsFile());

    if (!file)
    {
        // No previous sessions on the first run.
        ofstream createFile(sessionsFile());

        if (!createFile)
        {
            cerr << "Cannot create active_sessions.txt.\n";
            return false;
        }

        return true;
    }

    string line;

    unordered_set<string> restoredVehicles;

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

        if (!getline(ss, plate, '|') ||
            !getline(ss, floorString, '|') ||
            !getline(ss, typeString, '|') ||
            !getline(ss, numberString, '|') ||
            !getline(ss, inTime, '|') ||
            !getline(ss, paidString, '|') ||
            !getline(ss, amountString))
        {
            cerr << "Invalid session record skipped.\n";
            continue;
        }

        try
        {
            int floor = stoi(floorString);
            int type = stoi(typeString);
            int number = stoi(numberString);
            int paid = stoi(paidString);
            double amount = stod(amountString);

            if (plate.empty() ||
                floor < 1 ||
                type < 0 || type > 3 ||
                number < 1 ||
                (paid != 0 && paid != 1) ||
                amount < 0 ||
                inTime.empty())
            {
                cerr << "Invalid session record skipped.\n";
                continue;
            }

            if (restoredVehicles.count(plate))
            {
                cerr << "Duplicate active session skipped.\n";
                continue;
            }

            // Find the physical slot using its UID.
            // Never rely on the previous vector index.
            int index = -1;

            for (int i = 0; i < static_cast<int>(parkingSlots.size()); i++)
            {
                const UID &uid = parkingSlots[i].uid;

                if (uid.getFloor() == floor &&
                    uid.getSlotType() == type &&
                    uid.getSlotNumber() == number)
                {
                    index = i;
                    break;
                }
            }

            if (index == -1)
            {
                cerr << "Saved slot no longer exists: "
                     << plate << '\n';
                continue;
            }

            if (parkingSlots[index].occupied)
            {
                cerr << "Slot already occupied; session skipped.\n";
                continue;
            }

            auto vehicleIt = vehicles.find(plate);

            if (vehicleIt == vehicles.end())
            {
                cerr << "No registered vehicle for session: "
                     << plate << '\n';
                continue;
            }

            // Restore the active session.
            ParkingSlot &slot = parkingSlots[index];

            slot.occupied = true;
            slot.paid = (paid == 1);
            slot.plateNumber = plate;
            slot.inTime = inTime;
            slot.amountToPay = amount;

            vehicleIt->second.setTheSlot(index);

            restoredVehicles.insert(plate);
        }
        catch (const exception &)
        {
            cerr << "Invalid session record skipped.\n";
        }
    }

    return !file.bad();
}

// ============================================================
// 5. APPEND COMPLETED PARKING HISTORY
// ============================================================

inline bool FileManager::appendHistory(
    const string &plateNumber,
    const UID &uid,
    const string &inTime,
    const string &outTime)
{
    ensureDataDirectory();

    ofstream file(
        historyFile(),
        ios::out | ios::app);

    if (!file)
    {
        cerr << "Cannot open vehicle_history.txt.\n";
        return false;
    }

    file << plateNumber << '|'
         << uid.getFloor() << '|'
         << uid.getSlotType() << '|'
         << uid.getSlotNumber() << '|'
         << inTime << '|'
         << outTime << '\n';

    if (!file)
    {
        cerr << "Failed to append vehicle history.\n";
        return false;
    }

    return true;
}

// ============================================================
// 6. APPEND SUCCESSFUL PAYMENT
// ============================================================

inline bool FileManager::appendPayment(
    const string &plateNumber,
    const UID &uid,
    const string &inTime,
    const string &paymentTime,
    double amount)
{
    ensureDataDirectory();

    ofstream file(
        paymentsFile(),
        ios::out | ios::app);

    if (!file)
    {
        cerr << "Cannot open payments.txt.\n";
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

    if (!file)
    {
        cerr << "Failed to append payment record.\n";
        return false;
    }

    return true;
}

// ============================================================
// 7. SEARCH PARKING HISTORY
// ============================================================

inline void FileManager::searchHistory(
    const string &plateNumber)
{
    ifstream file(historyFile());

    if (!file)
    {
        cout << "No vehicle history file found.\n";
        return;
    }

    string line;
    bool found = false;

    cout << "\n===== VEHICLE HISTORY =====\n";

    while (getline(file, line))
    {
        stringstream ss(line);
        string plate;

        if (!getline(ss, plate, '|'))
            continue;

        if (plate == plateNumber)
        {
            cout << line << '\n';
            found = true;
        }
    }

    if (!found)
        cout << "No history found for this vehicle.\n";
}

// ============================================================
// 8. SEARCH PAYMENTS
// ============================================================

inline void FileManager::searchPayments(
    const string &plateNumber)
{
    ifstream file(paymentsFile());

    if (!file)
    {
        cout << "No payments file found.\n";
        return;
    }

    string line;
    bool found = false;

    cout << "\n===== PAYMENT HISTORY =====\n";

    while (getline(file, line))
    {
        stringstream ss(line);
        string plate;

        if (!getline(ss, plate, '|'))
            continue;

        if (plate == plateNumber)
        {
            cout << line << '\n';
            found = true;
        }
    }

    if (!found)
        cout << "No payment records found for this vehicle.\n";
}

#endif
