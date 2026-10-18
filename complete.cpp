#include <bits/stdc++.h>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

// UID.H — UNIQUE IDENTIFIER FOR A PHYSICAL PARKING SLOT

class UID
{
private:
    int floor;
    int slotType;
    int slotNumber;

public:
    UID(int floor, int slotType, int slotNumber)
        : floor(floor), slotType(slotType), slotNumber(slotNumber)
    {
    }

    int getFloor() const
    {
        return floor;
    }

    int getSlotType() const
    {
        return slotType;
    }

    int getSlotNumber() const
    {
        return slotNumber;
    }

    string toString() const
    {
        return "F" + to_string(floor) + "-T" + to_string(slotType) + "-S" + to_string(slotNumber);
    }

    bool operator==(const UID &other) const
    {
        return floor == other.floor &&
               slotType == other.slotType &&
               slotNumber == other.slotNumber;
    }

    friend ostream &operator<<(ostream &out, const UID &obj)
    {
        out << "F" << obj.floor
            << "-T" << obj.slotType
            << "-S" << obj.slotNumber;

        return out;
    }
};

// HELPER.H — VEHICLE AND SLOT TYPE MAPPINGS

inline void toLowerString(string &s)
{
    for (char &c : s)
    {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
}

inline int VC(const string &type)
{
    if (type == "bike")
        return 0;

    if (type == "car")
        return 1;

    if (type == "truck")
        return 2;

    if (type == "ev")
        return 3;

    return -1;
}

inline string invertVC(int x)
{
    if (x == 0)
        return "bike";

    if (x == 1)
        return "car";

    if (x == 2)
        return "truck";

    if (x == 3)
        return "ev";

    return "unknown";
}

inline int SC(const string &s)
{
    if (s == "bike")
        return 0;

    if (s == "compact")
        return 1;

    if (s == "large")
        return 2;

    if (s == "ev")
        return 3;

    return -1;
}

inline string invertSC(int x)
{
    if (x == 0)
        return "bike";

    if (x == 1)
        return "compact";

    if (x == 2)
        return "large";

    if (x == 3)
        return "ev";

    return "unknown";
}

inline vector<int> VehicleToSlotsCode(int code)
{
    if (code == 0)
        return {0, 1};

    if (code == 1)
        return {1};

    if (code == 2)
        return {2};

    if (code == 3)
        return {2, 3};

    return {};
}

inline int slotTypeFromArea(int area)
{
    if (area == 2)
        return 0;

    if (area == 6)
        return 1;

    if (area == 10)
        return 2;

    return -1;
}

inline vector<int> VehicleToSlotsString(const string &type)
{
    return VehicleToSlotsCode(VC(type));
}

// VEHICLE CLASS

class Vehicle
{
private:
    string plateNumber;
    int vehicleType;
    string ownerName;
    int slot = -1;

public:
    Vehicle(string plate, int type, string owner)
        : plateNumber(plate),
          vehicleType(type),
          ownerName(owner)
    {
    }

    string getPlateNumber() const
    {
        return plateNumber;
    }

    int getVehicleType() const
    {
        return vehicleType;
    }

    string getOwnerName() const
    {
        return ownerName;
    }

    int getSlot() const
    {
        return slot;
    }

    void setTheSlot(int code)
    {
        slot = code;
    }
};

// MAPPING.H — PARKING SLOT AND MAP FUNCTIONS

inline string getCurrentTime()
{
    time_t now = time(nullptr);
    tm *local = localtime(&now);

    if (local == nullptr)
        return "";

    stringstream ss;
    ss << put_time(local, "%d-%m-%Y %H:%M:%S");

    // Format: DD-MM-YYYY HH:MM:SS
    return ss.str();
}

class ParkingSlot
{
public:
    UID uid;
    int area;

    bool occupied = false;
    bool paid = false;

    string plateNumber;

    double amountToPay = 0.0;

    string inTime;
    string outTime;

    ParkingSlot(UID id, int a)
        : uid(id), area(a)
    {
    }
};

inline bool loadMap(vector<vector<vector<int>>> &binaryMap)
{
    binaryMap.clear();

    int floor = 1;

    while (true)
    {
        string file = "Maps/floor" + to_string(floor) + ".txt";

        ifstream hin(file);

        if (!hin)
            break;

        vector<vector<int>> floorMap;
        string line;

        while (getline(hin, line))
        {
            stringstream ss(line);
            vector<int> row;
            int value;

            while (ss >> value)
            {
                if (value != 0 && value != 1)
                {
                    cout << "Invalid map value in " << file << '\n';
                    return false;
                }

                row.push_back(value);
            }

            if (!row.empty())
                floorMap.push_back(row);
        }

        if (floorMap.empty())
            return false;

        int columns = static_cast<int>(floorMap[0].size());

        for (const auto &row : floorMap)
        {
            if (static_cast<int>(row.size()) != columns)
            {
                cout << "Inconsistent row width in " << file << '\n';
                return false;
            }
        }

        binaryMap.push_back(floorMap);
        floor++;
    }

    return !binaryMap.empty();
}

inline int sizeOfSlot(
    const vector<vector<int>> &mat,
    int sr,
    int sc,
    vector<vector<bool>> &visited)
{
    if (mat.empty() || mat[0].empty())
        return 0;

    int rows = static_cast<int>(mat.size());
    int cols = static_cast<int>(mat[0].size());

    queue<pair<int, int>> q;

    q.push({sr, sc});
    visited[sr][sc] = true;

    int count = 1;

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};

    while (!q.empty())
    {
        auto [r, c] = q.front();
        q.pop();

        for (int i = 0; i < 4; i++)
        {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < rows &&
                nc >= 0 && nc < cols &&
                mat[nr][nc] == 1 &&
                !visited[nr][nc])
            {
                visited[nr][nc] = true;
                q.push({nr, nc});
                count++;
            }
        }
    }

    return count;
}

