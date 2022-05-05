import trafficC
import numpy as np



p = 0.2
p0 = 0.8
pchange = 0.2
pchange_slow = 0.4
psurr = 0.1
psurr_slow = 0.2
gradient = -10
factor = 1500


if __name__ == "__main__":
    trafficC.run_simulation_parallel(30,gradient, factor, p, p0, pchange, pchange_slow, psurr, psurr_slow, True)
    #trafficC.run_simulation(gradient, factor, p, p0, pchange, pchange_slow, psurr, psurr_slow)
