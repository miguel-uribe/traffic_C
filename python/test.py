import createFiles
import filecmp
import subprocess
import os
import numpy as np

for gradient in [10,40,70,100,130,160]:
    for density in np.arange(20,200,10):
        # we start by creating the files with the lane configuration
        confname = createFiles.createSystemFiles(density, gradient)

        # once the files are created we compile the cpp script if there are changes
        dirname = os.path.dirname(__file__)
        dirname = os.path.join(dirname,'../cpp/')
        #if (not filecmp.cmp(dirname+'parameters.h',dirname+'parameters.bck')):
        print('Recompiling')
        comp = subprocess.run(['g++','-O2','simulation.cpp','-o','simulation_'+confname], cwd=dirname)

        # we run the command
        for seed in np.arange(30):
            subprocess.run(['./simulation_'+confname,str(seed), str(density)], cwd=dirname)