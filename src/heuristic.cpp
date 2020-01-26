#include "heuristic.h"

void solve_problem_with_heuristic(IloEnv &env, const Instance &I) {
  MasterVariables var(env, I);
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
  for (unsigned int k = 0; k < I.K; k++) {
    IloExpr expr(env);
    for (unsigned int i = 0; i < I.n; i++)
        expr += I.w_v[i] * (1 + I.W_v[i]) * var.x[i][k];
    master_model.add(expr <= I.B);
    expr.end();
  }

  IloExpr expr (env);
  for (unsigned int i = 0; i < I.n; i++) {
    for (unsigned int j = i + 1; j < I.n; j++) {
      expr += 3 * (I.lh[i] + I.lh[j]) * var.y[i][j];
    }
  }
  master_model.add(var.z >= expr);
  expr.end();

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

  IloCplex master_cplex(master_model);
  master_cplex.setOut(env.getNullStream()); // tell cplex to be silent
  master_cplex.solve();
  std::cout << "Solved objective: " << master_cplex.getObjValue() << std::endl;

}
