#include <iostream>
#include "parameters.h"
#include "createsystem.h"
#include "carC.h"
#include <array>
#include <vector>
#include <numeric>
#include <math.h>


//#include <chrono>
//#include <random>
//#include <fstream>
//#include <sstream>
//#include <thread>
//#include <future>


using namespace std;

int main (int argc, char **argv){
    // The arguments list
    // 1 - seed
    // 2 - density
    // 3 - p, random breaking probability
    // 4 - p0, random breaking probability, stopped vehicle
    // 5 - pchange, the probability to change lanes when possible
    // 6 - pchange_slow, the probability to change lanes when possible, when at low speed
    // 7 - psurr, the probability to change lane without standard safety conditions
    // 8 - psurr_slow, the probability to change lane without standard safety conditions, when at low speeds
    // 9 - print, whether or not to print the results

    
    const int seed = stoi(argv[1]);
    srand(seed);

    // the density fixes the number of cars
    const int density = stoi(argv[2]);
    const int Ncars = int(density*L/1000);
    if(Ncars>Carmax){
        cout<<"The number of cars "<<Ncars<<" exceeds the maximum, leaving the simulation"<<endl;
        return 0;
    }
    
    // the driving parameters
    const float p = stof(argv[3]);
    const float p0 = stof(argv[4]);
    const float pchange = stof(argv[5]);
    const float pchange_slow = stof(argv[6]);
    const float psurr = stof(argv[7]);
    const float psurr_slow = stof(argv[8]);
    int print = stoi(argv[9]);


    ///////////////////////////////////////////
    // Reading the configuration files
    // INFO:: In all these arrays, the first element addresses the lane, the second one the x position
    array<array<int, L>, Nmax> lanes, V, LC, RC, EL;
    lanes = loadconffile("lanes");
    V = loadconffile("V");
    LC = loadconffile("LC");
    RC = loadconffile("RC");
    EL = loadconffile("EL");
    
    /////////////////////////////////////////////////////////////
    // Creating the array of cars

    int flow = 0;
    int speeds = 0;
    int speedcounts = 0;
    int DT = 20*60;
    int NepochEq = 5;
    int Nepoch = 1;
    array<vector<int>, Nparam> carparam;       

    populate(carparam,lanes, Ncars);
  

    /////////////////////////////////////////////////////////
    // exporting the data
    string filename, fileroot;
    fileroot = "./sim_results/sim_results_";
    // adding the configuration
    fileroot = fileroot + confname;
    for (int i=2; i<argc-1;i++)
        fileroot = fileroot + "_" + argv[i];
    filename = fileroot + "_" + argv[1] + ".txt";

    ofstream printfile;
    if (print == 1){
        string filename = fileroot + "_cardata.txt";
        printfile.open(filename, ios_base::out);
    }
    // sorting the cars
    vector<int> index(Ncars, 0);
    for (int epoch = 0; epoch<(NepochEq+Nepoch); epoch++){
        for (int TIME = DT*epoch; TIME< DT*(epoch+1); TIME++){
            sortcars(carparam, index);
            calculategaps(carparam, EL, V);
            carchangelane(carparam,V, LC, RC, EL, TIME, pchange, pchange_slow, psurr, psurr_slow);
            sortcars(carparam, index);
            calculategaps(carparam, EL, V);
            caradvance(carparam, V, lanes, flow, speeds, speedcounts, p, p0);
            // we print the info of the last epoch
            if ((epoch == (NepochEq+Nepoch-1)) & (print==1)){
                for (int i = 0; i<Ncars; i++){
                    printfile<<TIME<<" ";
                    for (int j = 0; j< 2; j++){
                        printfile<<carparam[j][i]<<" ";
                    }
                    printfile<<carparam[Nparam-1][i]<<endl;
                    
                }    
            }
        }
        if ((epoch >= NepochEq)){
            ofstream outfile;
            outfile.open(filename, std::ios_base::app);
            outfile<<double(flow)/DT<<" "<<double(speeds)/double(speedcounts)<<endl;
            outfile.close();
        }
        flow = 0;
        speeds = 0;
        speedcounts = 0;
    }
    printfile.close();

}