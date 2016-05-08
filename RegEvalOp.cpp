#include <ecf/ECF.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "RegEvalOp.h"

std::ostream &operator<<(std::ostream &os, const RegEvalOp &reg) {
    uint total = 0;

    for (uint i = 0; i < reg.nSamples; i++) {
        if (reg.codomain.at(i).compare(reg.predicted.at(i)) == 0) {
            total += 1;
        }
    }
    os << "Total: " << total << " / " << reg.nSamples << std::endl;

    return os;

}

void RegEvalOp::registerParameters(StateP state) {

    state->getRegistry()->registerEntry("learning", (voidP)(new std::string("learning.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("test", (voidP)(new std::string("test.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classes", (voidP)(new std::string("classes.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classesNum", (voidP)(new uint(2)), ECF::UINT);
}

bool RegEvalOp::initialize(StateP state) {

    state->getContext()->environment = this;
    
    voidP sptr = state->getRegistry()->getEntry("classesNum");
    classesNum = *((uint *) sptr.get());

    sptr = state->getRegistry()->getEntry("learning");
    std::string filePath = *((std::string *) sptr.get());

    ifstream file;
    file.open(filePath.c_str());

    if (!file.is_open()) {
        ECF_LOG_ERROR(state, "Error: Can't open input file " + filePath);
        return false;
    }

    file >> nSamples >> variables;

    for (uint i = 0; i < nSamples; ++i) {
        std::vector<double> v;
        double var;
        for (uint j = 0; j < variables; ++j) {
            file >> var;
            v.push_back(var);
        }

        domain.push_back(v);
        std::string result;
        file >> result;
        codomain.push_back(result);
    }

    file.close();

    sptr = state->getRegistry()->getEntry("classes");
    filePath = *((std::string *) sptr.get());

    file.open(filePath.c_str());

    if (!file.is_open()) {
        generateDefaultClasses();
    } else {
        generateParsedClasses(file);
    }

    return true;
}

// fitness funkcija se brine za postavljanje čvorova koji su prije inicijalizirani
// fitnessInc treba biti implementiran za povećavanje fitnessa
FitnessP RegEvalOp::evaluate(IndividualP individual) {

    FitnessP fitness(new FitnessMin);

    Tree::Tree *tree = (Tree::Tree *) individual->getGenotype(0).get();

    double value = 0;
    predicted.clear();

    for (uint i = 0; i < nSamples; i++) {
        std::vector<double> v = domain.at(i);
        for (uint j = 0; j < v.size(); j++) {
            std::string terminal = "X" + std::to_string(j);
            tree->setTerminalValue(terminal, &v[j]);
        }

        double result;
        tree->execute(&result);
        value += fitnessInc(result, i);
    }
    fitness->setValue(value);
    std::cout << *this;

    return fitness;
}

// za sad vraca 1 u slucaju pogreske, 0 inace
double RegEvalOp::fitnessInc(double result, uint index) {

    for (auto& entry : classes) {
        for (auto& pair : entry.second) {
            if (result >= pair.first && result < pair.second) {
                predicted.push_back(entry.first);
                if (entry.first.compare(codomain.at(index)) == 0) {
                    return 0;
                } else {
                    return 1;
                }
            }
        }
    }

    return 1;
}

void RegEvalOp::generateDefaultClasses() {
    uint classID = 0;

    std::vector<std::pair<double, double>> valuesFirst;
    valuesFirst.push_back(std::make_pair(-pow(2, classID), pow(2, classID)));
    classes.insert(std::make_pair(std::to_string(classID), valuesFirst));
    classID++;

    for (uint i = 1; i < classesNum - 1; i++, classID++) {
        std::vector<std::pair<double, double>> values;
        values.push_back(std::make_pair(-pow(2, classID), -pow(2, classID - 1)));
        values.push_back(std::make_pair(pow(2, classID - 1), pow(2, classID)));
        classes.insert(std::make_pair(std::to_string(classID), values));
    }

    uint p = classesNum - 2;
    std::vector<std::pair<double, double>> values;
    values.push_back(std::make_pair(-INFINITY, -pow(2, p)));
    values.push_back(std::make_pair(pow(2,p), INFINITY));
    classes.insert(std::make_pair(std::to_string(classID), values));
}


void RegEvalOp::generateParsedClasses(std::ifstream& file) {
    std::string line;
    std::string className;

    for (uint i = 0; i < classesNum; i++) {
        std::getline(file, line);
        std::vector<std::string> elements = split(line, ' ');
        className = elements.at(0);
        
        std::vector<std::pair<double,double>> values;
        for (uint j = 1; j < elements.size(); j++) {
            std::string interval = elements.at(j);
            
            if (interval.at(0) == ':' || interval.back() == ':') {
                values.push_back(splitWithInf(interval));
            } else {
                std::vector<std::string> intervals = split(interval, ':');

                double beginning = std::stod(intervals.at(0));
                double end = std::stod(intervals.at(1));
                values.push_back(std::make_pair(beginning, end));
            }
        }

        classes.insert(std::make_pair(className, values));
    }

}

std::vector<std::string> RegEvalOp::split(std::string line, char delim) {
    std::vector<std::string> elements;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delim)) {
        elements.push_back(item);
    }

    return elements;
}

std::pair<double, double> RegEvalOp::splitWithInf(const std::string line) { 
    std::pair<double, double> interval;

    if (line.at(0) == ':') {
        std::string rightN = line.substr(1, line.size() - 1);
        interval = std::make_pair( -INFINITY, std::stod(rightN) );
    } else {
        std::string leftN = line.substr(0, line.size() - 1);
        interval = std::make_pair( std::stod(leftN), INFINITY );
    }

    return interval;
}

