#include <iostream>
#include <fstream>
#include <vector>

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN
typedef IloArray<IloNumArray> DataMatrix;

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

void setData(const IloEnv env, char nom_fichier[]) {
    char readChar;
    int readInt;
    float readFloat, readFloat2;
    std::ifstream fichier(nom_fichier);
    if(fichier) {
        fichier >> readChar >> readChar >> n >> readChar;
        fichier >> readChar >> L >> readChar;
        fichier >> readChar >> W >> readChar;
        fichier >> readChar >> K >> readChar;
        fichier >> readChar >> B >> readChar;

        w_v = IloNumArray(env);
        fichier >> readChar >> readChar >> readChar;
        for (unsigned int i = 0; i < n; i++){
            fichier >> readChar >> readInt;
            w_v.add(readInt);
        }

        W_v = IloNumArray(env);
        fichier >> readChar >> readChar >> readChar >> readChar >> readChar;
        for (unsigned int i = 0; i < n; i++){
            fichier >> readChar >> readFloat;
            W_v.add(readFloat);
        }

        lh = IloNumArray(env);
        fichier >> readChar >> readChar >> readChar >> readChar;
        for (unsigned int i = 0; i < n; i++){
            fichier >> readChar >> readInt;
            lh.add(readInt);
        }

        coordinates = std::vector<std::pair<float, float> >(0);
        for (unsigned int i = 0; i < 13; i++) fichier >> readChar;
//        std::cout << readChar << std::endl;
        for (unsigned int i = 0; i < n; i++){
            fichier >> readChar >> readFloat >> readFloat2;
            coordinates.push_back(std::pair<float, float>(readFloat, readFloat2));
        }

//        std::cout << coordinates[0].first << std::endl;
//        std::cout << coordinates[9].second << std::endl;

        l = IloNumArray(env);
        for (unsigned int i = 0; i < n; i++)
            for (unsigned int j = 0; j < n; j++)
                l.add(pow(coordinates[i].first - coordinates[j].first, 2) + pow(coordinates[i].second - coordinates[j].second, 2));

        fichier.close();
    }
    else{
        std::cout << "pas de fichier "<< nom_fichier << std::endl;
    }
}

int main (void) {
    IloEnv env;
    char saisie[] = "../data/10_ulysses_9.tsp";
    IloModel model(env);

    try {

        // cout << "Veuillez saisir le nom du fichier de donn�es : ";
        // cin >> saisie;

        setData(env, saisie);

        IloBoolVarArray x(env, K * n);
        IloBoolVarArray y(env, n * n);
        IloNumVar a(env, 0.0, IloInfinity);
        IloNumVarArray z(env, n * n);
        IloNumVar b(env, 0.0, IloInfinity);
        IloNumVarArray s(env, n);

        for (unsigned int j = 0; j < n; j++) {
            for (unsigned int i = 0; i < n; i++) {
                z[i + n * j] = IloNumVar(env, 0.0 , IloInfinity);
            }
            s[j] = IloNumVar(env, 0.0 , IloInfinity);
        }

        // objective
        IloExpr obj(env);
        for (unsigned int j = 0; j < n; j++) {
            for(unsigned int i = 0; i < n; i++) {
                obj += 3 * z[i + n * j];
                obj += y[i + n * j] * l[i + n * j];
            }
        }
        obj += a * L;
        model.add(IloMinimize(env, obj));
        obj.end();

        // constraints
        for(unsigned int i = 0; i < n; i++) {
            for (unsigned int j = 0; j < n; j++) {
                for (unsigned int k = 0; k < K; k++){
                    model.add(y[i + n * j] >= x[i * K + k] + x[j * K + k] - 1);
                }
                model.add(a + z[i + n * j] >= (lh[i] + lh[j]) * y[i + n * j]);
            }
            for (unsigned int k = 0; k < K; k++)
                model.add(b + s[i] >= x[i * K + k] * w_v[i]);
        }
        for (unsigned int k = 0; k < K; k++){
            obj = IloExpr(env);
            obj += b * W;
            for (unsigned int i = 0; i < n; i ++) {
                obj += s[i] * W_v[i];
                obj += x[i * K + k] * w_v[i];
            }
            model.add(obj <= B);
            obj.end();
        }
        for (unsigned int k = 0; k < K; k++){
            obj = IloExpr(env);
            for (unsigned int i = 0; i < n; i ++) {
                obj += x[i * K + k];
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