inline void scanMap(
    const vector<vector<vector<int>>> &binaryMap,
    vector<ParkingSlot> &parkingSlots,
    unordered_map<string, int> &slotIndex)
{
    parkingSlots.clear();
    slotIndex.clear();

    vector<vector<int>> counters(binaryMap.size(), vector<int>(4, 0));

    for (int f = 0; f < static_cast<int>(binaryMap.size()); f++)
    {
        const auto &mat = binaryMap[f];

        if (mat.empty() || mat[0].empty())
            continue;

        vector<vector<bool>> visited(
            mat.size(),
            vector<bool>(mat[0].size(), false));

        for (int r = 0; r < static_cast<int>(mat.size()); r++)
        {
            for (int c = 0; c < static_cast<int>(mat[r].size()); c++)
            {
                if (mat[r][c] == 0 || visited[r][c])
                    continue;

                int area = sizeOfSlot(mat, r, c, visited);
                int slotType = slotTypeFromArea(area);

                if (slotType == -1)
                {
                    cout << "Unknown area: " << area
                         << " on floor " << f + 1 << '\n';
                    continue;
                }

                int number = ++counters[f][slotType];
                UID uid(f + 1, slotType, number);

                int index = static_cast<int>(parkingSlots.size());

                parkingSlots.push_back({uid, area});
                slotIndex[uid.toString()] = index;
            }
        }
    }
}

// FORWARD DECLARATIONS

class ParkingManagementSystem;
class Admin;

void print3DMatrix(const vector<vector<vector<int>>> &matrix);
void showBillScheme(int slotType);
void showAllBillSchemes();
int hourCalculator(string inTime, string currentTime);
double totalBill(UID uid, int totalHours);

// FILEMANAGER.H — TEXT FILE PERSISTENCE

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
    static bool loadVehicles(unordered_map<string, Vehicle> &vehicles);
    static bool saveVehicles(const unordered_map<string, Vehicle> &vehicles);

    static bool loadActiveSessions(
        vector<ParkingSlot> &parkingSlots,
        unordered_map<string, Vehicle> &vehicles);

    static bool saveActiveSessions(const vector<ParkingSlot> &parkingSlots);

    static bool appendHistory(
        const string &plateNumber,
        const UID &uid,
        const string &inTime,
        const string &outTime);

    static bool appendPayment(
        const string &plateNumber,
        const UID &uid,
        const string &inTime,
        const string &paymentTime,
        double amount);

    static void searchHistory(const string &plateNumber);
    static void searchPayments(const string &plateNumber);
};

inline bool FileManager::saveVehicles(
    const unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ofstream file(vehiclesFile());

    if (!file)
    {
        cerr << "Error: Cannot open vehicles.txt for writing.\n";
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
        cerr << "Error: Failed while saving vehicles.\n";
        return false;
    }

    return true;
}

