#include <iostream>
#include <fstream>
#include <vector>

#pragma once

#include <ilcplex/ilocplex.h>

class Instance {
public:
    IloInt n;
    IloInt L;
    IloInt W;
    IloInt K;
    IloInt B;

    IloNumArray w_v;
    IloNumArray W_v;
    IloNumArray lh;
    IloArray<IloNumArray> l;
    std::vector<std::pair<float, float> > coordinates;

    Instance(const IloEnv &env, const char* filename);
    void print_distances() const;
};
