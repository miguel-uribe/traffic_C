 # Creating the route configuration files
import numpy as np
import os
import subprocess
import shutil
import filecmp
from os.path import exists
import time
from route_conf import *


# this file creates the configuration files needed to run the simulation
def createSystemFiles(gradient):
    # The gradient should be given in meters
    confname = "5_2_left_%dm"%(gradient*Dx)  # the configuration name
    # now we set the allowed lane changes
    # LNA: Lane not available, this is to close lanes
    # 0, the code, 1, the lane, 2, the position (in m) where the lane first becomes unavailable, 3, the position (in m) where the lane last is unavailable
    if gradient <= 1000:
        LCR = [
            ['LNA',4,(endgrad-2*gradient)/Dx,(DL/Dx)-1],    
            ['LNA',3,(endgrad-gradient)/Dx,(DL/Dx)-1],
            ['LNA',2,(endgrad)/Dx,(DL/Dx)-1],
        ]
    else: # if the gradient is larger than 1000, the lanes are closed altogether
        LCR = [
            ['LNA',4,0,(DL/Dx)-1],    
            ['LNA',3,0,(DL/Dx)-1],
            ['LNA',2,0,(DL/Dx)-1],
        ]
    # creating the lane availability file, this is 0 when a lane is not available and 1 when the lane is available
    L = int(DL/Dx) # this is the final length of the corridor in cells
    lanes = np.ones((L,Nmax))   # by default, all lanes are available among all the corridor
    # correcting for the information provided in route_conf
    for rule in LCR:
        if rule[0]=='LNA':
            lanes[int(rule[2]/Dx):int(rule[3]/Dx)+1,rule[1]] = 0

    # creating the maximal speed file, this is a number with the information regarding the maximal speed per lane and per position
    V = (vmax*Dt/Dx)*np.ones((L,Nmax))  # by default the maximal speed is the same along the entire corridor

    # Now we create the leftchange file, containing the information regarding the posibility to change the lane to the left, this is increasing the lane. 0 means change is not possible, 1 means change is possible
    LC = np.ones((L,Nmax))
    # clearly for the leftmost lane it is always impossible to move to the left
    LC[:,-1]=0
    # in addition, it is impossible to move to the left when the lane in the left is not there
    mask = lanes-np.roll(lanes, -1, axis = 1)==1
    LC[mask] = 0

    # Now we create the rightchange file, containing the information regarding the posibility to change the lane to the right, this is decreasing the lane. 0 means change is not possible, 1 means change is possible
    RC = np.ones((L,Nmax))
    # clearly for the rightmost lane it is always impossible to move to the left
    RC[:,0]=0
    # in addition, it is impossible to move to the left when the lane in the left is not there
    mask = np.where(lanes-np.roll(lanes, 1, axis = 1)==1)
    RC[mask] = 0

    # Finally we create the endoflane where a number marks the distance to the end of the lane, by default this is set as 1000
    EL = 1000*np.ones((L,Nmax))
    auxEL = np.zeros((L,Nmax))
    mask =  (np.roll(lanes, 1, axis = 0) - lanes) == 1
    auxEL[mask] = 1

    for i in range(Nmax):
        endF = False
        for j in np.arange(L-1,-1,-1):
            if endF:
                EL[j,i] = endPos-j
            if auxEL[j,i] == 1:
                endF = True
                endPos = j - 1



    # Exporting the information
    dirname = os.path.dirname(__file__)
    file = os.path.join(dirname,"../conf/lanes_"+confname+".txt")
    np.savetxt(file,lanes, fmt ='%d')  # the lane availability
    file = os.path.join(dirname,"../conf/V_"+confname+".txt")
    np.savetxt(file,V, fmt ='%d')  # the speed
    file = os.path.join(dirname,"../conf/LC_"+confname+".txt")
    np.savetxt(file,LC, fmt ='%d')  # the left change
    file = os.path.join(dirname,"../conf/RC_"+confname+".txt")
    np.savetxt(file,RC, fmt ='%d')  # the left change
    file = os.path.join(dirname,"../conf/EL_"+confname+".txt")
    np.savetxt(file,EL, fmt ='%d')  # the left change


    # We also create the configuration files for the c++ code
    text = '#ifndef PARAMETERS_H\n'
    text = text + '#define PARAMETERS_H\n'
    text = text + 'const int Db=%d;    // The car length, in cells\n'%(int(Db/Dx))
    text = text + 'const int Nparam = %d;    // Number of integer parameters for each bus\n'%Nparam
    text = text + 'const std::string confname = "%s"; // the configuration name\n'%(confname)
    text = text + 'const int Nmax = %d;    // Maximum number of lanes\n'%Nmax
    text = text + 'const int Carmax = %d;    // Maximum number of cars in the system\n'%Carmax
    text = text + 'const int L = %d;    // Total extension of the system\n'%L
    text = text + 'const int acc = %d;    // Total the acceleration\n'%acc  
    text = text + 'const int tpen = %d;    // the number of seconds that a car that has surrended pirority will remain stopped\n'%tpen
    text = text + 'const int vthres = %d;    // the speed changing the low speed rules from high speed rules\n'%vthres
    text = text + 'const int xmin = %d;    // the minimal position where the speed measurements are taken\n'%xmin
    text = text + 'const int xmax = %d;    // the maximal position where the speed measurements are taken\n'%xmax
    text = text + 'const int grad = %d;    // The gradient \n'%gradient
    text = text + '#endif'

    # creating a backup of the parameters.h file and creating a new version
    
    try:
        shutil.copy('parameters.h', 'parameters.bck')
    except:
        pass
    #file = os.path.join(dirname,"parameters.h")
    file = "parameters.h"
    parfile = open(file, 'w')
    parfile.write(text)
    parfile.close()
    return confname


