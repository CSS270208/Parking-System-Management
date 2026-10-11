#ifndef HELPER_H
#define HELPER_H

#include <bits/stdc++.h>
using namespace std;

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

// slotcode -> dimensions

#endif
