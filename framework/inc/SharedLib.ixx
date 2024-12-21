export module SharedLib;



export class SharedLib {
public:
    virtual ~SharedLib() = default;

    // Get a function pointer by name
    template <typename Func>
    virtual Func getFunction(const std::string& funcName) = 0;
};