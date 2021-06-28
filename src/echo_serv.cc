#include "echo_serv.h"
#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "spdlog/spdlog.h"
#include "spdlog/common.h"
#include "nlohmann/json.hpp"
// #include <nlohmann/adl_serializer.hpp>
#include "echo.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::SslServerCredentialsOptions;
using echo::Message;
using echo::ECHO;

using json = nlohmann::json;

namespace square_rpc_serv{
    void read ( const std::string& filename, std::string& data ){
        std::ifstream file ( filename.c_str (), std::ios::in );
        if ( file.is_open () )
        {
            std::stringstream ss;
            ss << file.rdbuf ();

            file.close ();

            data = ss.str ();
        }

        return;
    }
    // Logic and data behind the server's behavior.
    class ECHOServiceImpl final : public ECHO::Service {
        public:
        ECHOConf *square_rpc_conf;
        ECHOServiceImpl (ECHOConf *square_rpc_conf){
            this->square_rpc_conf=square_rpc_conf;
        }
        Status Square(ServerContext* context, const Message* request,
                        Message* reply) override {
            auto m =request->message();
            spdlog::info("Query is query");
            reply->set_message(pow(m,2));
            return Status::OK;
        }
        Status RangeSquare(ServerContext* context, const Message* request,
                            ServerWriter<Message>* writer) override {
            auto m =request->message();
            for (int n = 0; n < m; ++n) {
                auto f = pow(double(n),2);
                Message res;
                res.set_message(f);
                writer->Write(res);
            }
            return Status::OK;
        }

        Status SumSquare(ServerContext* context, ServerReader<Message>* reader, Message* summary) override {
            Message m;
            double r = 0.0;
            while (reader->Read(&m)) {
            spdlog::info("Query is query");
            auto n = (&m)->message();
            r += pow(double(n),2);
            }
            spdlog::info("Query is end");
            summary->set_message(r);
            return Status::OK;
        }

        Status StreamrangeSquare(ServerContext* context,
                        ServerReaderWriter<Message, Message>* stream) override {
            Message m;
            while (stream->Read(&m)) {
                spdlog::info("Query get");
                auto n = (&m)->message();
                Message r;
                r.set_message(pow(double(n),2));
                stream->Write(r);
                spdlog::info("Response send");
            }
            return Status::OK;
        }
    };

