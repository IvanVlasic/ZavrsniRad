#include <ecf/ECF.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "RegEvalOp.h"

void RegEvalOp::registerParameters(StateP state) {

    state->getRegistry()->registerEntry("inputfile", (voidP)(new std::string("learning.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("testfile", (voidP)(new std::string("test.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classesfile", (voidP)(new std::string("classes.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("resultsfile", (voidP)(new std::string("results.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classesNum", (voidP)(new uint(2)), ECF::UINT);
}

bool RegEvalOp::initialize(StateP state) {

    state->getContext()->environment = this;
    
    voidP sptr = state->getRegistry()->getEntry("classesNum");
    classesNum = *((uint *) sptr.get());

    std::string outputPath = *((std::string*) state->getRegistry()->getEntry("resultsfile").get());
    
    std::ofstream outfile;
    outfile.open(outputPath.c_str());
   
    if (!outfile.is_open()) {
        ECF_LOG_ERROR(state, "Error: Can't open output file " + outputPath);
        return false;
    }
    
    outfile << "Gen_No,Training,Test" << std::endl;
    outfile.close();

    sptr = state->getRegistry()->getEntry("inputfile");
    std::string filePath = *((std::string *) sptr.get());

    ifstream file;
    file.open(filePath.c_str());

    if (!file.is_open()) {
        ECF_LOG_ERROR(state, "Error: Can't open input file " + filePath);
        return false;
    }

    parseFile(domain, codomain, file);
    file.close();



    sptr = state->getRegistry()->getEntry("testfile");
    filePath = *((std::string *) sptr.get());
    file.open(filePath.c_str());

    if (!file.is_open()) {
        ECF_LOG_ERROR(state, "Error: Can't open test file " + filePath);
        return false; 
    }

    parseFile(testDomain, testCodomain, file);
    file.close();
    
    sptr = state->getRegistry()->getEntry("classesfile");
    filePath = *((std::string *) sptr.get());

    file.open(filePath.c_str());

    if (!file.is_open()) {
        generateDefaultClasses();
    } else {
        generateParsedClasses(file);
    }

    file.close();

    for (auto& entry: classes) {
        f1Score.insert(std::make_pair(entry.first, std::vector<uint>(3, 0)));
    }
    
    return true;
}

// fitness funkcija se brine za postavljanje čvorova koji su prije inicijalizirani
// fitnessInc treba biti implementiran za povećavanje fitnessa
FitnessP RegEvalOp::evaluate(IndividualP individual) {
    
    FitnessP fitness(new FitnessMax);

    Tree::Tree *tree = (Tree::Tree *) individual->getGenotype(0).get();

    double value = fitnessEvaluation(domain, codomain, tree); 
    fitness->setValue(value);

    value = fitnessEvaluation(testDomain, testCodomain, tree);
    testValues[individual->index] = value;

    return fitness;
}

void RegEvalOp::fitnessInc(double result, uint index, std::vector<std::string> &codomain) {

    for (auto& entry : classes) {
        for (auto& pair : entry.second) {
            if (result >= pair.first && result < pair.second) {
                if (entry.first.compare(codomain.at(index)) == 0) {
                    f1Score[entry.first][0]++;
                } else {
                    f1Score[codomain.at(index)][1]++;
                    f1Score[entry.first][2]++;
                }
            }
        }
    }
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

void RegEvalOp::parseFile(std::vector<std::vector<double>> &domain, std::vector<std::string> &codomain, std::ifstream& file) {

    std::string line;
    std::vector<std::string> attributes;
    while (std::getline(file, line)) {
        
        if (line.empty()) {
            break;
        }
        
        attributes = split(line, ',');
        int length = attributes.size();

        std::vector<double> v;
        for (int i = 0; i < length - 1; ++i) {
            v.push_back(std::stod(attributes.at(i)));
        }
        
        domain.push_back(v);
        codomain.push_back(attributes.at(length - 1));
   }

}

double RegEvalOp::fitnessEvaluation(std::vector<std::vector<double>> &domain, std::vector<std::string> &codomain, Tree::Tree* tree) {
    
    double value = 0;
   
    for (auto& entry: f1Score) {
        std::fill(entry.second.begin(), entry.second.end(), 0);
    }
    for (uint i = 0; i < codomain.size(); i++) {
        std::vector<double> v = domain.at(i);
        
        for (uint j = 0; j < v.size(); j++) {
            std::string terminal = "X" + std::to_string(j);
            tree->setTerminalValue(terminal, &v[j]);
        }

        double result;
        tree->execute(&result);
        fitnessInc(result, i, codomain);
    }
    
    for (auto& entry: f1Score) {
        auto& terms = entry.second;
        if (terms[0] == 0 && terms[1] == 0 && terms[2] == 0) {
            continue;
        }
        
        value += (double)(2 * terms[0]) / (2 * terms[0] + terms[1] + terms[2]);
    }

    value /= classesNum;

    return value;

}
