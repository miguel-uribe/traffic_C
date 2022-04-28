#ifndef CAR_C
#define CAR_C
#include <array>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "parameters.h"

// This function randomly populates the system with cars
void populate(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<std::array<int, L>, Nmax> lanes){
     /* Parameters List:
    0: position in x
    1: position in y
    2: speed
    3: gapf, forward gap
    4: gapfl, forward gap in the lane to the left
    5: gapbl, backward gap in the lane to the left
    6: gapfr, forward gap in the lane to the right
    7: gapbr, backward gap in the lane to the right
    8: vbefl, speed of the car behind in the left lane
    9: vbefr, speed of the car behind in the right lane
    10: acc, the car acceleration
    11; vfor, the speed of the car ahead
    12; vforl, the speed of the vehicle ahead in the left lane
    13; vforr, the speed of the vehicle ahead in the right lane
    14; tepen, the penalty time
    15; ID, the car ID
    */

    int x,y;
    for (int i=0; i<Ncars; i++){
        bool bad = true;
        while(bad){
            // we generate a random position
            x = rand() % L;
            // we generate a random lane
            y = rand() % Nmax;
            // now we evaluate whether the position is valid
            if (lanes[y][x]==1){
                bad = false;
                //std::cout<<i<<" "<<x<<" "<<y<<std::endl;
                carparam[0][i] = x; // x
                carparam[1][i] = y; // y
                carparam[2][i] = rand() % 17; // speed
                carparam[3][i] = 1000; // gapf
                carparam[4][i] = 1000; // gapfl
                carparam[5][i] = 1000; // gapbl
                carparam[6][i] = 1000; // gapfr
                carparam[7][i] = 1000; // gapbr
                carparam[10][i] = acc; //acc
                carparam[9][i] = 0; //vbefr
                carparam[8][i] = 0; //vbefr
                carparam[11][i] = 1000; //vfor
                carparam[12][i] = 1000; //vforl
                carparam[13][i] = 1000; //vforr
                carparam[14][i] = 0; //tpen
                carparam[15][i] = i; //ID
            }
        }
    }
}

// This function updates the car array to ensure that the cars are ordered
void sortcars(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<int, Ncars> &index){

    // cars are only sorted if there are more than one cars
    if (Ncars>1){
        //std::cout<<"Sorting "<<BUSESPAR[0].size()<<std::endl;
        std::array<int, Ncars> idx;
        std::iota(idx.begin(), idx.end(),0);
        /*for (int i=0; i<idx.size(); i++){
            std::cout<<idx[i]<<" ";
        }*/
        //sorting indexes
        std::sort(idx.begin(),idx.end(),
        [&carparam](int i1, int i2){
            if (carparam[0][i1]==carparam[0][i2]){ //if the cars are in the same position, the order is given according to the lanes
                return carparam[1][i1]<carparam[1][i2];
            }
            else{
                return carparam[0][i1]<carparam[0][i2]; // if they are not in the same position, the order is given according to the position
            }
        }
        );
        // we now check whether there is a change in order
        if (index!=idx){ // in case there is a change in order
            //std::cout<<"there is a change"<<std::endl;
            // once sorted, we proceed to update all the arrays
            for (int j =0; j<Nparam; j++){
                std::array<int, Ncars> aux;
                for (int i =0; i<idx.size(); i++){
                    aux[i]=(carparam[j][idx[i]]);
                }
                carparam[j]=aux; 
            }

        }
        // We finally update the index
        index = idx;
    }
}


