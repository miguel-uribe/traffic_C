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
    
    int seed = stoi(argv[1]);
    srand(seed);

//    int density = stoi(argv[2]);
//    int gradient = stoi(argv[3]);
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

    double flow = 0;
    int counts = 0;
    int DT = 20*60;
    int NepochEq = 5;
    int Nepoch = 5;
    array<array<int, Ncars>, Nparam> carparam;
    populate(carparam,lanes);


    /////////////////////////////////////////////////////////
    // exporting the data
    string filename = "../cpp/sim_results/sim_results_";
    // adding the configuration
    filename = filename + confname + "_" + argv[2] + ".txt";

    // sorting the cars
    array<int, Ncars> index;
    for (int epoch = 0; epoch<(NepochEq+Nepoch); epoch++){
        for (int TIME = DT*epoch; TIME< DT*(epoch+1); TIME++){
            sortcars(carparam, index);
            calculategaps(carparam, EL, V);
            carchangelane(carparam,V, LC, RC, EL, TIME);
            sortcars(carparam, index);
            calculategaps(carparam, EL, V);
            //if (epoch == Nepoch-1)
            //    printdata(carparam, TIME);
            caradvance(carparam, V, lanes, flow, counts);
        }
        if (epoch >= NepochEq){
            ofstream outfile;
            outfile.open(filename, std::ios_base::app);
            outfile<<flow/DT/L<<" "<<double(counts)/DT<<" "<<flow/DT/Ncars<<endl;
            outfile.close();
        }
        flow = 0;
        counts = 0;
    }

}