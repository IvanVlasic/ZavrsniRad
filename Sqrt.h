class Sqrt : public Tree::Primitives::Primitive 
{
public:
    Sqrt() 
    {
        nArguments_ = 1;
        name_ = "sqrt";
    }

    void execute(void *evalOp, Tree::Tree& tree)
    {
        double first;
        double& result = *(double*)evalOp;
        getNextArgument(&first, tree);
        result = sqrt(fabs(first));
    }

    ~Sqrt() 
    {  }
};
