#include <ecf/ECF.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "RegEvalOpMult.h"


void RegEvalOpMult::registerParameters(StateP state) {

    state->getRegistry()->registerEntry("inputfile", (voidP)(new std::string("learning.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("testfile", (voidP)(new std::string("test.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classesfile", (voidP)(new std::string("classes.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("resultsfile", (voidP)(new std::string("results.txt")), ECF::STRING);

    state->getRegistry()->registerEntry("classesNum", (voidP)(new uint(2)), ECF::UINT);
}

bool RegEvalOpMult::initialize(StateP state) {

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
    
    for (auto entry: classes) {
        f1Score.insert(std::make_pair(entry, std::vector<uint>(3, 0)));
    }

    return true;
}


FitnessP RegEvalOpMult::evaluate(IndividualP individual) {

    FitnessP fitness(new FitnessMax);
    
    double value = fitnessEvaluation(domain, codomain, individual);
    fitness->setValue(value);

    value = fitnessEvaluation(testDomain, testCodomain, individual);
    testValues[individual->index] = value;

    return fitness;
}

void RegEvalOpMult::fitnessInc(std::vector<std::pair<std::string, double>> values, uint index, std::vector<std::string> &codomain) {

    std::transform(values.begin(), values.end(), values.begin(), &getAbs); 
    std::pair<std::string, double> selected = values.at(0);
    
    for (auto &pair : values) {
        if (pair.second < selected.second) {
            selected = pair;
        }
    }
    
    if (selected.first.compare(codomain.at(index)) == 0) {
        f1Score[selected.first][0]++;
    } else {
        f1Score[codomain.at(index)][1]++;
        f1Score[selected.first][2]++;
    }

}

void RegEvalOpMult::generateDefaultClasses() {
    
    for (uint i = 0; i < classesNum; i++) {
        classes.push_back(std::to_string(i));
    }

}

void RegEvalOpMult::generateParsedClasses(std::ifstream& file) {
    std::string line;

    for (uint i = 0; i < classesNum; i++) {
        std::getline(file, line);
        classes.push_back(line);
    }
}

std::pair<std::string, double> RegEvalOpMult::getAbs(std::pair<std::string, double> pair) {

    return std::make_pair(pair.first, std::abs(pair.second));

}

std::vector<std::string> RegEvalOpMult::split(std::string line, char delim) {
    std::vector<std::string> elements;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delim)) {
        elements.push_back(item);
    }

    return elements;
}

void RegEvalOpMult::parseFile(std::vector<std::vector<double>> &domain, std::vector<std::string> &codomain, std::ifstream& file) {
    
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

double RegEvalOpMult::fitnessEvaluation(std::vector<std::vector<double>> &domain, std::vector<std::string> &codomain, IndividualP individual) {

    double value = 0;

    std::vector<std::pair<std::string, double>> values;

    for (auto& entry: f1Score) {
        std::fill(entry.second.begin(), entry.second.end(), 0);
    }

    for (uint i = 0; i < codomain.size(); i++) {
        std::vector<double> v = domain.at(i);
        values.clear();

        for (uint j = 0; j < classesNum; j++) {
            Tree::Tree *tree = (Tree::Tree *) individual->getGenotype(j).get();
            
            for (uint k = 0; k < v.size(); k++) {
                std::string terminal = "X" + std::to_string(k);
                tree->setTerminalValue(terminal, &v[k]);
            }

            double result;
            tree->execute(&result);
            values.push_back(std::make_pair(classes.at(j), result));
        }

        fitnessInc(values, i, codomain);
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

