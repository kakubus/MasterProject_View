#include "UtilsFcn.h"
#include <iostream>
#include <Windows.h>
#include <random>


void clearScreen()
{
    //  static unsigned i = 0;
    //   if (i > 10)
    //   {
    //       i = 0; 
    //       system("cls");
    //   }
    HANDLE hOut;
    COORD Position;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    Position.X = 0;
    Position.Y = 0;
    SetConsoleCursorPosition(hOut, Position);
    //   i++;
}

int randomInt(int min, int max) {
    // Believe that min <= max! (Happy path)
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<> dist(min, max);

    return dist(gen);
}

float randomFloat(float min, float max)
{
    // Believe that min <= max! (Happy path)
    std::random_device rdF;
    std::mt19937 genF(rdF());
    std::uniform_real_distribution<double> distF(min, max);

    return distF(genF);
}
