#ifndef RegEvalOpMult_H
#define RegEvalOpMult_H

class RegEvalOpMult : public EvaluateOp {

public:
    FitnessP evaluate(IndividualP individual);

    bool initialize(StateP state);
    void registerParameters(StateP state);
    void fitnessInc(std::vector<std::pair<std::string, double>> values, uint index, std::vector<std::string>&);
    std::vector<std::string> split(const std::string, char);
    static std::pair<std::string, double> getAbs(std::pair<std::string, double> pair);
    void generateDefaultClasses();
    void generateParsedClasses(std::ifstream& file);
   
    void parseFile(std::vector<std::vector<double>>&, std::vector<std::string>&, std::ifstream&);
    double fitnessEvaluation(std::vector<std::vector<double>>&, std::vector<std::string>&, IndividualP);
    std::vector<std::string> classes;
    std::vector<std::vector<double>> domain;
    std::vector<std::string> codomain;
    std::vector<std::vector<double>> testDomain;
    std::vector<std::string> testCodomain;
    std::map<uint, double> testValues;

    uint nSamples;
    uint variables;
    uint classesNum;

    std::map<std::string, std::vector<uint>> f1Score;
};

#endif
