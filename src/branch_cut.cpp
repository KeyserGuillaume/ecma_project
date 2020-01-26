#include "branch_cut.h"

struct SlaveCallback : IloCplex::LazyConstraintCallbackI {
  IloEnv& env;
  const MasterVariables& master_var;
  const Instance& I;
  IloEnv slaveEnv;

  SlaveCallback(IloEnv& env, const MasterVariables& master_var, const Instance& I) :
    LazyConstraintCallbackI{env}, env{env}, master_var{master_var}, I{I} {}

  [[nodiscard]] IloCplex::CallbackI* duplicateCallback() const override {
    return new(getEnv()) SlaveCallback{*this};
  }

  void main() override {
    Slave1Variables slave1_var(slaveEnv, I);
    Slave2Variables slave2_var(slaveEnv, I);

    IloArray<IloNumArray> y(slaveEnv, I.n);
    for (unsigned int i=0; i<I.n; i++) {
        y[i] = IloNumArray(slaveEnv, I.n);
    }
    IloArray<IloNumArray> x(slaveEnv, I.n);
    for (unsigned int i=0; i<I.n; i++) {
        x[i] = IloNumArray(slaveEnv, I.K);
    }

    IloNum z;

    for (unsigned int i=0; i<I.n; i++) {
        for (unsigned int j=i+1; j<I.n; j++) {
            y[i][j] = getValue(master_var.y[i][j]);
        }
    }
    for (unsigned int i=0; i<I.n; i++) {
        for (unsigned int k=0; k<I.K; k++) {
            x[i][k] = getValue(master_var.x[i][k]);
        }
    }
    z = getValue(master_var.z);

    bool enriching1;
    IloArray<IloNumArray> delta1 = solve_slave_problem_1(slaveEnv, I, slave1_var, y, z, enriching1);
    if (enriching1) {// set by line above
      IloExpr expr(env);
      for (unsigned int i = 0; i < I.n; i++) {
        for (unsigned int j = i + 1; j < I.n; j++) {
          expr += delta1[i][j] * (I.lh[i] + I.lh[j]) * master_var.y[i][j];
        }
      }
      add(master_var.z >= expr);
      expr.end();
      // std::cout << "Adding U1" << std::endl;
    }

    for (unsigned int k=0; k<I.K;k++) {
      bool enriching2;
      IloNumArray delta2 = solve_slave_problem_2(slaveEnv, I, slave2_var, x, enriching2, k);
      if (enriching2) { // set by line above
        for (unsigned int k = 0; k < I.K; k++) {
          IloExpr expr(env);
          for (unsigned int i = 0; i < I.n; i++) {
            expr += I.w_v[i] * (1 + delta2[i]) * master_var.x[i][k];
          }
          add(expr <= I.B);
          expr.end();
        }
        // std::cout << "Adding U2" << std::endl;
      }
    }
  }
};


void solve_problem_with_branch_cut(IloEnv &env, const Instance &I) {
  MasterVariables master_var(env, I);
  Slave1Variables slave1_var(env, I);
  Slave2Variables slave2_var(env, I);
  std::vector<IloArray<IloNumArray>> U1(0);
  std::vector<IloNumArray> U2(0);

  IloModel master_model = get_master_model(env, I, U1, U2, master_var);
  IloCplex master_cplex(master_model);
  IloCplex::LazyConstraintCallbackI* cb = new SlaveCallback(env, master_var, I);
  master_cplex.use(cb);
  // master_cplex.setOut(env.getNullStream()); // tell cplex to be silent
  master_cplex.solve();
  if (master_cplex.getStatus() == IloAlgorithm::Infeasible) {
      std::cout << "No Solution" << std::endl;
      return;
  }
  // double obj = master_cplex.getObjValue();
  std::cout << "\t Obj " << master_cplex.getObjValue() << std::endl;

}