// This function calculates all the gaps for all cars
void calculategaps(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<std::array<int, L>, Nmax> EL, std::array<std::array<int, L>, Nmax> V){
    int lane;
    // The list of the last cars pending the forward gaps in a given lane
    std::vector<int> lastcarR[Nmax];  // pending gaps to the left
    std::vector<int> lastcarL[Nmax];  // pending gaps to the right
    std::array<int, Nmax> lastcar;  // pending forward gaps
    std::fill(lastcar.begin(), lastcar.end(),-1);

    // The arrays that tell whether the right-forward and left-forward gaps have been updated for a given lane
    std::array<bool, Nmax> rightF;  // whether the gaps to the right have been updated
    std::array<bool, Nmax> leftF;   // whether the gaps to the right have been updated
    std::fill(rightF.begin(), rightF.end(),false);
    std::fill(leftF.begin(), leftF.end(),false);

    // we initially set all gaps to their default values
    carparam[3].fill(1000);
    carparam[4].fill(1000);
    carparam[5].fill(1000);
    carparam[6].fill(1000);
    carparam[7].fill(1000);
    carparam[8].fill(0);
    carparam[9].fill(0);
    carparam[11].fill(1000);
    carparam[12].fill(1000);
    carparam[13].fill(1000);


    // now we proceed to scan all cars from east to west
    for (int i=0; i<Ncars; i++){
        // we get the lane information
        lane = carparam[1][i];

        /////////////////////////////////////////////////
        ///// FORWARD GAP
        // by default the forward gap corresponds to the distance to the next lane ending
        carparam[3][i] = EL[lane][carparam[0][i]];
        // if there is a last car already, we update its forward gap and speed ahead
        if (lastcar[lane]>=0){
            carparam[3][lastcar[lane]] = std::min(carparam[0][i]-carparam[0][lastcar[lane]]-Db, carparam[3][lastcar[lane]]);
            carparam[11][lastcar[lane]] = carparam[2][i];
        }

        // we now update the last car information
        lastcar[lane] = i;

        if(rightF[lane] == true)  // the forward gap to the right information is up to date
            lastcarR[lane].clear(); 
        lastcarR[lane].push_back(i);

        if(leftF[lane] == true)  // the forward gap to the left information is up to date
            lastcarL[lane].clear(); 
        lastcarL[lane].push_back(i);

        rightF[lane] = false;
        leftF[lane] = false;

        // Now we evaluate the gaps to the left
        if (lane<Nmax-1){
            if (lastcarR[lane+1].size()>0){  // If there is already some cars in the lane to the left
                // we update the gapbl and the vbefl
                carparam[5][i] =carparam[0][i]-carparam[0][lastcarR[lane+1].back()]-Db;
                carparam[8][i] =carparam[2][lastcarR[lane+1].back()];

                // Creating the rule to surrender priority
                // if the gap with the car in the back is between 1 and 3, if the speed of the current car is zero, and it is close to the end of a line, and the car in the back moves slowly, 
                /*
                // the car in the back may surrender the priority with certain probability
                if ((carparam[5][i]>0) && (carparam[5][i]<4) &&(carparam[2][i]==0) && (EL[lane][carparam[0][i]]<V[lane][carparam[0][i]]) && (carparam[2][lastcar[lane+1].back()]<=3) && (carparam[14][lastcar[lane+1].back()] == 0)){
                    // we throw the dice
                    float dice = ((double) rand() / (RAND_MAX));
                        if (dice < psurr)
                            carparam[14][lastcar[lane+1].back()]=tpen;
                }
*/
                // if the lastcar information has not been updated
                if (rightF[lane+1]==false){
                    for (int index : lastcarR[lane+1]){
                        // we update gapfr
                        carparam[6][index] = carparam[0][i]-carparam[0][index]-Db;
                        // we update vforr
                        carparam[13][index] = carparam[2][i];
                        rightF[lane+1] = true;
                    }
                }
            }
        }

        // Now we evaluate the gaps to the right
        if (lane>0){
            if (lastcarL[lane-1].size()>0){  // If there is already a car in the right lane
                // we update the gapbr and the vbefr
                carparam[7][i] = carparam[0][i]-carparam[0][lastcarL[lane-1].back()]-Db;
                carparam[9][i] = carparam[2][lastcarL[lane-1].back()];

/*
                // Creating the rule to surrender priority
                // if the gap with the car in the back is between 1 and 3, if the speed of the current car is zero, and it is close to the end of a line, and the car in the back moves slowly, and the car is not on penalty
                // the car in the back may surrender the priority with certain probability
                if ((carparam[7][i]>0) && (carparam[7][i]<4) &&(carparam[2][i]==0) && (EL[lane][carparam[0][i]]<V[lane][carparam[0][i]]) && (carparam[2][lastcar[lane-1].back()]<=3) && (carparam[14][lastcar[lane-1].back()] == 0)){
                    // we throw the dice
                    float dice = ((double) rand() / (RAND_MAX));
                        if (dice < psurr)
                            carparam[14][lastcar[lane-1].back()] = tpen;
                }*/
                // if the lastcar information has not been updated
                if (leftF[lane-1]==false){
                    for (int index : lastcarL[lane-1]){
                        // we update gapfl
                        carparam[4][index] = carparam[0][i]-carparam[0][index]-Db;
                        // we update vforl
                        carparam[12][index] = carparam[2][i];
                        leftF[lane-1] = true;
                    }
                }
            }
        }
    }
}

// This function creates all the line changes in the system, movements to the right are performed in even times, movements to the left are performed in odd times