inline bool FileManager::loadVehicles(
    unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ifstream file(vehiclesFile());

    if (!file)
    {
        ofstream createFile(vehiclesFile());

        if (!createFile)
        {
            cerr << "Error: Cannot create vehicles.txt.\n";
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
            cerr << "Warning: Invalid vehicle record skipped.\n";
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
                cerr << "Warning: Invalid vehicle record skipped.\n";
                continue;
            }

            Vehicle vehicle(plate, vehicleType, ownerName);
            vehicles.emplace(plate, vehicle);
        }
        catch (const exception &)
        {
            cerr << "Warning: Invalid vehicle record skipped.\n";
        }
    }

    return !file.bad();
}

inline bool FileManager::saveActiveSessions(
    const vector<ParkingSlot> &parkingSlots)
{
    ensureDataDirectory();

    ofstream file(sessionsFile());

    if (!file)
    {
        cerr << "Error: Cannot open active_sessions.txt.\n";
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
        cerr << "Error: Failed while saving active sessions.\n";
        return false;
    }

    return true;
}

inline bool FileManager::loadActiveSessions(
    vector<ParkingSlot> &parkingSlots,
    unordered_map<string, Vehicle> &vehicles)
{
    ensureDataDirectory();

    ifstream file(sessionsFile());

    if (!file)
    {
        ofstream createFile(sessionsFile());

        if (!createFile)
        {
            cerr << "Error: Cannot create active_sessions.txt.\n";
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
            cerr << "Warning: Invalid session record skipped.\n";
            continue;
        }

        try
        {
            int floor = stoi(floorString);
            int type = stoi(typeString);
            int number = stoi(numberString);
            int paid = stoi(paidString);
            double amount = stod(amountString);

            if (plate.empty() || floor < 1 || type < 0 || type > 3 ||
                number < 1 || (paid != 0 && paid != 1) ||
                amount < 0 || inTime.empty())
            {
                cerr << "Warning: Invalid session record skipped.\n";
                continue;
            }

            if (restoredVehicles.count(plate))
            {
                cerr << "Warning: Duplicate active session skipped.\n";
                continue;
            }

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
                cerr << "Warning: Saved slot no longer exists: "
                     << plate << '\n';
                continue;
            }

            if (parkingSlots[index].occupied)
            {
                cerr << "Warning: Slot already occupied; session skipped.\n";
                continue;
            }

            auto vehicleIt = vehicles.find(plate);

            if (vehicleIt == vehicles.end())
            {
                cerr << "Warning: No registered vehicle for session: "
                     << plate << '\n';
                continue;
            }

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
            cerr << "Warning: Invalid session record skipped.\n";
        }
    }

    return !file.bad();
}

inline bool FileManager::appendHistory(
    const string &plateNumber,
    const UID &uid,
    const string &inTime,
    const string &outTime)
{
    ensureDataDirectory();

    ofstream file(historyFile(), ios::out | ios::app);

    if (!file)
    {
        cerr << "Error: Cannot open vehicle_history.txt.\n";
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
        cerr << "Error: Failed to append vehicle history.\n";
        return false;
    }

    return true;
}

inline bool FileManager::appendPayment(
    const string &plateNumber,
    const UID &uid,
    const string &inTime,
    const string &paymentTime,
    double amount)
{
    ensureDataDirectory();

    ofstream file(paymentsFile(), ios::out | ios::app);

    if (!file)
    {
        cerr << "Error: Cannot open payments.txt.\n";
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
        cerr << "Error: Failed to append payment record.\n";
        return false;
    }

    return true;
}

inline void FileManager::searchHistory(const string &plateNumber)
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

inline void FileManager::searchPayments(const string &plateNumber)
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

// PARKING MANAGEMENT SYSTEM CLASS DECLARATION

class ParkingManagementSystem
{
private:
    bool running = false;

    vector<vector<vector<int>>> binaryMap;
    vector<ParkingSlot> parkingSlots;

    unordered_map<string, int> slotIndex;
    unordered_map<string, Vehicle> vehicles;

    inline static unordered_map<string, string> adminData = {
        {"CSS", "ChanduChai"}};

    void showMenu()
    {
        cout << "\n===== CSS Parking Station =====\n";
        cout << "1. Book a new parking\n";
        cout << "2. Find my vehicle\n";
        cout << "3. Take my vehicle out\n";
        cout << "4. Pay the current amount\n";
        cout << "5. Admin Control\n";
        cout << "6. See Parking Cost\n";
        cout << "7. Add/Change Maps\n";
        cout << "8. Turn off\n";
        cout << "Choose an option: ";
    }

