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
    // 2 - influx in cars per hour
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
    const int factor = stoi(argv[2]);

    // the driving parameters
    double p = stod(argv[3]);
    double p0 = stod(argv[4]);
    double pchange = stod(argv[5]);
    double pchange_slow = stod(argv[6]);
    double psurr = stod(argv[7]);
    double psurr_slow = stod(argv[8]);
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
    double speeds = 0;
    array<vector<int>, Nparam> carparam; 
    int ncarsout = 0;
  
    std::default_random_engine generator (seed);
    vector<int> index;

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

    /// Running the simulation
    int inittime = 4*3600;
    int finaltime = 10*3600;
    //int finaltime = inittime + 1000;
    int initprinttime = 7*3600;
    int finalprinttime = 7.17*3600;
    int Ncars = 0;  // the total number of cars

    for (int time = inittime; time<= finaltime; time++){
        introduceCars(factor, time, carparam, lanes, generator, Ncars);
        sortcars(carparam, index);
        removecars(carparam,ncarsout);
        calculategaps(carparam, EL, V);
        carchangelane(carparam,V, LC, RC, EL, time, pchange, pchange_slow, psurr, psurr_slow);
        sortcars(carparam, index);
        calculategaps(carparam, EL, V);
        ncarsout = caradvance(carparam, V, lanes, flow, speeds, p, p0, time);
        // we print the info of the last epoch
        if (print==1){
            if ((time>initprinttime) && (time<finalprinttime)){
                for (int i = 0; i<carparam[0].size(); i++){
                    printfile<<time<<" ";
                    for (int j = 0; j< 2; j++){
                        printfile<<carparam[j][i]<<" ";
                    }
                    printfile<<carparam[15][i]<<endl; 
                }    
            }
        }   
    }

    //cout<<flow<<" "<<speeds<<endl;

    // printing the results to an output file
    ofstream outfile;
    outfile.open(filename, std::ios_base::app);
    outfile<<double(flow)*3600/(finaltime-inittime)<<" "<<speeds/flow*3.6<<endl;
    outfile.close();
    
    printfile.close();
}