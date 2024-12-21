export module ServiceA;

import IService;

// Implement the factory function
extern "C" void service_deleter(IService* service);
extern "C" IService* create_service();