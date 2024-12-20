module ServiceA;
import IService;

#include <iostream>

class ServiceA : public IService {
public:
    void execute() const override {
        std::cout << "ServiceA executed." << std::endl;
    }
};

// Define a custom deleter function in the DLL
extern "C" void service_deleter(IService* service) {
    std::cout << "Deleting ServiceA in DLL context." << std::endl;
    delete service;
}

extern "C" IService* create_service() {
    return new ServiceA();
}