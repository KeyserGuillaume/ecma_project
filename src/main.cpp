#include "Instance.h"
#include "Dualization.h"
#include "cutting_plane.h"
#include "branch_cut.h"

#include <iostream>

ILOSTLBEGIN

int main (int argc, char* argv[]){
   IloEnv env;
//    string filename = "../data/test.tsp";
//    string filename = "../data/10_ulysses_9.tsp";
//    string filename = "../data/22_ulysses_3.tsp";
//    string filename = "../data/52_berlin_6.tsp";
   string filename = "../data/10_ulysses_9.tsp";

   string method = argv[1];
   if (argc > 2)
      filename = argv[2];
   std::cout << filename << std::endl;
   Instance I(env, filename.c_str());

   if (method == "DUAL") {
      solve_dualized_problem(env, I);
   } else if (method == "CUTTING_PLANES") {
      solve_problem_with_cutting_planes(env, I);
   } else if (method == "BRANCH_AND_CUT") {
      solve_problem_with_branch_cut(env, I);
   } else if (method == "HEURISTIC") {

   } else {
      std::cout << "First parameter should be DUAL or CUTTING_PLANES or BRANCH_AND_CUT or HEURISTIC, not " << method << std::endl;
   }

    return 0;
}
