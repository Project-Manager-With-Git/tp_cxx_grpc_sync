#ifndef ECHO_SERV__INCLUDED
#define ECHO_SERV__INCLUDED
#include <string>
#include "nlohmann/json.hpp"

namespace echo_serv {

    class ECHOConf {
        public:
        std::string app_version = "0.0.0";
        std::string app_name = "echo";
        std::string address = "0.0.0.0:5000";
        std::string log_level = "debug";
        //tls
        std::string server_cert_path = "";
        std::string server_key_path = "";
        std::string root_cert_path = "";
        //"not_request","not_verify","request_and_verify","enforce_request","enforce_request_and_verify"
        std::string client_certificate_request = "";
        //msg size
        int max_rec_msg_size = 1024 * 1024;
        int max_send_msg_size = 1024 * 1024;
        //http2
        int max_concurrent_streams = 100;
        int max_connection_idle = 0;
        int max_connection_age = 0;
        int max_connection_age_grace = 0;
        //keepalive
        int keepalive_time = 0;
        int keepalive_timeout = 0;
        bool keepalive_enforement_permit_without_stream = true;
        //compression
        std::string compression = "";

        ECHOConf() = default;
        ECHOConf(const nlohmann::json& j);
        nlohmann::json to_json() const;
        void RunServer();
        };
    void to_json(nlohmann::json& j, const ECHOConf& p);

    void from_json(const nlohmann::json& j, ECHOConf& p);
    // void RunServer(faiss_grpc_serv::FaissGrpcConf = faiss_grpc_serv::FaissGrpcConf());
    }

#endif