    void bookParking();
    void findVehicle();
    void takeVehicleOut();
    void payAmount();
    void payAmount(string plateNumber);
    void admin();

    void loadPersistentData()
    {
        if (!FileManager::loadVehicles(vehicles))
        {
            cerr << "Warning: Could not load registered vehicles.\n";
        }

        if (!FileManager::loadActiveSessions(parkingSlots, vehicles))
        {
            cerr << "Warning: Could not restore active sessions.\n";
        }
    }

    void savePersistentData()
    {
        if (!FileManager::saveVehicles(vehicles))
        {
            cerr << "Warning: Could not save vehicles.\n";
        }

        if (!FileManager::saveActiveSessions(parkingSlots))
        {
            cerr << "Warning: Could not save active sessions.\n";
        }
    }

public:
    void on()
    {
        if (!loadMap(binaryMap))
        {
            cerr << "Failed to load parking maps.\n";
            return;
        }

        scanMap(binaryMap, parkingSlots, slotIndex);

        cout << "\nDetected physical slots: "
             << parkingSlots.size() << '\n';

        loadPersistentData();

        int occupied = 0;

        for (const ParkingSlot &slot : parkingSlots)
        {
            if (slot.occupied)
                occupied++;
        }

        cout << "Restored active sessions: "
             << occupied << '\n';

        running = true;

        cout << "Welcome to CSS Parking Station!\n";

        while (running)
        {
            showMenu();

            int choice;

            if (!(cin >> choice))
            {
                if (cin.eof())
                    break;

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Invalid input. Enter a number.\n";
                continue;
            }

            switch (choice)
            {
            case 1:
                bookParking();
                break;

            case 2:
                findVehicle();
                break;

            case 3:
                takeVehicleOut();
                break;

            case 4:
                payAmount();
                break;

            case 5:
                admin();
                break;

            case 6:
                showAllBillSchemes();
                break;

            case 7:
                cout << "Map modification is not implemented yet.\n";
                break;

            case 8:
                off();
                break;

            default:
                cout << "Please choose a valid option.\n";
            }
        }

        savePersistentData();
    }

    void off()
    {
        running = false;
        cout << "Parking system turned off.\n";
    }

    friend class Admin;
    friend class FileManager;
};

// BOOKPARKING.H — BOOK A PARKING SLOT

void ParkingManagementSystem::bookParking()
{
    cout << "\n-> Book Parking Selected <-\n";

    string plateNumber;

    cout << "Enter plate number: ";
    cin >> plateNumber;
    toLowerString(plateNumber);

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

        vector<int> compatibleSlots = VehicleToSlotsCode(vehicleType);

        cout << "\nCompatible Slot Types:\n";

        for (int type : compatibleSlots)
        {
            cout << invertSC(type) << ' ';
        }

        cout << "\nEnter slot dimensions/type (bike/compact/large/ev): ";

        string temp;
        cin >> temp;
        toLowerString(temp);

        chosenSlot = SC(temp);

        if (find(compatibleSlots.begin(), compatibleSlots.end(), chosenSlot) == compatibleSlots.end())
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
                find(availableTypes.begin(), availableTypes.end(), type) == availableTypes.end())
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

        cout << "\nEnter slot dimensions/type (bike/compact/large/ev): ";

        string temp;
        cin >> temp;
        toLowerString(temp);

        chosenSlot = SC(temp);

        if (find(availableTypes.begin(), availableTypes.end(), chosenSlot) == availableTypes.end())
        {
            cout << "Invalid or unavailable slot dimensions.\n";
            return;
        }

        vehicleType = -1;
        break;
    }

    default:
        cout << "Invalid booking choice.\n";
        return;
    }

    showBillScheme(chosenSlot);

    cout << "\nEnter owner's name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, ownerName);
    toLowerString(ownerName);

    if (ownerName.empty())
    {
        cout << "Owner name cannot be empty.\n";
        return;
    }

    Vehicle vehicle(plateNumber, vehicleType, ownerName);

    cout << "\nVehicle Details:\n";
    cout << "Plate Number: " << vehicle.getPlateNumber() << '\n';

    if (vehicleType != -1)
    {
        cout << "Vehicle Type: " << invertVC(vehicle.getVehicleType()) << '\n';
    }
    else
    {
        cout << "Vehicle Type: Not specified\n";
    }

    cout << "Owner Name: " << vehicle.getOwnerName() << '\n';
    cout << "Selected Slot Type: " << invertSC(chosenSlot) << '\n';

    for (int i = 0; i < static_cast<int>(parkingSlots.size()); i++)
    {
        if (parkingSlots[i].uid.getSlotType() != chosenSlot)
            continue;

        if (parkingSlots[i].occupied)
            continue;

        parkingSlots[i].occupied = true;
        parkingSlots[i].plateNumber = plateNumber;
        parkingSlots[i].inTime = getCurrentTime();
        parkingSlots[i].amountToPay = 0.0;
        parkingSlots[i].paid = false;

        vehicle.setTheSlot(i);

        auto result = vehicles.emplace(plateNumber, vehicle);

        if (!result.second)
        {
            parkingSlots[i].occupied = false;
            parkingSlots[i].plateNumber.clear();
            parkingSlots[i].inTime.clear();

            cout << "Vehicle registration failed.\n";
            return;
        }

        bool vehiclesSaved = FileManager::saveVehicles(vehicles);
        bool sessionsSaved = FileManager::saveActiveSessions(parkingSlots);

        if (!vehiclesSaved || !sessionsSaved)
        {
            cout << "Warning: Parking data could not be fully saved.\n";
        }

        cout << "\nParking successful.\n";
        cout << "Assigned UID: " << parkingSlots[i].uid.toString() << '\n';
        cout << "Entry Time: " << parkingSlots[i].inTime << '\n';

        return;
    }

    cout << "No available physical slot of this type.\n";
}