# This script runs the simulation, it checks whether the configuration file has changed and compiles the code

def run_simulation(gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow):
    # we update the configuration files
    confname = createSystemFiles(gradient)
    # once the files are created we compile the cpp script if there are changes
    if (not filecmp.cmp('parameters.h','parameters.bck')):
        print('Recompiling')
        comp = subprocess.run(['g++','-O2','simulation.cpp','-o','simulation_'+confname])

    # we run the command
    seed = 0
    subprocess.Popen(['simulation_'+confname+'.exe',str(seed), str(density), str(p), str(p0), str(pchange), str(pchange_slow), str(psurr), str(psurr_slow),"1"], shell = True)


def run_simulation_parallel(Nsim, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata):
    # we update the configuration files
    confname = createSystemFiles(gradient)
    # the script is compiled when the executable does not exist
    if not exists('simulation_'+confname+'.exe'):
        print('Recompiling')
        comp = subprocess.run(['g++','-O2','simulation.cpp','-o','simulation_'+confname])


    # running the pool of processes
    procs=[]
    # Creating the processes
    for i in range(Nsim):
        #Creating one iteration process
        if ((i == 0) and cardata):
            procs.append(subprocess.Popen(['simulation_'+confname+'.exe',str(i), "%d"%density, "%.2f"%p, "%.2f"%(p0), "%.2f"%(pchange), "%.2f"%(pchange_slow), "%.2f"%(psurr), "%.2f"%(psurr_slow),"1"], shell = True))
        else:
            procs.append(subprocess.Popen(['simulation_'+confname+'.exe',str(i), "%d"%density, "%.2f"%p, "%.2f"%(p0), "%.2f"%(pchange), "%.2f"%(pchange_slow), "%.2f"%(psurr), "%.2f"%(psurr_slow),"0"], shell = True))
        
    # waiting for all processes to finish
    while(True):
        try:
            np.sum([proc.poll() for proc in procs])
            break
        except:
            pass
        time.sleep(0.05)

    # now that all the processes are finished we load the data
    flows = []
    speeds = []
    for i in range(Nsim):
        filename = 'sim_results/sim_results_'+ confname+'_%d_%.2f_%.2f_%.2f_%.2f_%.2f_%.2f_%d.txt'%(density, p, p0, pchange, pchange_slow, psurr, psurr_slow, i) 
        try:
            data = np.vstack((data,np.loadtxt(filename)))
        except:
            data = np.loadtxt(filename)
        os.remove(filename)

    fileout = 'sim_results/sim_results_'+ confname+'_%d_%.2f_%.2f_%.2f_%.2f_%.2f_%.2f.txt'%(density, p, p0, pchange, pchange_slow, psurr, psurr_slow) 
    means = data.mean(axis = 0)
    stds = data.std(axis = 0)
    np.savetxt(fileout, np.array([means[0], stds[0], means[1], stds[1]]))
