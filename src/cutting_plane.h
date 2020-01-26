#pragma once
#include "Instance.h"

class MasterVariables{
public:
    IloArray<IloBoolVarArray> x;
    IloArray<IloBoolVarArray> y;
    IloNumVar z;
    MasterVariables(const IloEnv& env, const Instance& I); // why does he permit me to set env as const here ...?
};

class Slave1Variables{
public:
    IloArray<IloNumVarArray> delta1;
    Slave1Variables(const IloEnv& env, const Instance& I); // why does he permit me to set env as const here ...?
};

class Slave2Variables{
public:
    IloNumVarArray delta2;
    Slave2Variables(const IloEnv& env, const Instance& I); // why does he permit me to set env as const here ...?
};


void solve_problem_with_cutting_planes(IloEnv &env, const Instance &I);
IloModel get_master_model(IloEnv &env, const Instance& I, const std::vector<IloArray<IloNumArray>> U1,
                          const std::vector<IloNumArray> U2, const MasterVariables &var);
IloArray<IloNumArray> solve_slave_problem_1(const IloEnv &env, const Instance &I,
                                            const Slave1Variables &var, const IloArray<IloNumArray>& y,
                                            const IloNum& z, bool& need_to_enrich);
IloNumArray solve_slave_problem_2(const IloEnv &env, const Instance &I,
                                  const Slave2Variables &var, const IloArray<IloNumArray>& x,
                                  bool& need_to_enrich, unsigned int cluster_index);
