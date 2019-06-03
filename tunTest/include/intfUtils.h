#pragma once
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
// https://elixir.bootlin.com/linux/v3.10/source/Documentation/networking/ifenslave.c#L990
static int skfd = -1;
int set_if_flags(char *ifname, short flags);
int set_if_up(std::string intfName, short flags);
int set_if_down(std::string intfName, short flags);
int check_if_is_up(std::string intfName);