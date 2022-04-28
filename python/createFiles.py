 # Creating the route configuration files
import numpy as np
import os
import subprocess
from route_conf import *

def createSystemFiles(density, gradient):
    # The gradient should be given in meters
    confname = "6_3_left_%dm_%.2f_%.2f_%.2f_%.2f"%(gradient*Dx, p, p0, pchange, psurr)  # the configuration name
    # now we set the allowed lane changes
    # LNA: Lane not available, this is to close lanes
    # 0, the code, 1, the lane, 2, the position (in m) where the lane first becomes unavailable, 3, the position (in m) where the lane last is unavailable
    LCR = [
        ['LNA',5,(endgrad-2*gradient)/Dx,(DL/Dx)-1],    
        ['LNA',4,(endgrad-gradient)/Dx,(DL/Dx)-1],
        ['LNA',3,(endgrad)/Dx,(DL/Dx)-1],
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
    text = text + 'const int Db=%d;    // The bus length, in cells\n'%(int(Db/Dx))
    text = text + 'const float p=%f;          // The random breaking probability\n'%p
    text = text + 'const float p0=%f;          // The random breaking probability when the vehicle is stopped\n'%p0
    text = text + 'const int Nparam = %d;    // Number of integer parameters for each bus\n'%Nparam
    text = text + 'const int Ncars = %d;     // the total number of cars\n'%(density*L*Dx/1000)
    text = text + 'const std::string confname = "%s"; // the configuration name\n'%(confname)
    text = text + 'const int Nmax = %d;    // Maximum number of lanes\n'%Nmax
    text = text + 'const int L = %d;    // Total extension of the system\n'%L
    text = text + 'const int acc = %d;    // Total extension of the system\n'%acc
    text = text + 'const float pchange = %f;    // the probability to change the lanes when possible\n'%pchange
    text = text + 'const float psurr = %f;    // the probability to surrender to a stopped car trying to change lane\n'%psurr
    text = text + 'const int tpen = %d;    // the number of seconds that a car that has surrended pirority will remain stopped\n'%tpen
    text = text + '#endif'

    # creating a backup of the parameters.h file and creating a new version
    dirname = os.path.join(dirname,'../cpp/')
    try:
        subprocess.run(['cp','parameters.h','parameters.bck'],cwd=dirname)
    except:
        pass
    file = os.path.join(dirname,"parameters.h")
    parfile = open(file, 'w')
    parfile.write(text)
    parfile.close()
    return confname