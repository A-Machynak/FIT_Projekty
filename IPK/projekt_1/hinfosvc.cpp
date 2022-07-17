/**
 * @file hinfosvc.cpp
 * @author Augustin Machynak
 * @brief IPP Project 1
 * @date 2022-02-03
 * 
 */
#define bHttp_DEBUG
#include "inc/BasicHTTP.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <limits.h>

#ifndef HOST_NAME_MAX
    #define HOST_NAME_MAX 64
#endif

#define USLEEP_TIME 1000000

std::string getLoad(uint64_t usleep_time) {
    std::ifstream in("/proc/stat");
    enum { user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice };

    // "cpuX" | user | nice | system | idle | iowait | irq | softirq | steal | guest | guest_nice
    static const uint8_t LINE_WORDS = 11;
    std::string throwaway;

    in >> throwaway;

    uint64_t prevValues[LINE_WORDS-1];
    for(uint8_t i = 0; i < LINE_WORDS-1; i++) {
        in >> prevValues[i];
    }

    usleep(usleep_time);
    in.close();

    in = std::ifstream("/proc/stat");
    in >> throwaway;
    uint64_t newValues[LINE_WORDS-1];

    for(uint8_t i = 0; i < LINE_WORDS-1; i++) {
        in >> newValues[i];
    }

    // Source: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    unsigned long long PrevIdle = prevValues[idle] + prevValues[iowait];
    unsigned long long Idle = newValues[idle] + newValues[iowait];

    unsigned long long PrevNonIdle = prevValues[user] + prevValues[nice] + prevValues[system] + prevValues[irq] + prevValues[softirq] + prevValues[steal];
    unsigned long long NonIdle = newValues[user] + newValues[nice] + newValues[system] + newValues[irq] + newValues[softirq] + newValues[steal];

    unsigned long long PrevTotal = PrevIdle + PrevNonIdle;
    unsigned long long Total = Idle + NonIdle;

    unsigned long long totald = Total - PrevTotal;
    unsigned long long idled = Idle - PrevIdle;

    if(totald == 0) { // Division by zero
        return "0%";
    }

    float percentage = ((float)(totald - idled) / totald) * 100.0;

    char buf[10];
    sprintf(buf, "%.2f%%", percentage);

    in.close();
    return buf;
}

std::string getCpuName() {
    std::ifstream in("/proc/cpuinfo");
    if(!in) {
        std::cerr << "Couldn't open /proc/cpuinfo" << std::endl;
        return "CPU name not found";
    }
    std::string line, cpuName;

    while(std::getline(in, line)) {
        if(line.find("model name") != std::string::npos) {
            size_t commaPos = line.find(":");
            if(commaPos == std::string::npos) {
                continue;
            }
            in.close();
            return line.substr(commaPos + 2);
        }
    }

    std::cerr << "Couldn't find model name in /proc/cpuinfo" << std::endl;
    in.close();
    return "CPU name not found";
}

std::string getHostname() {
    char name[HOST_NAME_MAX];
    if(gethostname(name, HOST_NAME_MAX) == -1) {
        std::cerr << "Couldn't retrieve hostname" << std::endl;
        return "Hostname not found";
    }
    return name;
}


// "/*" - default - Not Found
std::string defaultPath(bHttp::context_t *ctx) {
    return bHttp::ResponseBuilder::begin()
        .statusCode(bHttp::Not_Found)
        .body("Page not found\r\n")
        .build();
}

// "/cpu-name"
std::string cpuNamePath(bHttp::context_t *ctx) {
    static std::string body = getCpuName();

    return bHttp::ResponseBuilder::begin()
        .statusCode(bHttp::OK)
        .setHeader("Content-Type", "text/plain;charset=utf-8")
        .body(body + "\r\n")
        .build();
}

// "/load"
std::string loadPath(bHttp::context_t *ctx) {
    std::string body = getLoad(USLEEP_TIME);

    return bHttp::ResponseBuilder::begin()
        .statusCode(bHttp::OK)
        .setHeader("Content-Type", "text/plain;charset=utf-8")
        .body(body + "\r\n")
        .build();
}

// "/hostname"
std::string hostnamePath(bHttp::context_t *ctx) {
    static std::string body = getHostname();

    return bHttp::ResponseBuilder::begin()
        .statusCode(bHttp::OK)
        .setHeader("Content-Type", "text/plain;charset=utf-8")
        .body(body + "\r\n")
        .build();
}

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cerr << "[ERROR]: Incorrect amount of arguments" << std::endl;
        std::cout << "hinfosvc\nusage: hinfosvc { port | -h }>" << std::endl;
        return 1;
    }
    
    if(!strcmp(argv[1], "-h")) {
        std::cout << "hinfosvc\nusage: hinfosvc { port | -h }>" << std::endl;
        return 0;
    }

    char *p;
    auto port = (uint16_t)std::strtol(argv[1], &p, 10);
    if(*p != '\0') {
        std::cerr << "[ERROR]: Expected port number" << std::endl;
        std::cout << "hinfosvc\nusage: hinfosvc { port | -h }>" << std::endl;
        return 1;
    }

    if(port <= 0 || port >= 65535) {
        std::cerr << "[ERROR]: Port number out of range" << std::endl;
        std::cout << "hinfosvc\nusage: hinfosvc { port | -h }>" << std::endl;
        return 1;
    }

    auto server = bHttp::BasicHTTPServer(argv[1]);
    if(server.isInitialized()) {
        server.addPathFn("", defaultPath);
        server.addPathFn("/hostname", hostnamePath);
        server.addPathFn("/cpu-name", cpuNamePath);
        server.addPathFn("/load", loadPath);

        if(!server.run()) {
            std::cout << "Server exited with error" << std::endl;
            return 2;
        }
    } else {
        std::cout << "Server couldn't be initialized" << std::endl;
        return 2;
    }
    return 0;
}