    //ECHOConf 构造函数,可以使用 nlohmann::json 来构造实例
    ECHOConf::ECHOConf(const json &j)
    {
        try
        {
            j.at("app_version").get_to(this->app_version);
        }
        catch (std::exception &e)
        {
            this->app_version = "0.0.0";
        }
        try
        {
            j.at("app_name").get_to(this->app_name);
        }
        catch (std::exception &e)
        {
            this->app_name = "faiss_grpc";
        }
        try
        {
            j.at("address").get_to(this->address);
        }
        catch (std::exception &e)
        {
            this->address = "address";
        }
        try
        {
            j.at("log_level").get_to(this->log_level);
        }
        catch (std::exception &e)
        {
            this->log_level = "debug";
        }
        try
        {
            j.at("server_cert_path").get_to(this->server_cert_path);
        }
        catch (std::exception &e)
        {
            this->server_cert_path = "";
        }
        try
        {
            j.at("server_key_path").get_to(this->server_key_path);
        }
        catch (std::exception &e)
        {
            this->server_key_path = "";
        }
        try
        {
            j.at("root_cert_path").get_to(this->root_cert_path);
        }
        catch (std::exception &e)
        {
            this->root_cert_path = "";
        }
        try
        {
            j.at("client_certificate_request").get_to(this->client_certificate_request);
        }
        catch (std::exception &e)
        {
            this->client_certificate_request = "";
        }
        try
        {
            j.at("max_rec_msg_size").get_to(this->max_rec_msg_size);
        }
        catch (std::exception &e)
        {
            this->max_rec_msg_size = 1024 * 1024;
        }
        try
        {
            j.at("max_send_msg_size").get_to(this->max_send_msg_size);
        }
        catch (std::exception &e)
        {
            this->max_send_msg_size = 1024 * 1024;
        }
        try
        {
            j.at("max_concurrent_streams").get_to(this->max_concurrent_streams);
        }
        catch (std::exception &e)
        {
            this->max_concurrent_streams = 100;
        }
        try
        {
            j.at("max_connection_idle").get_to(this->max_connection_idle);
        }
        catch (std::exception &e)
        {
            this->max_connection_idle = 0;
        }
        try
        {
            j.at("max_connection_age").get_to(this->max_connection_age);
        }
        catch (std::exception &e)
        {
            this->max_connection_age = 0;
        }
        try
        {
            j.at("max_connection_age_grace").get_to(this->max_connection_age_grace);
        }
        catch (std::exception &e)
        {
            this->max_connection_age_grace = 0;
        }
        try
        {
            j.at("keepalive_time").get_to(this->keepalive_time);
        }
        catch (std::exception &e)
        {
            this->keepalive_time = 0;
        }
        try
        {
            j.at("keepalive_timeout").get_to(this->keepalive_timeout);
        }
        catch (std::exception &e)
        {
            this->keepalive_timeout = 0;
        }
        try
        {
            j.at("keepalive_enforement_permit_without_stream").get_to(this->keepalive_enforement_permit_without_stream);
        }
        catch (std::exception &e)
        {
            this->keepalive_enforement_permit_without_stream = true;
        }
        try
        {
            j.at("compression").get_to(this->compression);
        }
        catch (std::exception &e)
        {
            this->compression = "";
        }
    }
    //to_json 序列化对象为json
    json ECHOConf::to_json() const
    {
        auto j = json{
            {"app_version", this->app_version},
            {"app_name", this->app_name},
            {"address", this->address},
            {"log_level", this->log_level},
            {"server_cert_path", this->server_cert_path},
            {"server_key_path", this->server_key_path},
            {"root_cert_path", this->root_cert_path},
            {"client_certificate_request",this->client_certificate_request},
            {"max_rec_msg_size", this->max_rec_msg_size},
            {"max_send_msg_size", this->max_send_msg_size},
            {"max_concurrent_streams", this->max_concurrent_streams},
            {"max_connection_idle", this->max_connection_idle},
            {"max_connection_age", this->max_connection_age},
            {"max_connection_age_grace", this->max_connection_age_grace},
            {"keepalive_time", this->keepalive_time},
            {"keepalive_timeout", this->keepalive_timeout},
            {"keepalive_enforement_permit_without_stream", this->keepalive_enforement_permit_without_stream},
            {"compression", this->compression}};
        return j;
    }
    //RunServer 根据配置启动服务
    void ECHOConf::RunServer()
    {
        // 初始化log
        std::string jsonlastlogpattern = "{\"time\": \"%Y-%m-%dT%H:%M:%S.%f%z\", \"app_name\": \"";
        jsonlastlogpattern += this->app_name;
        jsonlastlogpattern += "\",\"app_version\": \"";
        jsonlastlogpattern += this->app_version;
        jsonlastlogpattern += "\", \"level\": \"%^%l%$\", \"process\": %P, \"thread\": %t, \"message\": %v}";
        spdlog::set_pattern(jsonlastlogpattern);
        std::vector<std::string> level_range = {"trace", "debug", "info", "warn", "err", "critical"};
        if (std::find(level_range.begin(), level_range.end(), this->log_level) != level_range.end())
        {
            auto level = spdlog::level::from_str(this->log_level);
            spdlog::set_level(level);
        }
        else
        {
            json evt;
            evt["event"] = "unknown log level";
            evt["log_level"] = this->log_level;
            evt["after"] = "use debug as log level";
            spdlog::info(evt.dump());
            spdlog::set_level(spdlog::level::debug);
        }

        spdlog::info("{\"config\":" + this->to_json().dump() + "}");
        std::string server_address(this->address);
        ECHOServiceImpl service(this);
        
        //配置grpc
        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        ServerBuilder builder;
        // 压缩
        if (this->compression == std::string("gzip")){
            builder.SetDefaultCompressionAlgorithm(GRPC_COMPRESS_GZIP);
            spdlog::info((json{
                          {"event", "Compression setted"},
                          {"algo", this->compression}})
                         .dump());
        }else if (this->compression == std::string("deflate")){
            builder.SetDefaultCompressionAlgorithm(GRPC_COMPRESS_DEFLATE);
            spdlog::info((json{
                          {"event", "Compression setted"},
                          {"algo", this->compression}})
                         .dump());
        }else{
            spdlog::info((json{{"event", "Compression not set"}}).dump());
        }
        builder.SetMaxReceiveMessageSize(this->max_rec_msg_size);
        builder.SetMaxSendMessageSize(this->max_send_msg_size);
        if (this->max_concurrent_streams > 0){
            builder.AddChannelArgument(GRPC_ARG_MAX_CONCURRENT_STREAMS,this->max_concurrent_streams);
        }
        if (this->max_connection_idle > 0){
            builder.AddChannelArgument(GRPC_ARG_MAX_CONNECTION_IDLE_MS,this->max_connection_idle);
        }
        if (this->max_connection_age > 0){
            builder.AddChannelArgument(GRPC_ARG_MAX_CONNECTION_AGE_MS,this->max_connection_age);
        }
        if (this->max_connection_age_grace > 0){
            builder.AddChannelArgument(GRPC_ARG_MAX_CONNECTION_AGE_GRACE_MS,this->max_connection_age_grace);
        }
        if (this->keepalive_time > 0){
            builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS,this->keepalive_time);
        }
        if (this->keepalive_timeout > 0){
            builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS,this->keepalive_timeout);
        }
        if (this->keepalive_enforement_permit_without_stream == true){
             builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS,1);
        }else{
            builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS,0);
        }

        // 设置tls
        if (this->server_cert_path != std::string("") && this->server_key_path != std::string("")){
            SslServerCredentialsOptions sslOps;
            std::string key;
	        std::string cert;
            read( this->server_cert_path, cert );
	        read( this->server_key_path, key );
            SslServerCredentialsOptions::PemKeyCertPair keycert ={key,cert};
            sslOps.pem_key_cert_pairs.push_back ( keycert );
            if (this->root_cert_path != std::string("")){
                std::string root;
                read( this->root_cert_path, root );
                sslOps.pem_root_certs = root;
                if (this->client_certificate_request == std::string("not_request")){
                    sslOps.force_client_auth=true;
                    sslOps.client_certificate_request=GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE;
                } else if (this->client_certificate_request == std::string("not_verify"))
                {
                    sslOps.force_client_auth=true;
                    sslOps.client_certificate_request=GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;
                } else if (this->client_certificate_request == std::string("request_and_verify"))
                {
                     sslOps.force_client_auth=true;
                    sslOps.client_certificate_request=GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_AND_VERIFY;
                } else if (this->client_certificate_request == std::string("enforce_request"))
                {
                     sslOps.force_client_auth=true;
                    sslOps.client_certificate_request=GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;
                } else if (this->client_certificate_request == std::string("enforce_request_and_verify"))
                {
                     sslOps.force_client_auth=true;
                    sslOps.client_certificate_request=GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY ;
                }  else
                {
                    sslOps.force_client_auth=false;
                    sslOps.client_certificate_request=GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE;
                }
                
            }
            builder.AddListeningPort(server_address, grpc::SslServerCredentials( sslOps ));
        }else{
            // Listen on the given address without any authentication mechanism.
            builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        }
        
        // clients. In this case it corresponds to an *synchronous* service.
        builder.RegisterService(&service);
        // Finally assemble the server.
        std::unique_ptr<Server> server(builder.BuildAndStart());
        spdlog::info((json{
                          {"event", "Server start"},
                          {"address", this->address}})
                         .dump());
        // Wait for the server to shutdown. Note that some other thread must be
        // responsible for shutting down the server for this call to ever return.
        server->Wait();
    }

    void to_json(json &j, const square_rpc_serv::ECHOConf &p)
    {
        j = p.to_json();
    }

    void from_json(const json &j, square_rpc_serv::ECHOConf &p)
    {
        p = square_rpc_serv::ECHOConf(j);
    }
}
