/**
 * Program to set up ip_forward & rp_filter rules form userspace as an 
 * alternative to set them up from command lines.
 * From command line:
 *      sudo sysctl -w net.ipv4.conf.tun2.rp_filter=0
 *      sudo sysctl -w net.ipv4.conf.all.rp_filter=0
 *      sudo sysctl -w net.ipv4.ip_forward=1
 * Instead, here we are directly writing the new values into the following
 * files:
 *      /proc/sys/net/ipv4/ip_forward
 *      /proc/sys/net/ipv4/conf/<iface>/rp_filter
 */

#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <linux/sysctl.h>
#include <net/if.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define SUCCESS 0
#define FAILURE (-1)

int setIpForwardRules(int nv)
{
    int fd;
    if ((fd=open("/proc/sys/net/ipv4/ip_forward", 
            O_WRONLY|O_CREAT|O_TRUNC), 0666) < 0) 
    {
        perror("OPEN"); 
        return FAILURE;
    }

    if(nv!=0 && nv!=1)
    {
        printf("Error: Invalid value given\n");
        return FAILURE;
    }
    char newVal[3];
    sprintf(newVal, "%d", nv);
    strncat(newVal, "\n", 1);
    if (write (fd, newVal, 2) < 0)
        perror("WRITE"); 
    printf("Overwritten new value = %d\n", nv);
    close(fd);
    return SUCCESS;
}


int setRpFilterRules(std::string iface, int nv)
{
    int fd;
    std::string fileName = "/proc/sys/net/ipv4/conf/"+iface+"/rp_filter";
    std::cout<<"File name = "<<fileName<<std::endl;

    if ((fd=open(fileName.c_str(), O_WRONLY|O_CREAT|O_TRUNC), 0666) < 0) 
    {
        perror("OPEN"); 
        return FAILURE;
    }

    /*if(nv!=0)
    {
        printf("Error: Invalid value given\n");
        return FAILURE;
    }*/
    char newVal[3];
    sprintf(newVal, "%d", nv);
    strncat(newVal, "\n", 1);
    if (write (fd, newVal, 2) < 0)
        perror("WRITE"); 
    printf("Overwritten new value = %d\n", nv);
    close(fd);
    return SUCCESS;
}


int main(int argc, char **argv)
{
    char all[]="all";
    setRpFilterRules(std::string(all), 0);
    return 0;
}