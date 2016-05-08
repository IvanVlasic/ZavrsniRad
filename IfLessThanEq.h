class IfLessThanEq : public Tree::Primitives::Primitive
{
public:
    IfLessThanEq()
    {
        nArguments_ = 4;
        name_ = "iflte";
    }

    void execute(void* evalOp, Tree::Tree& tree)
    {
        double first, second, third, fourth;
        double &result = *(double*)evalOp;
    
        getNextArgument(&first, tree);
        getNextArgument(&second, tree);
        getNextArgument(&third, tree);
        getNextArgument(&fourth, tree);

        result = first > second ? third : fourth;
    }

    ~IfLessThanEq()
    {  }
};

