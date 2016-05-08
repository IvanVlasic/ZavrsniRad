class IfPositive : public Tree::Primitives::Primitive
{
public:
    IfPositive() 
    {
        nArguments_ = 3;
        name_ = "ifpos";
    }

    void execute(void* evalOp, Tree::Tree& tree)
    {
        double first, second, third;
        double& result = *(double*)evalOp;
        
        getNextArgument(&first, tree);
        getNextArgument(&second, tree);
        getNextArgument(&third, tree);
    
        result = first > 0 ? second : third;
    }

    ~IfPositive()
    {  }
};
