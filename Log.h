class Log : public Tree::Primitives::Primitive
{
public:
    Log()
    {
        nArguments_ = 1;
        name_ = "log";
    }

    void execute(void *evalOp, Tree::Tree& tree)
    {
        double first;
        double& result = *(double*)evalOp;
        getNextArgument(&first, tree);
        result = first > 0 ? log(first) : log(1);
    }

    ~Log()
    {  }
};
