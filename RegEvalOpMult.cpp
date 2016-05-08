#include <ecf/ECF.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "RegEvalOpMult.h"

FitnessP RegEvalOpMult::evaluate(IndividualP individual) {

    FitnessP fitness(new FitnessMin);
    
    double value = 0;

    //pairs of (className, result of executing tree for current example)
    std::vector<std::pair<std::string, double>> values;

    predicted.clear();

    // evaluate each example
    for (uint i = 0; i < nSamples; i++) {
        std::vector<double> v = domain.at(i);
        values.clear();

        // execute example for every tree
        for (uint j = 0; j < classesNum; j++) {
            Tree::Tree *tree = (Tree::Tree *) individual->getGenotype(j).get();
            
            for (uint k = 0; k < v.size(); k++) {
                std::string terminal = "X" + std::to_string(k);
                tree->setTerminalValue(terminal, &v[k]);
            }

            double result;
            tree->execute(&result);
            values.push_back(std::make_pair(std::to_string(j), result));

        }

        value += fitnessInc(values, i);
    }

    fitness->setValue(value);
    std::cout << *this;

    return fitness;
}

double RegEvalOpMult::fitnessInc(std::vector<std::pair<std::string, double>> values, uint index) {

    // transform result of each tree to it's absolute value
    std::transform(values.begin(), values.end(), values.begin(), &getAbs); 
            
    // find minimum tree result
    std::pair<std::string, double> selected = values.at(0);

    for (auto &pair : values) {
        if (pair.second < selected.second) {
            selected = pair;
        }
    }

    if (selected.first.compare(codomain.at(index)) == 0) {
        return 0;
    }

    return 1;

}

std::pair<std::string, double> RegEvalOpMult::getAbs(std::pair<std::string, double> pair) {

    return std::make_pair(pair.first, std::abs(pair.second));

};
