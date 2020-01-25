#include "Dualization.h"

void solve_dualized_problem(IloEnv &env, const Instance &I) {
    IloModel model(env);

    IloArray<IloBoolVarArray> x(env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        x[i] = IloBoolVarArray(env, I.K);

    IloArray<IloBoolVarArray> y(env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        y[i] = IloBoolVarArray(env, I.n);

    IloArray<IloNumVarArray> z(env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        z[i] = IloNumVarArray(env, I.n);

    IloArray<IloNumVarArray> s(env, I.n);
    for (unsigned int i = 0; i < I.n; i++)
        s[i] = IloNumVarArray(env, I.K);

    IloNumVar a(env, 0.0, IloInfinity);

    IloNumVarArray b(env, I.K);
    for (unsigned int k = 0; k < I.K; k++)
        b[k] = IloNumVar(env, 0.0, IloInfinity);

    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
            z[i][j] = IloNumVar(env, 0.0, IloInfinity);
        }
        for (unsigned int k = 0; k < I.K; k++)
            s[i][k] = IloNumVar(env, 0.0, IloInfinity);
    }

    // objective
    IloExpr obj(env);
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
            obj += 3 * z[i][j];
            obj += y[i][j] * I.l[i][j];
        }
    }
    obj += a * I.L;
    model.add(IloMinimize(env, obj));
    obj.end();

    // constraints
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
            for (unsigned int k = 0; k < I.K; k++) {
                model.add(y[i][j] >= x[i][k] + x[j][k] - 1);
            }
            model.add(a + z[i][j] >= (I.lh[i] + I.lh[j]) * y[i][j]);
        }
        for (unsigned int k = 0; k < I.K; k++)
            model.add(b[k] + s[i][k] >= x[i][k] * I.w_v[i]);
    }
    for (unsigned int k = 0; k < I.K; k++) {
        obj = IloExpr(env);
        obj += b[k] * I.W;
        for (unsigned int i = 0; i < I.n; i++) {
            obj += s[i][k] * I.W_v[i] + x[i][k] * I.w_v[i];
        }
        model.add(obj <= I.B);
        obj.end();
    }
    for (unsigned int i = 0; i < I.n; i++) {
        obj = IloExpr(env);
        for (unsigned int k = 0; k < I.K; k++) {
            obj += x[i][k];
        }
        model.add(obj == 1);
        obj.end();
    }


    // solve
    IloCplex cplex(model);
    cplex.solve();

    // print stuff
    if (cplex.getStatus() == IloAlgorithm::Infeasible) {
        std::cout << "No Solution" << std::endl;
        return;
    }

    // Afficher la valeur de l'objectif
    std::cout << "objective: " << cplex.getObjValue() << std::endl;

    std::cout << "x: " << std::endl;
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int k = 0; k < I.K; k++)
            std::cout << (unsigned int) (cplex.getValue(x[i][k])) << ", ";
        std::cout << std::endl;
    }

    std::cout << "y: " << std::endl;
    for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = 0; j < I.n; j++) {
            if (i < j)
                std::cout << (unsigned int) (cplex.getValue(y[i][j])) << ", ";
            else
                std::cout << " , ";
        }
        std::cout << std::endl;
    }

    std::cout << "a: " << cplex.getValue(a) << std::endl;

    std::cout << std::endl;
//    I.print_distances();

    env.end();
}