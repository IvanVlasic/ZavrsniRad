#ifndef RegEvalOp_H
#define RegEvalOp_H


class RegEvalOp : public EvaluateOp {

    friend std::ostream &operator<<(std::ostream &os, const RegEvalOp &reg);

public:
    FitnessP evaluate(IndividualP individual);

    bool initialize(StateP state);
    std::vector<std::string> split(const std::string, char);
    std::pair<double, double> splitWithInf(const std::string);
    void registerParameters(StateP state);
    // defines fitness increment based on tree execution result
    double fitnessInc(double result, uint index);
    // vectors of parameters
    // default classes
    void generateDefaultClasses();
    // classes parsed from parameters' file
    void generateParsedClasses(std::ifstream& file);
    // domain
    std::vector <std::vector<double>> domain;
    // vector to hold predicted data in each iteration
    // used for printing out result of current iteration
    std::vector <std::string> predicted;
    // results of samples
    std::vector <std::string> codomain;
    // number of samples
    uint nSamples;
    // number of variables for each sample
    uint variables;
    // number od different classes
    uint classesNum;
    // classes represented as map with class identifier as key, and intervals as value 
    std::map<std::string, std::vector<std::pair<double, double>>> classes;

};

#endif
