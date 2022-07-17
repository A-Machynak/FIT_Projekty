/**
 * @file BasicHTTP.cpp
 * @author Augustin Machynak
 * @brief Basic http library
 * @date 2022-02-02
 * 
 */

#include "../inc/BasicHTTP.h"
#include <iostream>

namespace bHttp {
    BasicHTTPServer::BasicHTTPServer(std::string service, uint32_t backlog) {
        this->service = service;
        this->backlog = backlog;

        addrinfo_t hints {}, *res;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        int result = getaddrinfo(NULL, service.c_str(), &hints, &res);
        if(result != 0) {
            _LOG_ERR("getaddrinfo() failed. Error( " << result << "): " << gai_strerror(result));
            return;
        }

        _LOG("Initializing socket");
        socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(socket_fd == -1) {
            _LOG_ERR("Failed creating socket endpoint");
            return;
        }

        int tru = 1;
        if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &tru, sizeof(tru))) {
            _LOG_DEBUG("Failed setting SO_REUSEADDR for socket");
        }

        _LOG("Binding socket to " << this->service);
        if(bind(socket_fd, res->ai_addr, res->ai_addrlen) != 0) {
            shutdown(socket_fd, SHUT_RDWR);
            _LOG_ERR("Failed binding socket");
            return;
        }
        freeaddrinfo(res);

        initialized = true;
    }

    BasicHTTPServer::~BasicHTTPServer() {
        if(initialized) {
            close(socket_fd);
        }
    }

    bool BasicHTTPServer::run() {
        _LOG("Listen()ing on socket...");
        int result = listen(socket_fd, backlog);
        if(result != 0) {
            shutdown(socket_fd, SHUT_RDWR);
            if(errno == EADDRINUSE) {
                _LOG_ERR("Socket listen() call failed: Another socket is already listening on the same port.");
            } else {
                _LOG_ERR("Socket listen() call failed unexpectedly. Error code " << result);
            }
            return false;
        }

        running = true;
        sockaddr_in_t sock_addr_temp;
        unsigned int size = sizeof(sockaddr_in_t);
        int new_socket_fd, sent, expect_sent, received_message_size;

        std::string received_message;
        received_message.resize(maxHeaderSize);

        while((new_socket_fd = accept(socket_fd, (struct sockaddr*)&sock_addr_temp, &size))) {
            context_t ctx;

            // Receive data
            received_message_size = recv(new_socket_fd, (void*)received_message.data(), maxHeaderSize, 0);
            if(received_message_size == 0) {
                // Closed connection
                close(new_socket_fd);
                continue;
            } else if(received_message_size == -1) {
                // Error
                std::cout << received_message_size << std::endl;
                this->running = false;
                return false;
            }
            
            auto pair = parseHeaderToStruct(&ctx, received_message);

            std::string result;
            if(!pair.first) {
                // Bad header
                result = ResponseBuilder::begin()
                    .statusCode(Bad_Request)
                    .build();
            } else {
                // OK header
                auto fn = paths.find(ctx.header.URI);
                ctx.body = received_message.substr(pair.second);

                if(fn != paths.end()) {
                    result = fn->second(&ctx);
                } else {
                    // Call default path
                    result = paths[""](&ctx);
                }

                /*if((uint32_t)received_message_size > maxHeaderSize) {
                    // Parse and receive more if needed
                }*/
            }

            // Send response
            expect_sent = result.length();
            sent = send(new_socket_fd, result.data(), expect_sent, 0);
            
            if(sent != -1) {
                while(sent != expect_sent) {
                    // send() didn't send all the data, keep send()ing the next ones
                    std::string next_response = result.substr(sent);
                    expect_sent = next_response.length();
                    sent = send(new_socket_fd, result.data(), expect_sent, 0);
                }
            } // else couldn't send data (error); close the socket and just ignore it for now(?)

            close(new_socket_fd);
        }

        this->running = false;
        return true;
    }

    std::pair<bool, uint32_t> BasicHTTPServer::parseHeaderToStruct(context_t *ctx, std::string received_message) {
        // request-line

        // method
        auto t = readWord(received_message, 0);
        if(t.token_type != TOKEN_TYPE_WORD) {
            _LOG_DEBUG("Failed parsing request-line: !<METHOD> ...");
            return { false, 0 };
        }
        ctx->header.method = stringToMethod(t.word);
        // SP
        t = readWord(received_message, t.end_pos);
        if(t.token_type != TOKEN_TYPE_SPACE) {
            _LOG_DEBUG("Failed parsing request-line: <METHOD> !<SP> ...");
            return { false, 0 };
        }
        // request-target
        t = readWord(received_message, t.end_pos);
        if(t.token_type != TOKEN_TYPE_WORD) {
            _LOG_DEBUG("Failed parsing request-line: <METHOD> <SP> !<REQUEST-TARGET> ...");
            return { false, 0 };
        }
        ctx->header.URI = t.word;
        // SP
        t = readWord(received_message, t.end_pos);
        if(t.token_type != TOKEN_TYPE_SPACE) {
            _LOG_DEBUG("Failed parsing request-line: <METHOD> <SP> <REQUEST-TARGET> !<SP> ...");
            return { false, 0 };
        }
        // HTTP-version
        t = readWord(received_message, t.end_pos);
        if(t.token_type != TOKEN_TYPE_WORD) {
            _LOG_DEBUG("Failed parsing request-line: <METHOD> <SP> <REQUEST-TARGET> <SP> !<HTTP-VERSION> ...");
            return { false, 0 };
        }
        ctx->header.http_version = t.word;

        // CRLF
        t = readWord(received_message, t.end_pos);
        if(t.token_type != TOKEN_TYPE_CRLF) {
            _LOG_DEBUG("Failed parsing request-line: <METHOD> <SP> <REQUEST-TARGET> <SP> <HTTP-VERSION> !<CRLF> ...");
            return { false, 0 };
        }

        // Headers
        while(received_message.size() > t.end_pos) {
            // Read a word, colon and all the way to the end
            t = readWord(received_message, t.end_pos);
            if(t.token_type == TOKEN_TYPE_EOF || t.token_type == TOKEN_TYPE_CRLF) {
                break;
            }

            if(t.word[t.word.size()-1] != ':') {
                _LOG_DEBUG("Failed parsing header: <HEADER> !<:> ... (expected colon at \" " << t.word << "\"");
                return { false, 0 };
            }
            t.word.pop_back();

            bool end_line = false;
            std::string word;
            for(; t.end_pos < received_message.size(); t.end_pos++) {
                if(end_line && received_message[t.end_pos] == '\n') {
                    if(received_message[t.end_pos] == '\n') {
                        t.end_pos++;
                        break; // CRLF
                    } else {
                        _LOG_DEBUG("Failed parsing header: <HEADER> <:> <VALUE> !<CRLF> (expected CRLF at \" " << t.word << word << "\"");
                        return { false, 0 };
                    }
                } else if(received_message[t.end_pos] == '\r') {
                    end_line = true;
                } else {
                    word += received_message[t.end_pos];
                }
            }
            ctx->header.headers[t.word] = word;
        }
        return { true, t.end_pos };
    }

    token_t BasicHTTPServer::readWord(std::string array, uint32_t start_pos) {
        // (only for reading the request-line)
        token_t token;
        token_states_t state = TOKEN_STATE_START;
        
        token.end_pos = start_pos;

        while(array.size() > token.end_pos) {
            if(state == TOKEN_STATE_START) {
                // detect the single spaces
                if(array[token.end_pos] == ' ') {
                    token.token_type = TOKEN_TYPE_SPACE;
                    token.end_pos++;
                    break;
                }

                if(array[token.end_pos] == '\r') {
                    state = TOKEN_STATE_CR;
                    token.end_pos++;
                } else {
                    state = TOKEN_STATE_WORD;
                    token.token_type = TOKEN_TYPE_WORD;
                }

            } else if(state == TOKEN_STATE_CR) {
                if(array[token.end_pos] == '\n') {
                    // '\r\n'
                    token.token_type = TOKEN_TYPE_CRLF;
                } else {
                    token.token_type = TOKEN_TYPE_UNEXPECTED_CR;
                }
                token.end_pos++;
                break;

            } else /* TOKEN_STATE_WORD */ {
                token.word += array[token.end_pos++];
                if(array[token.end_pos] == ' ' 
                || array[token.end_pos] == '\r'
                || array[token.end_pos] == '\n') {
                    break;
                }
            }
        }

        return token;
    }

    void BasicHTTPServer::addPathFn(std::string path, std::string (*fnPathPtr)(context_t*)) {
        paths[path] = fnPathPtr;
    }

    void BasicHTTPServer::setMaxHeaderSize(uint32_t maxHeaderSize) {
        this->maxHeaderSize = maxHeaderSize;
    }

    uint32_t BasicHTTPServer::getMaxHeaderSize() {
        return this->maxHeaderSize;
    }

    bool BasicHTTPServer::isInitialized() {
        return this->initialized;
    }

    bool BasicHTTPServer::isRunning() {
        return this->running;
    }

    ResponseBuilder ResponseBuilder::begin() {
        ResponseBuilder builder;
        builder.response.http_version = "HTTP/1.1";
        builder.response.status_code = Not_Found;
        builder.response.headers["Content-Language"] = "en";
        builder.response.headers["Content-Type"] = "text/plain;charset=utf-8";
        
        return builder;
    }

    std::string ResponseBuilder::build() {
        std::string str;
        str.append(response.http_version);
        str += " " + std::to_string(response.status_code) + "\r\nDate: ";

        // Source: https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
        static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
                "Aug", "Sep", "Oct", "Nov", "Dec"};
        char buf[100];
        time_t now = time(0);
        struct tm *tm = gmtime(&now);
        snprintf(buf, sizeof(buf), "%s, %d %s %d %02d:%02d:%02d GMT\r\n",
                days[tm->tm_wday], tm->tm_mday, months[tm->tm_mon],
                tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
        str += buf;

        str += "Server: BasicHTTPServer\r\n";

        for(auto h : response.headers) {
            if(!h.first.compare("Content-Length") || !h.first.compare("Content-Type")) {
                // Insert content-length and type to the end, since we have to calculate it
                continue;
            }
            str.append(h.first + ": " + h.second + "\n");
        }

        str += "Content-Length: " + std::to_string(response.body.size()) + "\r\n";
        str += "Content-Type: " + response.headers["Content-Type"] + "\r\n\r\n";

        str += response.body;
        return str;
    }

    ResponseBuilder& ResponseBuilder::setHeader(std::string name, std::string value) {
        this->response.headers[name] = value;
        return *this;
    }

    ResponseBuilder& ResponseBuilder::body(std::string body) {
        this->response.body = body;
        return *this;
    }
    
    ResponseBuilder& ResponseBuilder::statusCode(response_status_code_t statusCode) {
        this->response.status_code = statusCode;
        return *this;
    }


    std::string BasicHTTPServer::methodToString(request_method_t method) {
        static std::unordered_map<request_method_t, std::string> map = {
            { GET, "GET" },
            { HEAD, "HEAD" },
            { POST, "POST" },
            { PUT, "PUT" },
            { DELETE, "DELETE" },
            { CONNECT, "CONNECT" },
            { OPTIONS, "OPTIONS" },
            { TRACE, "TRACE" }
        };
        auto item = map.find(method);
        if(item != map.end()) {
            return item->second;
        }

        return "";
    }

    request_method_t BasicHTTPServer::stringToMethod(std::string methodStr) {
        static std::unordered_map<std::string, request_method_t> map = {
            { "GET", GET },
            { "HEAD", HEAD },
            { "POST", POST },
            { "PUT", PUT },
            { "DELETE", DELETE },
            { "CONNECT", CONNECT },
            { "OPTIONS", OPTIONS },
            { "TRACE", TRACE }
        };
        auto item = map.find(methodStr);
        if(item != map.end()) {
            return item->second;
        }

        return UNKNOWN_REQUEST_METHOD;
    }

    response_status_code_t BasicHTTPServer::stringToStatus(std::string codeStr) {
        // uhh...
        static std::unordered_map<std::string, response_status_code_t> map = {
            { "Continue", Continue },
            { "Switching protocols", Switching_protocols },
            { "OK", OK },
            { "Created", Created },
            { "Accepted", Accepted },
            { "Non-Authoritative Information", Non_Authoritative_Information },
            { "No Content", No_Content },
            { "Reset Content", Reset_Content },
            { "Multiple Choices", Multiple_Choices },
            { "Moved Permanently", Moved_Permanently },
            { "Found", Found },
            { "See Other", See_Other },
            { "Not Modified", Not_Modified },
            { "Use Proxy", Use_Proxy },
            { "Temporary Redirect", Temporary_Redirect },
            { "Bad Request", Bad_Request },
            { "Payment Required", Payment_Required },
            { "Forbidden", Forbidden },
            { "Not Found", Not_Found },
            { "Method Not Allowed", Method_Not_Allowed },
            { "Not Acceptable", Not_Acceptable },
            { "Request Timeout", Request_Timeout },
            { "Conflict", Conflict },
            { "Gone", Gone },
            { "Length Required", Length_Required },
            { "Payload Too Large", Payload_Too_Large },
            { "URI Too Long", URI_Too_Long },
            { "Unsupported Media Type", Unsupported_Media_Type },
            { "Expectation Failed", Expectation_Failed },
            { "Upgrade Required", Upgrade_Required },
            { "Internal Server Error", Internal_Server_Error },
            { "Not Implemented", Not_Implemented },
            { "Bad Gateway", Bad_Gateway },
            { "Service Unavailable", Service_Unavailable },
            { "Gateway Timeout", Gateway_Timeout },
            { "HTTP Version Not Supported", HTTP_Version_Not_Supported }
        };
        auto pair = map.find(codeStr);
        if(pair != map.end()) {
            return pair->second;
        }

        return UNKNOWN_STATUS_CODE;
    }

    std::string BasicHTTPServer::statusToString(response_status_code_t code) {
        static std::unordered_map<response_status_code_t, std::string> map = {
            { Continue, "Continue" },
            { Switching_protocols, "Switching protocols" },
            { OK, "OK" },
            { Created, "Created" },
            { Accepted, "Accepted" },
            { Non_Authoritative_Information, "Non-Authoritative Information" },
            { No_Content, "No Content" },
            { Reset_Content, "Reset Content" },
            { Multiple_Choices, "Multiple Choices" },
            { Moved_Permanently, "Moved Permanently" },
            { Found, "Found" },
            { See_Other, "See Other" },
            { Not_Modified, "Not Modified" },
            { Use_Proxy, "Use Proxy" },
            { Temporary_Redirect, "Temporary Redirect" },
            { Bad_Request, "Bad Request" },
            { Payment_Required, "Payment Required" },
            { Forbidden, "Forbidden" },
            { Not_Found, "Not Found" },
            { Method_Not_Allowed, "Method Not Allowed" },
            { Not_Acceptable, "Not Acceptable" },
            { Request_Timeout, "Request Timeout" },
            { Conflict, "Conflict" },
            { Gone, "Gone" },
            { Length_Required, "Length Required" },
            { Payload_Too_Large, "Payload Too Large" },
            { URI_Too_Long, "URI Too Long" },
            { Unsupported_Media_Type, "Unsupported Media Type" },
            { Expectation_Failed, "Expectation Failed" },
            { Upgrade_Required, "Upgrade Required" },
            { Internal_Server_Error, "Internal Server Error" },
            { Not_Implemented, "Not Implemented" },
            { Bad_Gateway, "Bad Gateway" },
            { Service_Unavailable, "Service Unavailable" },
            { Gateway_Timeout, "Gateway Timeout" },
            { HTTP_Version_Not_Supported, "HTTP Version Not Supported" }
        };
        auto pair = map.find(code);
        if(pair != map.end()) {
            return pair->second;
        }

        return "";
    }
}
