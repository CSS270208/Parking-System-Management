#include <bits/stdc++.h>
#include <filesystem>
#include <ctime>
#include <sstream>

using namespace std;

// ============================================================
// 1. HELPER FUNCTIONS
// ============================================================

inline void toLowerString(string &s)
{
    for (char &c : s)
    {
        c = tolower(static_cast<unsigned char>(c));
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

// ============================================================
// 2. UID CLASS
// ============================================================

class UID
{
private:
    int floor;
    int slotType;
    int slotNumber;

public:
    UID(int floor, int slotType, int slotNumber)
        : floor(floor),
          slotType(slotType),
          slotNumber(slotNumber)
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
        return "F" + to_string(floor) +
               "-T" + to_string(slotType) +
               "-S" + to_string(slotNumber);
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

// ============================================================
// 3. PARKING SLOT AND MAP MANAGEMENT
// ============================================================

string getCurrentTime()
{
    time_t now = time(nullptr);

    tm *local = localtime(&now);

    stringstream ss;

    ss << put_time(local, "%d-%m-%Y %H:%M:%S");

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
                    cout << "Invalid map value in "
                         << file << '\n';

                    return false;
                }

                row.push_back(value);
            }

            if (!row.empty())
                floorMap.push_back(row);
        }

        if (floorMap.empty())
            return false;

        int columns = floorMap[0].size();

        for (const auto &row : floorMap)
        {
            if (row.size() != columns)
            {
                cout << "Inconsistent row width in "
                     << file << '\n';

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
    int rows = mat.size();
    int cols = mat[0].size();

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

    vector<vector<int>> counters(
        binaryMap.size(),
        vector<int>(4, 0));

    for (int f = 0; f < binaryMap.size(); f++)
    {
        const auto &mat = binaryMap[f];

        if (mat.empty() || mat[0].empty())
            continue;

        vector<vector<bool>> visited(
            mat.size(),
            vector<bool>(mat[0].size(), false));

        for (int r = 0; r < mat.size(); r++)
        {
            for (int c = 0; c < mat[r].size(); c++)
            {
                if (mat[r][c] == 0 || visited[r][c])
                    continue;

                int area = sizeOfSlot(
                    mat, r, c, visited);

                int slotType = slotTypeFromArea(area);

                if (slotType == -1)
                {
                    cout << "Unknown area: "
                         << area
                         << " on floor "
                         << f + 1 << '\n';

                    continue;
                }

                int number = ++counters[f][slotType];

                UID uid(f + 1, slotType, number);

                int index = parkingSlots.size();

                parkingSlots.push_back({uid, area});

                slotIndex[uid.toString()] = index;
            }
        }
    }
}

// ============================================================
// 4. VEHICLE CLASS
// ============================================================

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

// ============================================================
// 5. FORWARD DECLARATIONS
// ============================================================

class Admin;

void print3DMatrix(
    const vector<vector<vector<int>>> &matrix);

void showBillScheme(int slotType);

void showAllBillSchemes();

int hourCalculator(
    string inTime,
    string currentTime);

double totalBill(
    UID uid,
    int totalHours);

void paymentOption(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots);

void pay(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots);

void resetInfo(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots);

// ============================================================
// 6. PARKING MANAGEMENT SYSTEM CLASS
// ============================================================

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

public:
    void on();

    void off()
    {
        running = false;

        cout << "Parking system turned off.\n";
    }

    friend class Admin;
};

// ============================================================
// 7. BILLING SCHEME DISPLAY
// ============================================================

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
    cout << "                  PARKING BILLING SCHEME\n";
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

// ============================================================
// 8. BOOK PARKING
// ============================================================

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

        if (find(
                compatibleSlots.begin(),
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

        if (find(
                availableTypes.begin(),
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

    showBillScheme(chosenSlot);

    cout << "\nEnter owner's name: ";

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n');

    getline(cin, ownerName);

    toLowerString(ownerName);

    if (ownerName.empty())
    {
        cout << "Owner name cannot be empty.\n";

        return;
    }

    Vehicle vehicle(
        plateNumber,
        vehicleType,
        ownerName);

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

        parkingSlots[i].paid = false;

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

// ============================================================
// 9. FIND VEHICLE
// ============================================================

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

    const UID &uid = parkingSlots[slotIndex].uid;

    cout << "\n===== Vehicle Details =====\n";

    cout << "Plate Number : "
         << vehicle.getPlateNumber() << '\n';

    cout << "Owner Name   : "
         << vehicle.getOwnerName() << '\n';

    cout << "Vehicle Type : "
         << invertVC(vehicle.getVehicleType())
         << " (" << vehicle.getVehicleType() << ")\n";

    cout << "Slot Index   : "
         << slotIndex << '\n';

    cout << "\n===== Parking Slot Details =====\n";

    cout << "Floor        : "
         << uid.getFloor() << '\n';

    cout << "Slot Type    : "
         << invertSC(uid.getSlotType())
         << " (" << uid.getSlotType() << ")\n";

    cout << "Slot Number  : "
         << uid.getSlotNumber() << '\n';
}

// ============================================================
// 10. BILL CALCULATION
// ============================================================

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

    // Calculate remaining days in the entry year.
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

    // Calculate complete years between entry and exit.
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

    // Calculate days elapsed in the current year.
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

    // Calculate hours and minutes.
    totalHours += curH - inH;

    totalHours += (curMin - inMin) / 60;

    // Same-day calculation.
    if (curY == inY &&
        curM == inM &&
        curD == inD)
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

        bill += min(
            remainingBill,
            (double)dailyMaximum);
    }

    return bill;
}

// ============================================================
// 11. PAYMENT WORKFLOW
// ============================================================

void ParkingManagementSystem::payAmount()
{
    cout << "Enter your car number: ";

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

    parkingSlots[slotIndex].amountToPay =
        totalBill(uid, totalHours);

    if (parkingSlots[slotIndex].amountToPay < 0)
    {
        cout << "Unable to calculate parking bill.\n";

        return;
    }

    cout << "\n---------- PARKING BILL ----------\n";

    cout << "Vehicle Number : "
         << plateNumber << '\n';

    cout << "Parking Hours  : "
         << totalHours << '\n';

    cout << "Amount         : Rs. "
         << parkingSlots[slotIndex].amountToPay << '\n';

    cout << "----------------------------------\n\n";

    paymentOption(slotIndex, parkingSlots);
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

    parkingSlots[slotIndex].amountToPay =
        totalBill(uid, totalHours);

    if (parkingSlots[slotIndex].amountToPay < 0)
    {
        cout << "Unable to calculate parking bill.\n";

        return;
    }

    cout << "\n---------- PARKING BILL ----------\n";

    cout << "Vehicle Number : "
         << plateNumber << '\n';

    cout << "Parking Hours  : "
         << totalHours << '\n';

    cout << "Amount         : Rs. "
         << parkingSlots[slotIndex].amountToPay << '\n';

    cout << "----------------------------------\n\n";

    paymentOption(slotIndex, parkingSlots);
}

void paymentOption(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots)
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

void pay(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots)
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

    cout << "\nAmount to pay: Rs. "
         << slot.amountToPay << '\n';

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

// ============================================================
// 12. TAKE VEHICLE OUT
// ============================================================

void ParkingManagementSystem::takeVehicleOut()
{
    cout << "\n-> Take Vehicle Out Selected <-\n";

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

    cout << "Plate Number : "
         << vehicle.getPlateNumber() << '\n';

    cout << "Owner Name   : "
         << vehicle.getOwnerName() << '\n';

    cout << "Vehicle Type : "
         << invertVC(vehicle.getVehicleType()) << '\n';

    cout << "Slot Index   : "
         << slotIndex << '\n';

    resetInfo(slotIndex, parkingSlots);

    vehicle.setTheSlot(-1);
}

void resetInfo(
    int slotIndex,
    vector<ParkingSlot> &parkingSlots)
{
    parkingSlots[slotIndex].occupied = false;

    parkingSlots[slotIndex].plateNumber = "";

    parkingSlots[slotIndex].inTime = "";

    parkingSlots[slotIndex].outTime = "";

    parkingSlots[slotIndex].amountToPay = 0;

    parkingSlots[slotIndex].paid = false;

    cout << "\nThanks for coming! Be sure to visit again.\n";
}

// ============================================================
// 13. ADMIN CLASS
// ============================================================

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

        if (it != system.adminData.end() &&
            it->second == password)
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
            {
                occupied++;
            }
            else
            {
                available++;
            }
        }

        cout << "\n===== Admin Dashboard =====\n";

        cout << "Total slots: "
             << system.parkingSlots.size() << '\n';

        cout << "Available slots: "
             << available << '\n';

        cout << "Occupied slots: "
             << occupied << '\n';
    }
};

