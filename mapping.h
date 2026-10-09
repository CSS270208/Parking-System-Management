#ifndef MAPPING_H
#define MAPPING_H

#include <bits/stdc++.h>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

inline bool loadMap(vector<vector<vector<int>>> &binaryMap)
{
    binaryMap.clear();

    int floorNumber = 1;

    while (true)
    {
        string file = "floor" + to_string(floorNumber) + ".txt";
        fs::path filePath = fs::path("Maps") / file;

        ifstream hin(filePath);

        if (!hin)
            break;

        vector<vector<int>> floorMap;
        string line;

        while (getline(hin, line))
        {
            istringstream iss(line);

            vector<int> row;
            int value;

            while (iss >> value)
            {
                row.push_back(value);
            }

            if (!row.empty())
            {
                floorMap.push_back(row);
            }
        }

        binaryMap.push_back(floorMap);

        floorNumber++;
    }

    return !binaryMap.empty();
}

int sizeOfSlot(vector<vector<int>> mat, int sr, int sc)
{
    int m = mat.size();

    if (m == 0 || mat[0].empty())
        return 0;

    int n = mat[0].size();

    if (sr < 0 || sr >= m || sc < 0 || sc >= n || mat[sr][sc] != 1) return 0;

    queue<pair<int, int>> q;
    q.push({sr, sc});

    mat[sr][sc] = 2;
    int cnt = 1;

    int dy[] = {1, -1, 0, 0};
    int dx[] = {0, 0, 1, -1};

    while (!q.empty())
    {
        auto [r, c] = q.front();
        q.pop();

        for (int i = 0; i < 4; i++)
        {
            int nr = r + dy[i];
            int nc = c + dx[i];

            if (nr >= 0 && nr < m && nc >= 0 && nc < n && mat[nr][nc] == 1)
            {
                q.push({nr, nc});
                mat[nr][nc] = 2;
                cnt++;
            }
        }
    }

    return cnt;
}

#endif
