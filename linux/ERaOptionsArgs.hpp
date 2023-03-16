#ifndef INC_ERA_OPTIONS_ARGS_HPP_
#define INC_ERA_OPTIONS_ARGS_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ERa/ERaConfig.hpp>

static void processArgs(int argc, char* argv[],
                        const char*& auth,
                        const char*& boardID,
                        const char*& host,
                        uint16_t& port,
                        const char*& user,
                        const char*& pass)
{
    static struct option long_options[] = {
        {"token",   required_argument,   NULL, 't'},
        {"id",      required_argument,   NULL, 'i'},
        {"host",    required_argument,   NULL, 'h'},
        {"port",    required_argument,   NULL, 'p'},
        {"user",    required_argument,   NULL, 'u'},
        {"pass",    required_argument,   NULL, 'w'},
        {NULL, 0, NULL, 0}
    };

    // Set Default values
    auth = NULL;
    boardID = NULL;
    host = ERA_MQTT_HOST;
    port = ERA_MQTT_PORT;
    user = NULL;
    pass = NULL;

    const char* usage =
        "Usage: era [options]\n"
        "\n"
        "Options:\n"
        "  -t auth,     --token=auth    Your auth token\n"
        "  -i boardID,  --id=boardID    Your board ID\n"
        "  -h addr,     --host=addr     Server name (default: " ERA_MQTT_HOST ")\n"
        "  -p num,      --port=num      Server port (default: " ERA_TOSTRING(ERA_MQTT_PORT) ")\n"
        "  -u user,     --user=username Server username\n"
        "  -w pass,     --pass=password Server password\n"
        "\n";

    int rez;
    while (-1 != (rez = getopt_long(argc, argv,"t:i:h:p:u:w:", long_options, NULL))) {
        switch (rez) {
            case 't':
                auth = optarg;
                break;
            case 'i':
                boardID = optarg;
                break;
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                user = optarg;
                break;
            case 'w':
                pass = optarg;
                break;
            default:
                printf(usage);
                exit(1);
        };
    };

    // Check mandatory options args
    if (!auth) {
        printf(usage);
        exit(1);
    }
    if (!boardID) {
        boardID = auth;
    }
    if (!user) {
        user = auth;
    }
    if (!pass) {
        pass = auth;
    }
}

#endif /* INC_ERA_OPTIONS_ARGS_HPP_ */