void ParkingManagementSystem::admin()
{
    Admin::login(*this);
}

// ============================================================
// 14. PRINT PARKING MAP
// ============================================================

void print3DMatrix(
    const vector<vector<vector<int>>> &matrix)
{
    for (int f = 0; f < matrix.size(); f++)
    {
        cout << "\nFloor " << f + 1 << ":\n";

        for (int r = 0; r < matrix[f].size(); r++)
        {
            for (int c = 0; c < matrix[f][r].size(); c++)
            {
                cout << matrix[f][r][c] << ' ';
            }

            cout << '\n';
        }
    }
}

// ============================================================
// 15. MAIN PARKING SYSTEM LOOP
// ============================================================

void ParkingManagementSystem::on()
{
    if (!loadMap(binaryMap))
    {
        cerr << "Failed to load parking maps.\n";

        return;
    }

    scanMap(
        binaryMap,
        parkingSlots,
        slotIndex);

    cout << "\nDetected physical slots: "
         << parkingSlots.size() << '\n';

    running = true;

    cout << "Welcome to CSS Parking Station!\n";

    while (running)
    {
        showMenu();

        int choice;

        cin >> choice;

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
}

// ============================================================
// 16. MAIN FUNCTION
// ============================================================

int main()
{
    ParkingManagementSystem parkingSystem;

    parkingSystem.on();

    return 0;
}
