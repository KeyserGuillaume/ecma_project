import os
import subprocess
import time

# run your code


exe_path = '../Debug/ecma'
data_directory = '../data/'

instances = os.listdir(data_directory)

for instance in instances:
    print(instance)
    print("echo {} {} {} >> out.txt".format(exe_path, "CUTTING_PLANES", data_directory + instance))
    subprocess.call("echo {} {} {} >> out.txt".format(exe_path, "CUTTING_PLANES", data_directory + instance), shell=True)

    start = time.time()
    subprocess.call("{} {} {} >> out.txt".format(exe_path, "CUTTING_PLANES", data_directory + instance), shell=True)
    end = time.time()
    elapsed = end - start
    subprocess.call("echo Elapsed time {} >> out.txt".format(elapsed), shell=True)
    subprocess.call("echo ================================================== >> out.txt".format(elapsed), shell=True)


    subprocess.call("echo {} {} {} >> out.txt".format(exe_path, "HEURISTIC", data_directory + instance), shell=True)
    start = time.time()
    subprocess.call("{} {} {} >> out.txt".format(exe_path, "HEURISTIC", data_directory + instance), shell=True)
    end = time.time()
    elapsed = end - start
    subprocess.call("echo Elapsed time {} >> out.txt".format(elapsed), shell=True)
    subprocess.call("echo ================================================== >> out.txt".format(elapsed), shell=True)

    subprocess.call("echo {} {} {} >> out.txt".format(exe_path, "DUAL", data_directory + instance), shell=True)
    start = time.time()
    subprocess.call("{} {} {} >> out.txt".format(exe_path, "DUAL", data_directory + instance), shell=True)
    end = time.time()
    elapsed = end - start
    subprocess.call("echo Elapsed time {} >> out.txt".format(elapsed), shell=True)
    subprocess.call("echo ================================================== >> out.txt".format(elapsed), shell=True)

    subprocess.call("echo {} {} {} >> out.txt".format(exe_path, "BRANCH_AND_CUT", data_directory + instance), shell=True)
    start = time.time()
    subprocess.call("{} {} {} >> out.txt".format(exe_path, "BRANCH_AND_CUT", data_directory + instance), shell=True)
    end = time.time()
    elapsed = end - start
    subprocess.call("echo Elapsed time {} >> out.txt".format(elapsed), shell=True)
    subprocess.call("echo ================================================== >> out.txt".format(elapsed), shell=True)
