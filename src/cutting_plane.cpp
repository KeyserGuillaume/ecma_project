#include "cutting_plane.h"

MasterVariables::MasterVariables(const IloEnv &env, const Instance& I) {
    x = IloArray<IloBoolVarArray> (env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        x[i] = IloBoolVarArray(env, I.K);

    y = IloArray<IloBoolVarArray> (env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        y[i] = IloBoolVarArray(env, I.n);

    z = IloNumVar (env, 0.0, IloInfinity);
}

Slave1Variables::Slave1Variables(const IloEnv &env, const Instance& I) {
    delta1 = IloArray<IloNumVarArray> (env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        delta1[i] = IloNumVarArray(env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        for (unsigned int j = i + 1; j < I.n; j++)
            delta1[i][j] = IloNumVar(env, 0.0, 3);
}

Slave2Variables::Slave2Variables(const IloEnv &env, const Instance& I) {
    delta2 = IloNumVarArray (env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        delta2[i] = IloNumVar(env, 0.0, I.W_v[i]);
}

void solve_problem_with_cutting_planes(IloEnv &env, const Instance &I) {
    MasterVariables master_var(env, I);
    Slave1Variables slave1_var(env, I);
    Slave2Variables slave2_var(env, I);
    std::vector<IloArray<IloNumArray>> U1(0);
    std::vector<IloNumArray> U2(0);

    double obj;
    bool enriching = true;
    unsigned int iter = 0;

    IloArray<IloNumArray> y(env, I.n);
    for (unsigned int i=0; i<I.n; i++) {
        y[i] = IloNumArray(env, I.n);
    }
    IloArray<IloNumArray> x(env, I.n);
    for (unsigned int i=0; i<I.n; i++) {
        x[i] = IloNumArray(env, I.K);
    }

    IloNum z;

    while(enriching) {
        enriching = false;
        IloModel master_model = get_master_model(env, I, U1, U2, master_var);
        IloCplex master_cplex(master_model);
        master_cplex.setOut(env.getNullStream()); // tell cplex to be silent
        master_cplex.solve();
        if (master_cplex.getStatus() == IloAlgorithm::Infeasible) {
            std::cout << "No Solution" << std::endl;
            return;
        }
        obj = master_cplex.getObjValue();
        std::cout << "\tIter " << iter << "\t Obj " << master_cplex.getObjValue() << std::endl;

        for (unsigned int i=0; i<I.n; i++) {
            for (unsigned int j=i+1; j<I.n; j++) {
                y[i][j] = master_cplex.getValue(master_var.y[i][j]);
            }
        }
        for (unsigned int i=0; i<I.n; i++) {
            for (unsigned int k=0; k<I.K; k++) {
                x[i][k] = master_cplex.getValue(master_var.x[i][k]);
            }
        }
        z = master_cplex.getValue(master_var.z);

        bool enriching1 = true;
        IloArray<IloNumArray> delta1 = solve_slave_problem_1(env, I, slave1_var, y, z, enriching1);
        if (enriching1) {// set by line above
            U1.push_back(delta1);
            enriching = true;
            std::cout << "Adding U1" << std::endl;
        }

        for (unsigned int k=0; k<I.K;k++) {
            bool enriching2 = true;
            IloNumArray delta2 = solve_slave_problem_2(env, I, slave2_var, x, enriching2, k);
            if (enriching2) { // set by line above
                U2.push_back(delta2);
                enriching = true;
                std::cout << "Adding U2" << std::endl;
            }
        }
        iter++;
    }

    std::cout << "Solved (obj "<< obj << ") in " << iter << " iterations by adding " << U1.size() << " cuts with pb 1 and " << U2.size() << " with pb 2" << std::endl;
}

IloModel get_master_model(IloEnv &env, const Instance& I, const std::vector<IloArray<IloNumArray>> U1,
                          const std::vector<IloNumArray> U2, const MasterVariables &var){
    IloModel master_model(env);

    // objective
    IloExpr obj(env);
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
            obj += var.y[i][j] * I.l[i][j];
        }
    }
    obj += var.z;
    master_model.add(IloMinimize(env, obj));
    obj.end();

    // constraints
    for (unsigned int m = 0; m < U2.size(); m++) {
        for (unsigned int k = 0; k < I.K; k++) {
            IloExpr expr(env);
            for (unsigned int i = 0; i < I.n; i++)
                expr += I.w_v[i] * (1 + U2[m][i]) * var.x[i][k];
            master_model.add(expr <= I.B);
            expr.end();
        }
    }

    for (unsigned int m = 0; m < U1.size(); m++) {
        IloExpr expr (env);
        for (unsigned int i = 0; i < I.n; i++)
            for (unsigned int j = i + 1; j < I.n; j++)
                expr += U1[m][i][j] * (I.lh[i] + I.lh[j]) * var.y[i][j];
        master_model.add(var.z >= expr);
        expr.end();
    }

    for (unsigned int i = 0; i < I.n; i++)
        for (unsigned int j = i + 1; j < I.n; j++)
            for (unsigned int k = 0; k < I.K; k++)
                master_model.add(var.y[i][j] >= var.x[i][k] + var.x[j][k] - 1);

    for (unsigned int i = 0; i < I.n; i++) {
        IloExpr expr (env);
        for (unsigned int k = 0; k < I.K; k++)
            expr += var.x[i][k];
        master_model.add(expr == 1);
        expr.end();
    }

    return master_model;
}


IloArray<IloNumArray> solve_slave_problem_1(const IloEnv &env, const Instance &I,
                                            const Slave1Variables &var, const IloArray<IloNumArray>& y,
                                            const IloNum& z, bool& need_to_enrich){
    IloModel model(env);

    // objective
    IloExpr obj(env);
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
            obj += var.delta1[i][j] * (I.lh[i] + I.lh[j]) * y[i][j];
        }
    }
    model.add(IloMaximize(env, obj));
    obj.end();
    // constraints
    IloExpr expr(env);
    for (unsigned int i = 0; i < I.n; i++)
        for (unsigned int j = i + 1; j < I.n; j++)
            expr += var.delta1[i][j];
    model.add(expr <= I.L);
    expr.end();
    IloCplex slave1_cplex(model);
    slave1_cplex.setOut(env.getNullStream());
    slave1_cplex.solve();
    if (slave1_cplex.getObjValue() <= z) {
        need_to_enrich = false;
        return IloArray<IloNumArray>(0);
    } else {
        need_to_enrich = true;
        IloArray<IloNumArray> delta1 (env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            delta1[i] = IloNumArray(env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            for (unsigned int j = i + 1; j < I.n; j++)
                delta1[i][j] = slave1_cplex.getValue(var.delta1[i][j]);
        return delta1;
    }
}

IloNumArray solve_slave_problem_2(const IloEnv &env, const Instance &I,
                                            const Slave2Variables &var, const IloArray<IloNumArray>& x,
                                            bool& need_to_enrich, unsigned int cluster_index) {

    IloModel model(env);
    IloExpr obj(env);
    for (unsigned int i = 0; i < I.n; i++) {
        obj += I.w_v[i] * (1 + var.delta2[i]) * x[i][cluster_index];
    }
    model.add(IloMaximize(env, obj));
    obj.end();

    // constraints
    IloExpr expr(env);
    for (unsigned int i = 0; i < I.n; i++)
        expr += var.delta2[i];
    model.add(expr <= I.W);
    expr.end();

    IloCplex slave2_cplex(model);
    slave2_cplex.setOut(env.getNullStream());
    slave2_cplex.solve();

    if (slave2_cplex.getObjValue() <= I.B) {
        need_to_enrich = false;
        return IloNumArray(0);
    } else {
        need_to_enrich = true;
        IloNumArray delta2(env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            delta2[i] = slave2_cplex.getValue(var.delta2[i]);
        return delta2;
    }
}
