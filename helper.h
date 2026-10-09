#ifndef HELPER_H
#define HELPER_H

#include <bits/stdc++.h>
using namespace std;

void toLowerString(string &s)
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

string invertVC(int x)
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

int SC(const string &s)
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

string invertSC(int x)
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

// Vehicle code -> compatible slot codes.
vector<int> VehicleToSlotsCode(int code)
{
    if (code == 0)
        return {0, 1};

    else if (code == 1)
        return {1};

    else if (code == 2)
        return {2};

    else if (code == 3)
        return {2, 3};

    return {};
}

// Vehicle string -> compatible slot codes.
vector<int> VehicleToSlotsString(const string &type)
{
    return VehicleToSlotsCode(VC(type));
}


// size to slot string

// string to slot size
#endif
