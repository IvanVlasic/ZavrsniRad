#ifndef TrainingTestValues_h
#define TrainingTestValues_h
 
#include "ecf/Operator.h"
#include <fstream>
#include <iostream> 
class TrainingTestValuesOperator: public Operator {
    std::string outputPath = "";
 
    void registerParameters(StateP state) {
        state->getRegistry()->registerEntry("resultsfile",
                (voidP) (new std::string), ECF::STRING);
    }
 
    bool initialize(StateP state) {
        if (state->getRegistry()->isModified("resultsfile")) {
            outputPath = *((std::string*) state->getRegistry()->getEntry(
                    "resultsfile").get());
        }
 
        return true;
    }
 
    bool operate(StateP state) {
        RegEvalOp* evalOp = (RegEvalOp*) state->getEvalOp().get();
 
        DemeP deme = state->getPopulation()->getLocalDeme();
 
        IndividualP best = deme->at(0);
 
        for (uint i=1; i<deme->size(); i++) {
            if(deme->at(i)->fitness->isBetterThan(best->fitness)) {
                best = deme->at(i);
            }
        }
 
        int genNo = state->getGenerationNo();
        double train = best->fitness->getValue();
        double test = evalOp->testValues[best->index];
        
        if (!outputPath.empty()) {
            std::ofstream outfile;
            outfile.open(outputPath.c_str(), std::ios_base::app);
            outfile << genNo << "," << train << "," << test << std::endl;
        }
 
        return true;
 
    }
};
typedef boost::shared_ptr<TrainingTestValuesOperator> TrainingTestValuesOperatorP;
 
#endif
