#include "../inc/SFTP_Server.h"
#include "../inc/Utils.h"
#include <iostream>

void printHelp() {
    _LOG_INFO("Usage: ./ipk-simpleftp-server {-i interface} {-p port} [-u user_password_db_file] [-f working_directory]");
}

int main(int argc, char **argv) {
    // Command line arguments
    std::string interface = "";         // -i
    std::string credentialsFile = "";   // -u
    std::string workingDirectory = "";  // -f
    std::string port = "115";           // -p

    // Parse command line arguments
    auto vec = utils::parseArgFlags(argc, argv);
    for(auto &p : vec) {
        if(p.first == "-i") {
            interface = p.second;
        } else if(p.first == "-u") {
            credentialsFile = p.second;
        } else if(p.first == "-f") {
            workingDirectory = p.second;
        } else if(p.first == "-p") {
            auto pair = utils::toNumber(p.second);
            if(pair.first != 0 || pair.second <= 0 || pair.second >= 65535) {
                _LOG_ERR("Expected number between 0 and 65535");
                return 1;
            }
            port = p.second;
        } else if(p.first == "-h" || p.first == "--help") {
            printHelp();
            return 0;
        } else {
            _LOG_ERR("Unknown flag (" << p.first << ")");
            printHelp();
            return 1;
        }
    }

    if(workingDirectory == "") {
        _LOG_ERR("Missing working directory (-f)");
        printHelp();
        return 1;
    }

    if(credentialsFile == "") {
        _LOG_ERR("Missing credentials file (-u)");
        printHelp();
        return 1;
    }

    // Start the server
    auto sv = sftp::Server(workingDirectory, credentialsFile, interface, port);
    sv.run();

    return 0;
}
