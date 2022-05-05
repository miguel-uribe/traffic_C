import trafficC
import numpy as np


p = 0.2
p0 = 0.6
pchange = 0.8
pchange_slow = 0.8
psurr = 0.2
psurr_slow = 0.2

if __name__ == "__main__":
    cardata = False
    # testing the effect of p0
    for gradient in [10,40,70,100,130,160,190]:
        for factor in np.arange(100,3000,200):            
            for newp in np.arange(0.1,1,0.1):
                    trafficC.run_simulation_parallel(30, gradient, factor, newp, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)
                    trafficC.run_simulation_parallel(30, gradient, factor, p, newp, pchange, pchange_slow, psurr, psurr_slow, cardata)
                    trafficC.run_simulation_parallel(30, gradient, factor, p, p0, newp, pchange_slow, psurr, psurr_slow, cardata)
                    trafficC.run_simulation_parallel(30, gradient, factor, p, p0, pchange, newp, psurr, psurr_slow, cardata)
                    trafficC.run_simulation_parallel(30, gradient, factor, p, p0, pchange, pchange_slow, newp, psurr_slow, cardata)
                    trafficC.run_simulation_parallel(30, gradient, factor, p, p0, pchange, pchange_slow, psurr, newp, cardata)
