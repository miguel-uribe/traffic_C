
#ifndef CREATE_SYSTEM
#define CREATE_SYSTEM

# include "parameters.h"
# include <array>
# include <string>
#include <fstream>
#include <sstream>

// this script loads the configuration files and creates the corresponding lane configuration
std::array<std::array<int, L>, Nmax> loadconffile(std::string root){
    // creating the array
    std::array<std::array<int, L>, Nmax> lanes;
    // opening the file
    std::string filename = "../conf/";
    filename = filename + root +"_"+confname + ".txt";
    std::ifstream file(filename);
    std::string str;
    int val;
    int i = 0;
    while (std::getline(file,str)){
        // retrieving the origin and destination stations
        std::istringstream iss(str);
        for (int j=0; j<Nmax; j++){
            iss>>val;
            lanes[j][i] = val;
        }
        i++;
    }
    return lanes;
}


#endif