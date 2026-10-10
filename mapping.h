#ifndef MAPPING_H
#define MAPPING_H

#include <bits/stdc++.h>
#include <filesystem>
#include <ctime>
#include <sstream>

#include "helper.h"
#include "uid.h"

using namespace std;

string getCurrentTime()
{

    time_t now = time(nullptr);
    tm *local = localtime(&now);
    stringstream ss;
    ss << put_time(local, "%d-%m-%Y %H:%M:%S");
    // DD-MM-YYYY HH:MM:SS
    return ss.str(); // DD-MM-YYY Y  H  H  :  M  M  :  S  S
                     // 123456789 10 11 12 13 14 15 16 17 18
}
class ParkingSlot
{
public:
    UID uid;
    int area;
    bool occupied = false;
    string plateNumber;
    bool paid;
    double amountToPay ;
    string inTime;
    string outTime;

    ParkingSlot(UID id, int a) : uid(id), area(a)
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

        int columns = floorMap[0].size();

        for (auto row : floorMap)
        {
            if (row.size() != columns)
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

    vector<vector<int>> counters(binaryMap.size(), vector<int>(4, 0));

    for (int f = 0; f < binaryMap.size(); f++)
    {
        const auto &mat = binaryMap[f];

        vector<vector<bool>> visited(
            mat.size(),
            vector<bool>(mat[0].size(), false));

        for (int r = 0; r < mat.size(); r++)
        {
            for (int c = 0; c < mat[r].size(); c++)
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

                int index = parkingSlots.size();

                parkingSlots.push_back({uid, area});
                slotIndex[uid.toString()] = index;
            }
        }
    }
}

#endif
