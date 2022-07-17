/**
 * @file BasicHTTP.h
 * @author Augustin Machynak
 * @brief Basic http library
 * @date 2022-02-02
 * 
 */
#ifndef __BASIC_HTTP__
#define __BASIC_HTTP__

#include <cstdint>
#include <string>
#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <time.h>

namespace bHttp {
    #define DEFAULT_MAX_HEADER_SIZE 65535
    #define _LOG(msg) { std::cout << "[bHttp]: " << msg << std::endl; }
    #define _LOG_ERR(msg) { std::cerr << "[bHttp]: " << msg << std::endl; }

    #ifdef bHttp_DEBUG
        #define _LOG_DEBUG(msg) { std::cout << "[bHttp DEBUG]: " << msg << std::endl; }
    #else
        #define _LOG_DEBUG(msg) { }
    #endif

    typedef struct sockaddr_in sockaddr_in_t;
    typedef struct addrinfo addrinfo_t;

    typedef enum request_method_s {
        UNKNOWN_REQUEST_METHOD, // Unknown/Unsupported/"extension-method" request method
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE
    } request_method_t;

    typedef enum response_status_code_s {
        UNKNOWN_STATUS_CODE,
        Continue = 100,
        Switching_protocols = 101,

        OK = 200,
        Created = 201,
        Accepted = 202,
        Non_Authoritative_Information = 203,
        No_Content = 204,
        Reset_Content = 205,

        Multiple_Choices = 300,
        Moved_Permanently = 301,
        Found = 302,
        See_Other = 303,
        Not_Modified = 304,
        Use_Proxy = 305,
        Temporary_Redirect = 307,

        Bad_Request = 400,
        Payment_Required = 402,
        Forbidden = 403,
        Not_Found = 404,
        Method_Not_Allowed = 405,
        Not_Acceptable = 406,
        Request_Timeout = 408,
        Conflict = 409,
        Gone = 410,
        Length_Required = 411,
        Payload_Too_Large = 413,
        URI_Too_Long = 414,
        Unsupported_Media_Type = 415,
        Expectation_Failed = 417,
        Upgrade_Required = 426,

        Internal_Server_Error = 500,
        Not_Implemented = 501,
        Bad_Gateway = 502,
        Service_Unavailable = 503,
        Gateway_Timeout = 504,
        HTTP_Version_Not_Supported = 505,
    } response_status_code_t;

    typedef struct request_header_s {
        request_method_t method;
        std::string URI;
        std::string http_version;
        std::unordered_map<std::string, std::string> headers;
    } request_header_t;

    typedef struct context_s {
        bool connection_closed;
        request_header_t header;
        std::string body;
    } context_t;

    typedef struct response_s {
        std::string http_version;
        response_status_code_t status_code;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    } response_t;

    typedef enum token_type_e {
        TOKEN_TYPE_CRLF,
        TOKEN_TYPE_WORD,
        TOKEN_TYPE_SPACE,
        TOKEN_TYPE_EOF,
        TOKEN_TYPE_UNEXPECTED_CR
    } token_type_t;

    typedef enum token_states_e {
        TOKEN_STATE_START,
        TOKEN_STATE_WORD,
        TOKEN_STATE_CR,
        TOKEN_STATE_END
    } token_states_t;

    typedef struct token_s {
        token_type_t token_type;
        std::string word;
        uint32_t end_pos;
    } token_t;

    class BasicHTTPServer {
    public:
        /**
         * @brief Constructor
         * 
         * @param service Name of the service ("http", "ftp", ...) or a port number ("8080", "80", ...)
         * @param backlog Number of connections allowed on the incoming queue (listen() call)
         */
        BasicHTTPServer(std::string service = "8080", uint32_t backlog = 5);
        ~BasicHTTPServer();

        bool isInitialized();
        bool isRunning();
        bool run();

        /**
         * @brief Which function will be called on which uri path
         * 
         * @param path path ("" for default)
         * @param fnPathPtr function to be called
         */
        void addPathFn(std::string path, std::string (*fnPathPtr)(context_t*));
        
        /**
         * @brief Maximum received header size (- TODO)
         * 
         * @param maxHeaderSize Maximum received header size
         */
        void setMaxHeaderSize(uint32_t maxHeaderSize);
        uint32_t getMaxHeaderSize();

        static request_method_t stringToMethod(std::string methodStr);
        static std::string methodToString(request_method_t method);
        
        static response_status_code_t stringToStatus(std::string codeStr);
        static std::string statusToString(response_status_code_t code);
    private:
        std::string service;
        uint32_t backlog;
        int socket_fd;

        bool initialized = false;
        bool running = false;
        uint32_t maxHeaderSize = DEFAULT_MAX_HEADER_SIZE;

        typedef std::string (*fnPathPtr)(context_t*);
        std::unordered_map<std::string, fnPathPtr> paths;

        std::pair<bool, uint32_t> parseHeaderToStruct(context_t *ctx, std::string received_message);
        token_t readWord(std::string, uint32_t start_pos);
    };

    class ResponseBuilder {
    public:
        static ResponseBuilder begin();

        ResponseBuilder& statusCode(response_status_code_t statusCode);
        ResponseBuilder& body(std::string body);
        ResponseBuilder& setHeader(std::string name, std::string value);

        std::string build();
    private:
        ResponseBuilder *responseBuilder {};
        response_t response;
    };

}

#endif