#include "Instance.h"
#include "Dualization.h"
#include "cutting_plane.h"

ILOSTLBEGIN

int main (void){
    IloEnv env;
//    char filename[] = "../data/test.tsp";
    char filename[] = "../data/10_ulysses_9.tsp";
//    char filename[] = "../data/22_ulysses_3.tsp";
//    char filename[] = "../data/52_berlin_6.tsp";
    Instance I(env, filename);

//    solve_dualized_problem(env, I);
    solve_problem_with_cutting_planes(env, I);

    return 0;
}
