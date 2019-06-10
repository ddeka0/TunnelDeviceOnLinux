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

#include "../include/setupRules.h"
#include<string>

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
    printf("[ip_forward]: Overwritten new value = %d\n", nv);
    close(fd);
    return SUCCESS;
}

int getIpForwardRules()
{
    int fd;
    if((fd=open("/proc/sys/net/ipv4/ip_forward",
                    O_RDONLY), 0666) <0)
    {
        perror("OPEN_RD");
        return FAILURE;
    }
    char oldVal[3];
    if(read(fd, oldVal, 2) < 0)
    {
        perror("READ");
    }
    std::cout<<"[ip_forward:get]: Existing value = "<<oldVal<<std::endl;
    close(fd);
    return std::stoi(oldVal, nullptr, 10);
}

int getRpFilterRules(std::string iface)
{
    int fd;
    std::string fileName = "/proc/sys/net/ipv4/conf/"+iface+"/rp_filter";
    std::cout<<"File Name = "<<fileName<<std::endl;

    if((fd=open(fileName.c_str(), O_RDONLY), 0666) < 0)
    {
        perror("OPEN_READ");
        return FAILURE;
    }

    char oldVal[3];
    if(read(fd, oldVal, 2) < 0)
    {
        perror("READ_CONF");
        return FAILURE;
    }
    std::cout<<"[rp_filter:get] Existing value = "<<oldVal<<std::endl;
    close(fd);
    return std::stoi(oldVal, nullptr, 10);
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
    std::cout<<"["<<iface<<".rp_filter]: Overwritten new value = "
            <<nv<<std::endl;
    close(fd);
    return SUCCESS;
}


/*int main(int argc, char **argv)
{
    char all[]="all";
    //setRpFilterRules(std::string(all), 0);
    printf("old: %d\n", getIpForwardRules());
    setIpForwardRules(0);
    printf("after set: %d\n", getIpForwardRules());
    
    printf("[rp] old: %d\n", getRpFilterRules(std::string(all)));
    setRpFilterRules(std::string(all), 0);
    printf("[rp] after set: %d\n", getRpFilterRules(std::string(all)));
    return 0;
}*/