// FINDVEHICLE.H — FIND A REGISTERED, CURRENTLY PARKED VEHICLE

void ParkingManagementSystem::findVehicle()
{
    cout << "\nEnter your vehicle Number: ";

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
    int slotIndexFound = -1;

    for (int i = 0; i < static_cast<int>(parkingSlots.size()); i++)
    {
        if (parkingSlots[i].occupied && parkingSlots[i].plateNumber == plateNumber)
        {
            slotIndexFound = i;
            break;
        }
    }

    if (slotIndexFound == -1)
    {
        cout << "Vehicle is registered but no occupied parking slot was found.\n";
        return;
    }

    const UID &uid = parkingSlots[slotIndexFound].uid;

    cout << "\n===== Vehicle Details =====\n";
    cout << "Plate Number : " << vehicle.getPlateNumber() << '\n';
    cout << "Owner Name   : " << vehicle.getOwnerName() << '\n';
    cout << "Vehicle Type : " << invertVC(vehicle.getVehicleType())
         << " (" << vehicle.getVehicleType() << ")\n";
    cout << "Slot Index   : " << slotIndexFound << '\n';

    cout << "\n===== Parking Slot Details =====\n";
    cout << "Floor        : " << uid.getFloor() << '\n';
    cout << "Slot Type    : " << invertSC(uid.getSlotType())
         << " (" << uid.getSlotType() << ")\n";
    cout << "Slot Number  : " << uid.getSlotNumber() << '\n';
}

// PAYAMOUNT.H — BILL CALCULATION AND PAYMENT

int hourCalculator(string inTime, string currentTime)
{
    // The timestamps use DD-MM-YYYY HH:MM:SS.
    tm entry = {};
    tm current = {};

    istringstream entryStream(inTime);
    istringstream currentStream(currentTime);

    entryStream >> get_time(&entry, "%d-%m-%Y %H:%M:%S");
    currentStream >> get_time(&current, "%d-%m-%Y %H:%M:%S");

    if (entryStream.fail() || currentStream.fail())
        return -1;

    entry.tm_isdst = -1;
    current.tm_isdst = -1;

    time_t entryTime = mktime(&entry);
    time_t currentTimeValue = mktime(&current);

    if (entryTime == static_cast<time_t>(-1) ||
        currentTimeValue == static_cast<time_t>(-1) ||
        currentTimeValue < entryTime)
    {
        return -1;
    }

    double elapsedSeconds = difftime(currentTimeValue, entryTime);
    long long elapsedHours = static_cast<long long>(ceil(elapsedSeconds / 3600.0));

    if (elapsedHours > numeric_limits<int>::max())
        return -1;

    return static_cast<int>(elapsedHours);
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
            remainingBill += (remainingHours - 1) * additionalHourRate;
        }

        bill += min(remainingBill, static_cast<double>(dailyMaximum));
    }

    return bill;
}

