export module IService;

export class IService {
public:
    virtual ~IService() = default;
    virtual void execute() const = 0;
};