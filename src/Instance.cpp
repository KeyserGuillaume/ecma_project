#include "Instance.h"

Instance::Instance(const IloEnv &env, char *filename){
    char readChar;
    int readInt;
    float readFloat, readFloat2;
    std::ifstream file(filename);
    if(!file) throw std::runtime_error("File was not found");
    file >> readChar >> readChar >> n >> readChar;
    file >> readChar >> L >> readChar;
    file >> readChar >> W >> readChar;
    file >> readChar >> K >> readChar;
    file >> readChar >> B >> readChar;

    w_v = IloNumArray(env);
    file >> readChar >> readChar >> readChar;
    for (unsigned int i = 0; i < n; i++){
        file >> readChar >> readInt;
        w_v.add(readInt);
    }

    W_v = IloNumArray(env);
    file >> readChar >> readChar >> readChar >> readChar >> readChar;
    for (unsigned int i = 0; i < n; i++){
        file >> readChar >> readFloat;
        W_v.add(readFloat);
    }

    lh = IloNumArray(env);
    file >> readChar >> readChar >> readChar >> readChar;
    for (unsigned int i = 0; i < n; i++){
        file >> readChar >> readInt;
        lh.add(readInt);
    }

    coordinates = std::vector<std::pair<float, float> >(0);
    for (unsigned int i = 0; i < 13; i++) file >> readChar;
//        std::cout << readChar << std::endl;
    for (unsigned int i = 0; i < n; i++){
        file >> readChar >> readFloat >> readFloat2;
        coordinates.push_back(std::pair<float, float>(readFloat, readFloat2));
    }

//        std::cout << coordinates[0].first << std::endl;
//        std::cout << coordinates[9].second << std::endl;

    l = IloArray<IloNumArray>(env);
    for (unsigned int i = 0; i < n; i++){
        l.add(IloNumArray(env));
        for (unsigned int j = 0; j < n; j++)
            l[i].add(sqrt(pow(coordinates[i].first - coordinates[j].first, 2) + pow(coordinates[i].second - coordinates[j].second, 2)));
    }

    file.close();

}

void Instance::print_distances() const {
    // not beautiful
    std::cout << "distances: " << std::endl;
    for (unsigned int i = 0; i < n; i++){
        for (unsigned int j = 0; j < n; j++){
            if (i < j)
                std::cout << l[i][j] << ", ";
            else
                std::cout << " , ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
