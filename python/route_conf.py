Dt=1.0     # This is the time unit
Dx=1.0     # This is the space unit meters/cell
Db=4.0     # The car length, in m
vmax=17          # the default maximal speed, in m/s
acc = 1    # the acceleration in Dx/Dt^2
p = 0.2          # The random breaking probability
p0 = 0.5     # the breaking probability when the vehicle is stopped
pchange = 0.5 # the probability to change the lanes when possible
psurr = 1  # the probability to surrender to a stopped car trying to change lane
tpen = 5 # the number of seconds that a car that has surrended pirority will remain stopped
Nmax = 6  # the maximal number of lanes
DL = 5000 # the total length of the corridor, in m
Nparam = 16 # the number of parameters per car
gradient = 30 # The length of the lane dissapareance
endgrad = 4000 # The position of the gradient