/* In this version, we check not for the speeds but for the gaps in order to change the lanes, we also check for the safety criterion
For introduce a probability to breach the safety criterion
*/
void carchangelane(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<std::array<int, L>, Nmax> V, std::array<std::array<int, L>, Nmax> LC,std::array<std::array<int, L>, Nmax> RC, std::array<std::array<int, L>, Nmax> EL, int TIME){
    int x, lane;
    // We create a new array with all the new values of the lanes
    std::array<int, Ncars> newy = carparam[1]; // by default, the bus stays in the same lane
    // If the time is even, we perform the possible movements to the right
    if (TIME % 2 == 0){
        for (int i=0; i<Ncars; i++){ // we scan over all buses
            x = carparam[0][i];
            lane = carparam[1][i];
            if (RC[lane][x]==1){ // only if a movement to the right is allowed
                // we now evaluate willingness
                if (carparam[3][i]<3*V[lane][x]){ //only in the case where there is a short gap the car decides to change lane
                    // in the case the gap is produced by a car we check the speed of the car ahead, and also the speed or availability of scape in the destination lane
                    // in the case the gap is produced by an end of line, the path to follow is clear
                    // if (there is more space in the right lane) and (the car to the right is faster than the present car)
                    if ((carparam[3][i] < carparam[6][i]) && (carparam[13][i]>=carparam[2][i])){
                        // now we check for safety
                        // if (the space in the back is larger than the speed of the car in the back and the space in the front is also enough)
                        if ((carparam[7][i]>carparam[9][i]) && (carparam[6][i]>carparam[2][i])){
                            // now the car is able to change lanes, we throw the dice to check whether 
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < pchange))
                                newy[i] = lane-1;
                        }
                        // If the safety checks are not satisfied, the car can still change
                        // Some space in the back is still needed
                        else if(carparam[7][i]>2){
                             // now the car is able to change lanes, we throw the dice to check whether 
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < psurr))
                                newy[i] = lane-1;
                        }
                    }  
                }
            }
        }
    }
    // If the time is odd, we perform the possible movements to the left
    else{
        for (int i=0; i<Ncars; i++){ // we scan over all buses
            x = carparam[0][i];
            lane = carparam[1][i];
            if (LC[lane][x]==1){ // only if a movement to the left is allowed
                // we now evaluate willingness
                if (carparam[3][i]<3*V[lane][x]){ //only in the case where there is a short gap the car decides to change lane
                    // in the case the gap is produced by a car we check the speed of the car ahead, and also the speed or availability of scape in the destination lane
                    // in the case the gap is produced by an end of line, the path to follow is clear
                    // if (there is more space in the left lane) and (the car to the left is faster than the present car)
                    if ((carparam[3][i] < carparam[4][i]) && (carparam[12][i] >= carparam[2][i])){
                        // now we check for safety
                        // if (the space in the back is larger than the speed of the car in the back and the space in the front is also enough)
                        if ((carparam[5][i]>carparam[8][i]) && (carparam[4][i]>carparam[2][i])){
                            // now the car is able to change lanes, we throw the dice to check whether it does.
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < pchange))
                                newy[i] = lane+1;
                        }
                        // If the safety checks are not satisfied, the car can still change
                        // Some space in the back is still needed
                        else if(carparam[5][i]>2){
                             // now the car is able to change lanes, we throw the dice to check whether 
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < psurr))
                                newy[i] = lane+1;
                        }
                    }  
                }
            }
        }
    }
    
    // Now we update all the lanes
    carparam[1]=newy;        
}


