#include "../inc/SFTP_Client.h"
#include "../inc/Utils.h"
#include <iostream>

void printHelp() {
    _LOG_INFO("Usage: ./ipk-simpleftp-server [-h IP] {-p port} [-f working_directory]");
}

int main(int argc, char **argv) {
    // Command line arguments
    std::string ip = "";                // -h
    std::string port = "115";           // -p
    std::string workingDirectory = "";  // -f

     // Parse command line arguments
    auto vec = utils::parseArgFlags(argc, argv);
    for(auto &p : vec) {
        if(p.first == "-h") {
            ip = p.second;
        } else if(p.first == "-p") {
            port = p.second;
        } else if(p.first == "-f") {
            workingDirectory = p.second;
        } else if(p.first == "--help") {
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
        return 1;
    }

    if(ip == "") {
        _LOG_ERR("Missing IP(v4/v6) (-h)");
        return 1;
    }

    // Start the client
    auto cl = sftp::Client(ip, workingDirectory, port);
    cl.run();

    return 0;
}
