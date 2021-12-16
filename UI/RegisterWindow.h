//
// Created by sbeam on 05/12/2021.
//

#ifndef MEGABOY_REGISTERWINDOW_H
#define MEGABOY_REGISTERWINDOW_H


#include "../CPU/cpu.h"

class RegisterWindow {

    CPU& cpu;

public:

    explicit RegisterWindow(CPU &cpu) : cpu(cpu) {
    }

    void UpdateUI();

};


#endif //MEGABOY_REGISTERWINDOW_H
