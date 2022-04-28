import trafficC
import numpy as np



p = 0.2
p0 = 0.7
pchange = 0.2
pchange_slow = 0.4
psurr = 0.1
psurr_slow = 0.2
gradient = 100
density = 50


if __name__ == "__main__":
    trafficC.run_simulation(gradient, density, p, p0, pchange, pchange_slow, psurr, psurr_slow)
