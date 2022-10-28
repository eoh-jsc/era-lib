#ifndef INC_ERA_OPTIONS_ARGS_HPP_
#define INC_ERA_OPTIONS_ARGS_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ERa/ERaConfig.hpp>

static void processArgs(int argc, char* argv[],
                        const char*& auth,
                        const char*& host,
                        uint16_t& port)
{
    static struct option long_options[] = {
        {"token",   required_argument,   NULL, 't'},
        {"host",    required_argument,   NULL, 'h'},
        {"port",    required_argument,   NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    // Set Default values
    auth = NULL;
    host = ERA_MQTT_HOST;
    port = ERA_MQTT_PORT;

    const char* usage =
        "Usage: era [options]\n"
        "\n"
        "Options:\n"
        "  -t auth, --token=auth    Your auth token\n"
        "  -h addr, --host=addr     Server name (default: " ERA_MQTT_HOST ")\n"
        "  -p num,  --port=num      Server port (default: " ERA_TOSTRING(ERA_MQTT_PORT) ")\n"
        "\n";

    int rez;
    while (-1 != (rez = getopt_long(argc, argv,"t:h:p:", long_options, NULL))) {
        switch (rez) {
            case 't':
                auth = optarg;
                break;
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
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
}

#endif /* INC_ERA_OPTIONS_ARGS_HPP_ */
