from re import L
import trafficC
import numpy as np


if __name__ == "__main__":
    cardata = False
    # testing the effect of p0
    for gradient in [10,40,70,100,130,160]:
        for density in np.arange(20,200,10):
            for pchange in [0,0.25,0.5,0.75,1]:
                p = 0.25
                p0 = 0.5
                pchange_slow = 0.25
                psurr = 0.25
                psurr_slow = 0.25
                trafficC.run_simulation_parallel(30, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)


            for pchange_slow in [0,0.25,0.5,0.75,1]:
                p = 0.25
                p0 = 0.5
                pchange = 0.25
                psurr = 0.25
                psurr_slow = 0.25
                trafficC.run_simulation_parallel(30, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)


            for psurr in [0,0.25,0.5,0.75,1]:
                p = 0.25
                p0 = 0.5
                pchange = 0.25
                pchange_slow = 0.25
                psurr_slow = 0.25
                trafficC.run_simulation_parallel(30, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)

            for psurr_slow in [0,0.25,0.5,0.75,1]:
                p = 0.25
                p0 = 0.5
                pchange = 0.25
                pchange_slow = 0.25
                psurr = 0.25
                trafficC.run_simulation_parallel(30, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)

            for p0 in [0,0.25,0.5,0.75,1]:
                p = 0.25
                p0 = 0.5
                pchange = 0.25
                pchange_slow = 0.25
                psurr = 0.25
                psurr_slow = 0.25
                trafficC.run_simulation_parallel(30, gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow, cardata)
