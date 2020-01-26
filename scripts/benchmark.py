import os
import subprocess

exe_path = '../Debug/ecma'
data_directory = '../data/'

instances = os.listdir(data_directory)

for instance in instances:
    print(instance)
    subprocess.call("{} {} {} | grep Solved".format(exe_path, "CUTTING_PLANES", data_directory + instance), shell=True)
    # subprocess.call("{} {} {} | grep Solved".format(exe_path, "DUAL", data_directory + instance), shell=True)
