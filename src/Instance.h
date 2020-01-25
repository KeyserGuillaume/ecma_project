#include <iostream>
#include <fstream>
#include <vector>

#pragma once

#include <ilcplex/ilocplex.h>

typedef IloArray<IloNumArray> DataMatrix;

class Instance {
public:
    IloInt n;
    IloInt L;
    IloInt W;
    IloInt K;
    IloInt B;

    IloNumArray w_v;
    IloNumArray W_v;
    IloNumArray l;
    IloNumArray lh;
    std::vector<std::pair<float, float> > coordinates;

    Instance(const IloEnv &env, char filename[]);
};
