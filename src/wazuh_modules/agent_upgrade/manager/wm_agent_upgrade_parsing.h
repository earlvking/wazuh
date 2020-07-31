/*
 * Wazuh Module for Agent Upgrading
 * Copyright (C) 2015-2020, Wazuh Inc.
 * July 15, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */
#ifndef WM_AGENT_UPGRADE_PARSING_H
#define WM_AGENT_UPGRADE_PARSING_H

#include "wm_agent_upgrade_manager.h"

/**
 * Parse received upgrade message and returns params and agents values and a code
 * representing a command if it is valid or an error code otherwise
 * @param buffer message to be parsed
 * @param json_api pointer where the parsed message will be allocated
 * @param params on success command params will be stored in this variable
 * @param pagents on success agents list will be stored in this variable
 * @return error code
 * @retval OS_INVALID on errors
 * @retval UPGRADE if is and upgrade command
 * @retval UPGRADE_RESULT if it is and upgrade_result command
 * */
int wm_agent_upgrade_parse_command(const char* buffer, cJSON** json_api, cJSON **params, cJSON **agents);

/**
 * Parses a response message based on state 
 * @param error_id 1 if error, 0 if successs
 * @param message response message
 * @param agent_id [OPTIONAL] id of the agent
 * @param task_id [OPTIONAL] id of the task
 * @param status [OPTIONAL] status string
 * @return response json
 * */
cJSON* wm_agent_upgrade_parse_response_message(int error_id, const char* message, const int *agent_id, const int* task_id, const char* status);

/**
 * Parses a message to be sent to the request module
 * @param command task command
 * @param agent_id agent id
 * @return json to be sent
 * */
cJSON* wm_agent_upgrade_parse_task_module_message(wm_upgrade_command command, const int agent_id);

/**
 * Parses upgrade command and returns an upgrade task from the information
 * Example:
 * WPK Repository
 * { 
 *      "repository" : "wazuh.packages.com"
 *      "version"    : "3.12",
 *      "use_http"   : "false",
 *      "force_upgrade" : "0"
 * }
 * @param params JSON where the task parameters are 
 * @param output message in case of error
 * @return upgrade task if there is no error, NULL otherwise
 * */
wm_upgrade_task* wm_agent_upgrade_parse_upgrade_command(const cJSON* params, char* output);

/**
 * Parses upgrade custom command and returns an upgrade task from the information
 * Example:
 * Custom WPK Package
 * { 
 *      "file_path" : "./wazuh_wpk"
 *      "installer" : "installer.sh"
 * }
 * @param params JSON where the task parameters are
 * @param output message in case of error
 * @return upgrade task if there is no error, NULL otherwise
 * */
wm_upgrade_custom_task* wm_agent_upgrade_parse_upgrade_custom_command(const cJSON* params, char* output);

extern const char* upgrade_commands[];

#endif
