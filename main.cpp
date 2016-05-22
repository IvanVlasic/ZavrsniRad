#include <ecf/ECF.h>
#include "RegEvalOp.h"
#include "RegEvalOpMult.h"
#include <vector>
#include <iostream>
#include "IfPositive.h"
#include "IfLessThanEq.h"
#include "Sqrt.h"
#include "Log.h"
#include "TrainingTestValuesOperator.h"

int main(int argc, char **argv) {
    StateP state(new State);
    state->setEvalOp(new RegEvalOp);
    
    TrainingTestValuesOperatorP op = (TrainingTestValuesOperatorP) new TrainingTestValuesOperator;
    state->addOperator(op);

    TreeP tree(new Tree::Tree);

    Tree::PrimitiveP ifpos (new IfPositive);
    tree->addFunction(ifpos);
    Tree::PrimitiveP max (new Tree::Primitives::Max);
    tree->addFunction(max);
    Tree::PrimitiveP min (new Tree::Primitives::Min);
    tree->addFunction(min);
    Tree::PrimitiveP iflte (new IfLessThanEq);
    tree->addFunction(iflte);
    Tree::PrimitiveP sqrt (new Sqrt);
    tree->addFunction(sqrt);
    Tree::PrimitiveP log (new Log);
    tree->addFunction(log);

    state->addGenotype(tree);

    if (!state->initialize(argc, argv)) {
        return 1;
    }
    state->run();

}

