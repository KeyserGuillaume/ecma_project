#include "Instance.h"

ILOSTLBEGIN

int main (void) {
    IloEnv env;
    char filename[] = "../data/10_ulysses_9.tsp";
    Instance I(env, filename);
    IloModel model(env);

    try {

        IloBoolVarArray x(env, I.K * I.n);
        IloBoolVarArray y(env, I.n * I.n);
        IloNumVar a(env, 0.0, IloInfinity);
        IloNumVarArray z(env, I.n * I.n);
        IloNumVar b(env, 0.0, IloInfinity);
        IloNumVarArray s(env, I.n);

        for (unsigned int j = 0; j < I.n; j++) {
            for (unsigned int i = 0; i < I.n; i++) {
                z[i + I.n * j] = IloNumVar(env, 0.0 , IloInfinity);
            }
            s[j] = IloNumVar(env, 0.0 , IloInfinity);
        }

        // objective
        IloExpr obj(env);
        for (unsigned int j = 0; j < I.n; j++) {
            for(unsigned int i = 0; i < I.n; i++) {
                obj += 3 * z[i + I.n * j];
                obj += y[i + I.n * j] * I.l[i + I.n * j];
            }
        }
        obj += a * I.L;
        model.add(IloMinimize(env, obj));
        obj.end();

        // constraints
        for(unsigned int i = 0; i < I.n; i++) {
            for (unsigned int j = 0; j < I.n; j++) {
                for (unsigned int k = 0; k < I.K; k++){
                    model.add(y[i + I.n * j] >= x[i * I.K + k] + x[j * I.K + k] - 1);
                }
                model.add(a + z[i + I.n * j] >= (I.lh[i] + I.lh[j]) * y[i + I.n * j]);
            }
            for (unsigned int k = 0; k < I.K; k++)
                model.add(b + s[i] >= x[i * I.K + k] * I.w_v[i]);
        }
        for (unsigned int k = 0; k < I.K; k++){
            obj = IloExpr(env);
            obj += b * I.W;
            for (unsigned int i = 0; i < I.n; i ++) {
                obj += s[i] * I.W_v[i];
                obj += x[i * I.K + k] * I.w_v[i];
            }
            model.add(obj <= I.B);
            obj.end();
        }
        for (unsigned int k = 0; k < I.K; k++){
            obj = IloExpr(env);
            for (unsigned int i = 0; i < I.n; i ++) {
                obj += x[i * I.K + k];
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
