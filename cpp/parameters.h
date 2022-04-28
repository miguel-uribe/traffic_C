#ifndef PARAMETERS_H
#define PARAMETERS_H
const int Db=4;    // The bus length, in cells
const float p=0.200000;          // The random breaking probability
const float p0=0.500000;          // The random breaking probability when the vehicle is stopped
const int Nparam = 16;    // Number of integer parameters for each bus
const int Ncars = 100;     // the total number of cars
const std::string confname = "6_3_left_10m_0.20_0.50_0.50_1.00"; // the configuration name
const int Nmax = 6;    // Maximum number of lanes
const int L = 5000;    // Total extension of the system
const int acc = 1;    // Total extension of the system
const float pchange = 0.500000;    // the probability to change the lanes when possible
const float psurr = 1.000000;    // the probability to surrender to a stopped car trying to change lane
const int tpen = 5;    // the number of seconds that a car that has surrended pirority will remain stopped
#endif