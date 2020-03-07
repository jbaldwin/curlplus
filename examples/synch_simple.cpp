#include <lift/Lift.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Initialize must be called first before using the LiftHttp library.
    lift::GlobalScopeInitializer lift_init{};

    lift::RequestPool request_pool;
    {
        auto request = request_pool.Produce("http://www.example.com");
        std::cout << "Requesting http://www.example.com" << std::endl;
        const auto& response = request->Perform();
        std::cout << response.GetResponseData() << std::endl;
        // when the request destructs it will return to the pool automatically
    }

    {
        // this request object will be the same one as above recycled through the pool
        auto request = request_pool.Produce("http://www.google.com");
        std::cout << "Requesting http://www.google.com" << std::endl;
        const auto& response = request->Perform();
        std::cout << response.GetResponseData() << std::endl;

        for (const auto& header : response.GetResponseHeaders()) {
            std::cout << header.GetName() << ": " << header.GetValue() << "\n";
        }
    }

    return 0;
}
