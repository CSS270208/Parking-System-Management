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

int VC(const string &type)
{
    if (type == "bike")
        return 0;
    else if (type == "car")
        return 1;
    else if (type == "truck")
        return 2;
    else if (type == "ev")
        return 3;

    return -1;
}

string invertVC(int x)
{
    if (x == 0)
        return "bike";
    else if (x == 1)
        return "car";
    else if (x == 2)
        return "truck";
    else if (x == 3)
        return "ev";

    return "unknown";
}

int SC(const string &s)
{
    if (s == "bike")
        return 0;
    else if (s == "compact")
        return 1;
    else if (s == "large")
        return 2;
    else if (s == "ev")
        return 3;

    return -1;
}

string invertSC(int x)
{
    if (x == 0)
        return "bike";
    else if (x == 1)
        return "compact";
    else if (x == 2)
        return "large";
    else if (x == 3)
        return "ev";

    return "unknown";
}

vector<int> VehicleToSlotsString(const string &type)
{
    int vehicleType = VC(type);

    if (vehicleType == 0)
        return {0, 1};

    else if (vehicleType == 1)
        return {1};

    else if (vehicleType == 2)
        return {2};

    else if (vehicleType == 3)
        return {2, 3};

    return {};
}
vector<int> VehicleToSlotsCode(const int &code)
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

#endif