void paymentOption(int slotIndex, vector<ParkingSlot> &parkingSlots);
void pay(int slotIndex, vector<ParkingSlot> &parkingSlots);

void ParkingManagementSystem::payAmount()
{
    cout << "Enter your Vehicle number: ";

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
        cout << "Vehicle Not Found\n";
        return;
    }

    int slotIndexFound = -1;

    for (int i = 0; i < static_cast<int>(parkingSlots.size()); i++)
    {
        if (parkingSlots[i].occupied && parkingSlots[i].plateNumber == plateNumber)
        {
            slotIndexFound = i;
            break;
        }
    }

    if (slotIndexFound == -1)
    {
        cout << "Vehicle is registered but no occupied parking slot was found.\n";
        return;
    }

    if (parkingSlots[slotIndexFound].paid)
    {
        cout << "Payment already completed.\n";
        return;
    }

    UID uid = parkingSlots[slotIndexFound].uid;
    string currentTime = getCurrentTime();

    int totalHours = hourCalculator(parkingSlots[slotIndexFound].inTime, currentTime);

    if (totalHours < 0)
    {
        cout << "Unable to calculate parking duration. Check the saved timestamps.\n";
        return;
    }

    parkingSlots[slotIndexFound].amountToPay = totalBill(uid, totalHours);

    if (parkingSlots[slotIndexFound].amountToPay < 0)
    {
        cout << "Unable to calculate parking bill.\n";
        return;
    }

    cout << "\n---------- PARKING BILL ----------\n";
    cout << "Vehicle Number : " << plateNumber << '\n';
    cout << "Entry Time     : " << parkingSlots[slotIndexFound].inTime << '\n';
    cout << "Current Time   : " << currentTime << '\n';
    cout << "Parking Hours  : " << totalHours << '\n';
    cout << "Amount         : Rs. " << fixed << setprecision(2)
         << parkingSlots[slotIndexFound].amountToPay << '\n';
    cout << "----------------------------------\n\n";

    if (!FileManager::saveActiveSessions(parkingSlots))
    {
        cout << "Warning: Could not save the latest calculated amount.\n";
    }

    paymentOption(slotIndexFound, parkingSlots);
}

void paymentOption(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    cout << "\n1. Pay the amount\n";
    cout << "2. <- Go Back\n";
    cout << "Choose an option: ";

    int x;

    if (!(cin >> x))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }

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
    if (slotIndex < 0 || slotIndex >= static_cast<int>(parkingSlots.size()))
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















































































































    if (slot.paid)
    {
        cout << "Payment already completed.\n";
        return;
    }

    if (slot.amountToPay < 0)
    {
        cout << "Invalid payment amount.\n";
        return;
    }

    cout << "\nAmount to pay: Rs. "
         << fixed << setprecision(2)
         << slot.amountToPay << '\n';

    cout << "Confirm payment? (1 = Yes, 0 = No): ";

    int choice;

    if (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }

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
        cout << "Payment could not be recorded. Please try again.\n";
        return;
    }

    slot.paid = true;

    if (!FileManager::saveActiveSessions(parkingSlots))
    {
        // The payment record exists, but the active session may not have
        // persisted the paid flag. Report this instead of hiding the issue.
        cout << "Warning: Payment was recorded, but the paid status could not be saved.\n";
    }

    cout << "Payment successful!\n";
    cout << "Payment Time: " << paymentTime << '\n';
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
    cout << '\n';
    cout << "====================================================================\n";
    cout << "                  PARKING BILLING SCHEME                           \n";
    cout << "====================================================================\n";

    cout << left
         << setw(18) << "Slot Type"
         << setw(18) << "First Hour"
         << setw(18) << "Additional Hour"
         << "Daily Maximum\n";

    cout << "--------------------------------------------------------------------\n";

    cout << left << setw(18) << invertSC(0)
         << setw(18) << "Rs. 10"
         << setw(18) << "Rs. 5"
         << "Rs. 60\n";

    cout << left << setw(18) << invertSC(1)
         << setw(18) << "Rs. 30"
         << setw(18) << "Rs. 20"
         << "Rs. 300\n";

    cout << left << setw(18) << invertSC(2)
         << setw(18) << "Rs. 60"
         << setw(18) << "Rs. 40"
         << "Rs. 600\n";

    cout << left << setw(18) << invertSC(3)
         << setw(18) << "Rs. 30"
         << setw(18) << "Rs. 20"
         << "Rs. 300\n";

    cout << "====================================================================\n\n";
}

