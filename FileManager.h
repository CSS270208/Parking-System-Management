#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <bits/stdc++.h>
#include <filesystem>

#include "helper.h"
#include "uid.h"
#include "mapping.h"
#include "admin.h"

using namespace std;

class FileManager{
public:
    void loadVehicles(); // data of Vehcile Object
    void saveVehicles(); // data of Vehcile Object

    void loadActiveSessions(); // Data about the slot
    void saveActiveSessions(); // data about the slot

    void loadHistory();
    void appendHistory();
    void searchHistory();

    void loadPayments();
    void appendPayment();
    void searchPayments();
};

#endif
