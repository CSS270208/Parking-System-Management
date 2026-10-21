#ifndef UID_H
#define UID_H

#include <bits/stdc++.h>
using namespace std;

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

#endif