// TAKEVEHICLEOUT.H — CHECK OUT A VEHICLE

void resetInfo(int slotIndex, vector<ParkingSlot> &parkingSlots)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(parkingSlots.size()))
        return;

    parkingSlots[slotIndex].occupied = false;
    parkingSlots[slotIndex].plateNumber.clear();
    parkingSlots[slotIndex].inTime.clear();
    parkingSlots[slotIndex].amountToPay = 0.0;
    parkingSlots[slotIndex].paid = false;

    cout << "\nThanks for coming! Be sure to visit again.\n";
}

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

    for (int i = 0; i < static_cast<int>(parkingSlots.size()); i++)
    {
        if (parkingSlots[i].occupied && parkingSlots[i].plateNumber == plateNumber)
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
    cout << "Plate Number : " << it->second.getPlateNumber() << '\n';
    cout << "Owner Name   : " << it->second.getOwnerName() << '\n';
    cout << "Vehicle Type : " << invertVC(it->second.getVehicleType()) << '\n';
    cout << "Floor        : " << slot.uid.getFloor() << '\n';
    cout << "Slot Number  : " << slot.uid.getSlotNumber() << '\n';
    cout << "Entry Time   : " << slot.inTime << '\n';
    cout << "Exit Time    : " << outTime << '\n';

    slot.occupied = false;
    slot.plateNumber.clear();
    slot.inTime.clear();
    slot.outTime = outTime;
    slot.amountToPay = 0.0;
    slot.paid = false;

    it->second.setTheSlot(-1);

    bool vehiclesSaved = FileManager::saveVehicles(vehicles);
    bool sessionsSaved = FileManager::saveActiveSessions(parkingSlots);

    if (!vehiclesSaved || !sessionsSaved)
    {
        cout << "Warning: Failed to save the latest parking state.\n";
    }

    cout << "\nThanks for coming! Be sure to visit again.\n";
}

// ADMIN.H — ADMIN LOGIN AND DASHBOARD

class Admin
{
public:
    static void login(ParkingManagementSystem &system)
    {
        string username;
        string password;

        cout << "\nUsername: ";
        cin >> username;

        cout << "Password: ";
        cin >> password;

        auto it = system.adminData.find(username);

        if (it != system.adminData.end() && it->second == password)
        {
            cout << "Admin login successful.\n";
            control(system);
        }
        else
        {
            cout << "Invalid username or password.\n";
        }
    }

    static void control(ParkingManagementSystem &system)
    {
        int available = 0;
        int occupied = 0;

        for (const ParkingSlot &slot : system.parkingSlots)
        {
            if (slot.occupied)
                occupied++;
            else
                available++;
        }

        cout << "\n===== Admin Dashboard =====\n";
        cout << "Total slots: " << system.parkingSlots.size() << '\n';
        cout << "Available slots: " << available << '\n';
        cout << "Occupied slots: " << occupied << '\n';

        cout << "\n1. Search vehicle history\n";
        cout << "2. Search payment history\n";
        cout << "3. Back\n";
        cout << "Choose an option: ";

        int choice;

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            return;
        }

        if (choice == 1 || choice == 2)
        {
            string plateNumber;
            cout << "Enter vehicle number: ";
            cin >> plateNumber;
            toLowerString(plateNumber);

            if (choice == 1)
                FileManager::searchHistory(plateNumber);
            else
                FileManager::searchPayments(plateNumber);
        }
        else if (choice != 3)
        {
            cout << "Invalid option.\n";
        }
    }
};

void ParkingManagementSystem::admin()
{
    Admin::login(*this);
}

// UTILITY FUNCTION — PRINT THE 3D PARKING MAP

void print3DMatrix(const vector<vector<vector<int>>> &matrix)
{
    for (int f = 0; f < static_cast<int>(matrix.size()); f++)
    {
        cout << "\nFloor " << f + 1 << ":\n";

        for (int r = 0; r < static_cast<int>(matrix[f].size()); r++)
        {
            for (int c = 0; c < static_cast<int>(matrix[f][r].size()); c++)
            {
                cout << matrix[f][r][c] << ' ';
            }

            cout << '\n';
        }
    }
}

// MAIN FUNCTION

int main()
{
    ParkingManagementSystem parkingSystem;
    parkingSystem.on();
    return 0;
}
