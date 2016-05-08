#include <ecf/ECF.h>
#include "RegEvalOp.h"
#include <vector>
#include <iostream>
#include "IfPositive.h"
#include "IfLessThanEq.h"
#include "Sqrt.h"
#include "Log.h"

int main(int argc, char **argv) {
    StateP state(new State);
    state->setEvalOp(new RegEvalOp);

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

    std::vector <IndividualP> hof = state->getHoF()->getBest();
    IndividualP ind = hof[0];

    std::cout << "\nPrediction data based on optimal regression tree:" << std::endl;

    RegEvalOp *evalOp = new RegEvalOp;

    state->getRegistry()->modifyEntry("learning", state->getRegistry()->getEntry("test"));

    evalOp->initialize(state);
    evalOp->evaluate(ind);
}

