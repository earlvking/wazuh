/* Copyright (C) 2015-2020, Wazuh Inc.
 * Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */


#ifndef SYSCOM_WRAPPERS_H
#define SYSCOM_WRAPPERS_H

#include <stddef.h>

size_t __wrap_syscom_dispatch(char * command, char ** output);

#endif