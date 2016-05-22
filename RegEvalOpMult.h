#ifndef RegEvalOpMult_H
#define RegEvalOpMult_H

class RegEvalOpMult : public EvaluateOp {
    friend std::ostream &operator<<(std::ostream &os, const RegEvalOpMult &reg);

public:
    FitnessP evaluate(IndividualP individual);

    bool initialize(StateP state);
    void registerParameters(StateP state);
    void fitnessInc(std::vector<std::pair<std::string, double>> values, uint index);
    std::vector<std::string> split(const std::string, char);
    static std::pair<std::string, double> getAbs(std::pair<std::string, double> pair);
    void generateDefaultClasses();
    void generateParsedClasses(std::ifstream& file);
    std::vector<std::string> classes;
    std::vector<std::vector<double>> domain;
    std::vector<std::string> codomain;
    std::vector<std::string> predicted;

    uint nSamples;
    uint variables;
    uint classesNum;

    std::map<std::string, std::vector<uint>> f1Score;
};

#endif
