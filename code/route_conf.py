Dt=1.0     # This is the time unit
Dx=1.0     # This is the space unit meters/cell
Db=4.0     # The car length, in m
vmax=17          # the default maximal speed, in m/s
acc = 1    # the acceleration in Dx/Dt^2
tpen = 5 # the number of seconds that a car that has changed lane must stay in that lane
Nmax = 5  # the maximal number of lanes
Carmax = 2000 # the maximal number of cars in the system
DL = 5000 # the total length of the corridor, in m
Nparam = 16 # the number of parameters per car
gradient = 30 # The length of the lane dissapareance
endgrad = 4000 # The position of the gradient
vthres = 4 # the speed changing the low speed rules from high speed rules
xmin = endgrad-1000 # the minimal position where the speed measurements are taken
xmax = endgrad+200 # the maximal position where the speed measurements are taken


