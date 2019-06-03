#pragma once
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <linux/sysctl.h>
#define ADD_ROUTE   1
#define DEL_ROUTE   0
#define GEN_MASK "255.255.255.0"
#define FORWARD_ENABLE 1
#define FORWARD_DISABLE 0

int configRoute(std::string intfName,std::string dstIp,int flag);