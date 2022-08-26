//
// Created by sbeam on 05/12/2021.
//

#ifndef MEGABOY_REGISTERWINDOW_H
#define MEGABOY_REGISTERWINDOW_H


#include "../CPU/cpu.h"

class RegisterWindow {

    static void DrawRegisterTable(CPU &cpu);
    static void DrawFlagsTable(CPU &cpu);

public:

    static void UpdateUI(CPU &cpu);

};


#endif //MEGABOY_REGISTERWINDOW_H
