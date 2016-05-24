#ifndef RegEvalOp_H
#define RegEvalOp_H


class RegEvalOp : public EvaluateOp {


public:
    FitnessP evaluate(IndividualP individual);
    std::map<uint, double> testValues;
    bool initialize(StateP state);
    std::vector<std::string> split(const std::string, char);
    std::pair<double, double> splitWithInf(const std::string);
    void registerParameters(StateP state);
    // defines fitness increment based on tree execution result
    void fitnessInc(double result, uint index, std::vector<std::string>&);
    // vectors of parameters
    // default classes
    void generateDefaultClasses();
    // classes parsed from parameters' file
    void generateParsedClasses(std::ifstream& file);
    // domain

    void parseFile(std::vector<std::vector<double>>&, std::vector<std::string>&, std::ifstream&);
    double fitnessEvaluation(std::vector<std::vector<double>>&, std::vector<std::string>&, Tree::Tree*);
    std::vector <std::vector<double>> domain;
    std::vector<std::vector<double>> testDomain;
    // results of samples
    std::vector <std::string> codomain;
    std::vector<std::string> testCodomain;
    // number of samples
    uint nSamples;
    // number of variables for each sample
    uint variables;
    // number od different classes
    uint classesNum;
    // classes represented as map with class identifier as key, and intervals as value 
    std::map<std::string, std::vector<std::pair<double, double>>> classes;
    // TP - FN - FP
    std::map<std::string, std::vector<uint>> f1Score;
};

#endif
