/*
 * Wazuh Module for Agent Upgrading
 * Copyright (C) 2015-2020, Wazuh Inc.
 * July 30, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */
#include "wazuh_modules/wmodules.h"
#include "wm_agent_upgrade_agent.h"

const char* upgrade_values[] = {
    [WM_UPGRADE_SUCCESSFULL] = "0",
    [WM_UPGRADE_FAILED] = "2"
};

const char* upgrade_messages[] = {
    [WM_UPGRADE_SUCCESSFULL] = "Upgrade was successfull",
    [WM_UPGRADE_FAILED]      = "Upgrade failed"
};

const int command_code[] = {
    [WM_UPGRADE_SUCCESSFULL] = WM_UPGRADE_AGENT_UPGRADED,
    [WM_UPGRADE_FAILED]      = WM_UPGRADE_AGENT_UPGRADE_FAILED
};

static void wm_upgrade_agent_send_ack_message(int queue_fd, wm_upgrade_agent_state state);

void wm_agent_upgrade_check_status() {
    char buffer[20];
    FILE * result_file;
    const char * PATH = WM_AGENT_UPGRADE_RESULT_FILE;
    /**
     *  StartMQ will wait until agent connection which is when the pkg_install.sh will write 
     *  the upgrade result
    */
    int queue_fd = StartMQ(DEFAULTQPATH, WRITE, MAX_OPENQ_ATTEMPS);

    if (queue_fd < 0) {
        mterror(WM_AGENT_UPGRADE_LOGTAG, WM_UPGRADE_QUEUE_FD);
    } else {
        #ifndef WIN32
            if (result_file = fopen(PATH, "r"), result_file) {
        #else
            if (result_file = fopen(PATH, "rb"), result_file) {
        #endif
            fgets(buffer,20,result_file);
            fclose(result_file);
        }

        wm_upgrade_agent_state state;
        for(state = 0; state <= WM_UPGRADE_MAX_AGENT_STATE; state++) {
            if (strcmp(buffer, upgrade_values[state]) >= 0) {
                // Matched value, send message
                wm_upgrade_agent_send_ack_message(queue_fd, state);
            }
        }
        
        close(queue_fd);
    }
}


static void wm_upgrade_agent_send_ack_message(int queue_fd, wm_upgrade_agent_state state) {
    int msg_delay = 1000000 / wm_max_eps;
    cJSON* root = cJSON_CreateObject();
    // TODO review this
    int code = command_code[state];
    cJSON_AddStringToObject(root, "command", upgrade_commands[code]);
    cJSON* params = cJSON_CreateObject();
    cJSON_AddNumberToObject(params, "error", atoi(upgrade_values[state]));
    cJSON_AddStringToObject(params, "message", upgrade_messages[state]);
    cJSON_AddItemToObject(root,"params",params);

    char *msg_string = cJSON_PrintUnformatted(root);
    if (wm_sendmsg(msg_delay, queue_fd, msg_string, WM_AGENT_UPGRADE_MODULE_NAME, UPGRADE_MQ) < 0) {
        mterror(WM_AGENT_UPGRADE_LOGTAG, QUEUE_ERROR, DEFAULTQUEUE, strerror(errno));
    }
    mtdebug1(WM_AGENT_UPGRADE_LOGTAG, WM_UPGRADE_ACK_MESSAGE, msg_string);
    os_free(msg_string);
    cJSON_Delete(root);
}
