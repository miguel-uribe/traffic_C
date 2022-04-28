
#ifndef CREATE_SYSTEM
#define CREATE_SYSTEM

# include "parameters.h"
# include <array>
# include <string>
#include <fstream>
#include <sstream>


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


/*
struct System
{
    lineC Lines[Nlines];
    stationC Stations[NStations];
    int limits[2];
};

auto createsystem(std::string filename){
    // Creating the structure of the system
    System SYSTEM;
    
    int xmin=0;
    
    // creating the list of stations in the system
    for (int i =0; i<NStations;i++){
        std::string name = "S";
        name= name+std::to_string(i);
        int x = gap+i*DS;
        SYSTEM.Stations[i]=stationC(name,x);
    }

    // Reading the routedefinition file
    std::vector <std::vector<int>> wagons;
    std::vector <std::vector<int>> stoplist;

    std::ifstream file;
    file.open(filename);
    std::string line;
    int j=0; // the line counter
    while (std::getline(file,line)){
        // retrieving the stations
        std::vector<int> stops;
        std::vector<int> wags;
        std::istringstream iss(line);
        std::string number;
        while(iss>>number){
            stops.push_back(stoi(number));
        }
        // now we retrieve the wagons
        std::getline(file,line);
        std::istringstream oss(line);
        while(oss>>number){
            wags.push_back(stoi(number));
        }
        stoplist.push_back(stops);
        wagons.push_back(wags);
        j++;
    }
    // creating the lines
    for (int i=0; i<Nlines-2; i++){
        std::string name;
        int acc;
        if (stoplist[i][0]<stoplist[i][1]){ // even index go to the east
            int index = i;
            name = "E"+std::to_string(index);
            acc = 1;
        }
        else{ // odd index go to west
            int index = i;
            name = "W"+std::to_string(index);
            acc = -1;    
        }
        SYSTEM.Lines[i]=lineC(name,stoplist[i],wagons[i],SYSTEM.Stations, acc);
        for (auto j: stoplist[i]){SYSTEM.Stations[j].addline(i);}
    }

    // we now need to create the empty lines
    
    std::vector<int> aux;
    SYSTEM.Lines[Nlines-2]=lineC("E"+std::to_string(Nlines-2),aux, aux,SYSTEM.Stations, 1);
    SYSTEM.Lines[Nlines-1]=lineC("W"+std::to_string(Nlines-1),aux, aux,SYSTEM.Stations, -1);

    int xmax=SYSTEM.Stations[NStations-1].x+(Nw-1)*Ds+Dw+gap;
    SYSTEM.limits[0]=xmin;
    SYSTEM.limits[1]=xmax;


    return (SYSTEM);

}
*/
#endif