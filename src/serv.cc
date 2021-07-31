#include <spdlog/spdlog.h>
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "echo.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using echo::ECHO_RPC;
using echo::Message;

// Logic and data behind the server's behavior.
class ECHO_RPCServiceImpl final : public ECHO_RPC::Service {
    Status echo(ServerContext* context, const Message* request, Message* response) override {
        spdlog::info("get message {}",request->message());
        std::string prefix("Hello ");
        response->set_message(prefix + request->message());
        return Status::OK;
    }
};


void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ECHO_RPCServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    spdlog::info("Server listening on {}",server_address);
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}


int main(int argc, char** argv) {
    RunServer();
    return 0;
}