/*
// This function creates all the line changes in the system, movements to the right are performed in even times, movements to the left are performed in odd times
void carchangelane(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<std::array<int, L>, Nmax> V, std::array<std::array<int, L>, Nmax> LC,std::array<std::array<int, L>, Nmax> RC, std::array<std::array<int, L>, Nmax> EL, int TIME){
    int x, lane;
    // We create a new array with all the new values of the lanes
    std::array<int, Ncars> newy = carparam[1]; // by default, the bus stays in the same lane
    // If the time is even, we perform the possible movements to the right
    if (TIME % 2 == 0){
        for (int i=0; i<Ncars; i++){ // we scan over all buses
            x = carparam[0][i];
            lane = carparam[1][i];
            if (RC[lane][x]==1){ // only if a movement to the right is allowed
                // we now evaluate willingness
                if (carparam[3][i]<3*V[lane][x]){ //only in the case where there is a short gap the car decides to change lane
                    // in the case the gap is produced by a car we check the speed of the car ahead, and also the speed or availability of scape in the destination lane
                    // in the case the gap is produced by an end of line, the path to follow is clear
                    // if ( the gap is produced by a lane change) or ((the car in front is slower) and (the car in the right lane is faster or there is enough space))
                    if ((carparam[3][i] == EL[lane][x]) || ((carparam[11][i]<=carparam[2][i]) && ((carparam[11][i]<=carparam[13][i]) || (carparam[6][i]>carparam[2][i])))){
                        // now we check for safety
                        // if (the space in the back is larger than the speed of the car in the back and the space in the front is also enough)
                        if ((carparam[7][i]>=(carparam[9][i]+1)) && (carparam[6][i]>=carparam[2][i]+1)){
                            // now the car is able to change lanes, we throw the dice to check whether 
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < pchange))
                                newy[i] = lane-1;
                        }
                    }  
                }
            }
        }
    }
    // If the time is odd, we perform the possible movements to the left
    else{
        for (int i=0; i<Ncars; i++){ // we scan over all buses
            x = carparam[0][i];
            lane = carparam[1][i];
            if (LC[lane][x]==1){ // only if a movement to the left is allowed
                // we now evaluate willingness
                if (carparam[3][i]<3*V[lane][x]){ //only in the case where there is a short gap the car decides to change lane
                    // in the case the gap is produced by a car we check the speed of the car ahead, and also the speed or availability of scape in the destination lane
                    // in the case the gap is produced by an end of line, the path to follow is clear
                    // if ( the gap is produced by a lane change) or ((the car in front is slower) and (the car in the left lane is faster or there is enough space))
                    if ((carparam[3][i] == EL[lane][x]) || ((carparam[11][i]<=carparam[2][i]) && ((carparam[11][i]<=carparam[12][i]) || (carparam[4][i]>carparam[2][i])))){
                        // now we check for safety
                        // if (the space in the back is larger than the speed of the car in the back and the space in the front is also enough)
                        if ((carparam[5][i]>=(carparam[8][i]+1)) && (carparam[4][i]>=carparam[2][i]+1)){
                            // now the car is able to change lanes, we throw the dice to check whether it does. If the car is still, it WILL change the lanes
                            float dice = ((double) rand() / (RAND_MAX));
                            if ((dice < pchange))
                                newy[i] = lane+1;
                        }
                    }  
                }
            }
        }
    }
    
    // Now we update all the lanes
    carparam[1]=newy;        
}
*/



// making the cars move
void caradvance(std::array<std::array<int, Ncars>, Nparam> &carparam, std::array<std::array<int, L>, Nmax> V, std::array<std::array<int, L>, Nmax> lanes, double & flow, int & counts){
    float prand;
    for (int i =0; i<Ncars; i++){
        /* Not using penalty
        // only cars out of penaly move
        if (carparam[14][i]>0){
            carparam[14][i] = carparam[14][i]-1;  // we reduce the penalty
        }
        */
        // we implement the VDR-TCA model of Maerivoet
        // we determine the randomization parameter depending on the speed
        if (carparam[2][i]==0)
            prand = p0;
        else
            prand = p;
        // we calculate the new speed, vnew=max(0,min(acc*(v+acc),acc*gap,acc*vmax))
        carparam[2][i]=std::max(0,std::min(carparam[2][i]+carparam[10][i],std::min(carparam[3][i],V[carparam[1][i]][carparam[0][i]])));
        // We introduce randomization
        float r = ((double) rand() / (RAND_MAX));
        if (r<prand){carparam[2][i]= std::max(0,carparam[2][i]-1);}

    
        // now we update the position
        carparam[0][i]=carparam[0][i]+carparam[2][i];
        // checking whether the car leaves the system
        if (carparam[0][i]>=L){
            counts++;
            // we set the car to the beginning
            carparam[0][i] = carparam[0][i] - L;
            // we also switch the lane to be randomly chosen
            bool bad = true;
            while(bad){
                // we generate a random lane
                int y = rand() % Nmax;
                // now we evaluate whether the position is valid
                if (lanes[y][carparam[0][i]]==1){
                    bad = false;
                    carparam[1][i] = y; // y
                }
            }
        }
        // we add the speed to the database
        flow = flow + carparam[2][i];
    }
}


// This function calculates all the gaps for all cars
void printdata(std::array<std::array<int, Ncars>, Nparam> carparam, int TIME){
    // opening the file
    std::string filename = "cardata.txt";
    std::ofstream outfile;
    outfile.open(filename, std::ios_base::app);
    for (int i = 0; i<Ncars; i++){
        outfile<<TIME<<" ";
        for (int j = 0; j< Nparam; j++){
            outfile<<carparam[j][i]<<" ";
        }
        outfile<<std::endl;
    }
    outfile.close();
}


#endif