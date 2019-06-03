#pragma once
#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <linux/sysctl.h>
// #include <net/if.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define RP_FILTER_DISABLE 0
#define SUCCESS 0
#define FAILURE (-1)

int setIpForwardRules(int nv);
int setRpFilterRules(std::string iface, int nv);