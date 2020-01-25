#include "Instance.h"

ILOSTLBEGIN

int main (void) {
    IloEnv env;
    char filename[] = "../data/10_ulysses_9.tsp";
    Instance I(env, filename);
    IloModel model(env);

    try {
        IloArray<IloBoolVarArray> x(env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            x[i] = IloBoolVarArray(env, I.K);

        IloArray<IloBoolVarArray> y(env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            y[i] = IloBoolVarArray(env, I.n);

        IloArray<IloNumVarArray> z(env, I.n);
        for (unsigned int i = 0; i < I.n; i++)
            z[i] = IloNumVarArray(env, I.n);

        IloNumVar a(env, 0.0, IloInfinity);
        IloNumVar b(env, 0.0, IloInfinity);
        IloNumVarArray s(env, I.n);

        for (unsigned int j = 0; j < I.n; j++) {
            for (unsigned int i = 0; i < I.n; i++) {
                z[i][j] = IloNumVar(env, 0.0 , IloInfinity);
            }
            s[j] = IloNumVar(env, 0.0 , IloInfinity);
        }

        // objective
        IloExpr obj(env);
        for (unsigned int j = 0; j < I.n; j++) {
            for(unsigned int i = 0; i < I.n; i++) {
                obj += 3 * z[i][j];
                obj += y[i][j] * I.l[i][j];
            }
        }
        obj += a * I.L;
        model.add(IloMinimize(env, obj));
        obj.end();

        // constraints
        for(unsigned int i = 0; i < I.n; i++) {
            for (unsigned int j = 0; j < I.n; j++) {
                for (unsigned int k = 0; k < I.K; k++){
                    model.add(y[i][j] >= x[i][k] + x[j][k] - 1);
                }
                model.add(a + z[i][j] >= (I.lh[i] + I.lh[j]) * y[i][j]);
            }
            for (unsigned int k = 0; k < I.K; k++)
                model.add(b + s[i] >= x[i][k] * I.w_v[i]);
        }
        for (unsigned int k = 0; k < I.K; k++){
            obj = IloExpr(env);
            obj += b * I.W;
            for (unsigned int i = 0; i < I.n; i ++) {
                obj += s[i] * I.W_v[i];
                obj += x[i][k] * I.w_v[i];
            }
            model.add(obj <= I.B);
            obj.end();
        }
        for (unsigned int k = 0; k < I.K; k++){
            obj = IloExpr(env);
            for (unsigned int i = 0; i < I.n; i ++) {
                obj += x[i][k];
            }
            model.add(obj == 1);
            obj.end();
        }


        // ******************
        // Resolution
        // ******************
        IloCplex cplex(model);
        cplex.solve();

        // ******************
        // Affichage des resultats
        // ******************

        // Si le resultat est infaisable
        if (cplex.getStatus() == IloAlgorithm::Infeasible)
            cout << "No Solution" << endl;

            // Si le resultat est faisable
        else{

            // Afficher la valeur de l'objectif
            cout << "objective: " << cplex.getObjValue() << endl;
//
//            cout << "x: ";
//            for (int i = 0; i < n; i++)
//                cout << cplex.getValue(x[i]) << ", ";
//            cout << endl;
        }

    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }

    env.end();
    return 0;

}
