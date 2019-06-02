#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/sysctl.h>
#include <linux/sysctl.h>
#include<net/if.h>

#define SUCCESS 0
#define FAILURE (-1)

int setIpForwardRules(int newval[])
{
    int name[] = {CTL_NET, NET_IPV4, NET_IPV4_FORWARD};
    int namelen = 3;
    int oldval[1];
    size_t len = sizeof(oldval);
    int i, error;
    //int newval[] = {1};
    size_t newlen = sizeof(newval);
    
    // setting new value via sysctl    
    error = sysctl (name, namelen, (void *)oldval, &len, newval , newlen);
    if (error) {
        fprintf(stderr,"sysctl() %s\n", strerror(error));
        return FAILURE;
    }
    /*printf("%d %d\n", len, newlen);
    printf("len is %i bytes\n", len);
    for (i = 0; i < len/(sizeof(int)); i++)
            printf("%i\t", oldval[i]);
    printf("\n");*/
    return SUCCESS;
}


int setRpFilterRules(char *iface, int newval[])
{
    int name[] = {CTL_NET, NET_IPV4, NET_IPV4_CONF, NET_PROTO_CONF_ALL, NET_IPV4_CONF_RP_FILTER};
    
    if(!strncmp(iface, "all", 3))
    {
        unsigned int idx = if_nametoindex(iface);
        printf("iface name = %s, ifindex = %u\n", iface, idx);
        name[3] = name[3]&idx;
    }

    int namelen = 5;
    int oldval[1];
    size_t len = sizeof(oldval);
    int i, error;
    //int newval[] = {1};
    size_t newlen = sizeof(newval);
    
    // setting new value via sysctl    
    error = sysctl (name, namelen, (void *)oldval, &len, newval , newlen);
    if (error) {
        fprintf(stderr,"sysctl() %s\n", strerror(error));
        return FAILURE;
    }
    /*printf("%d %d\n", len, newlen);
    printf("len is %i bytes\n", len);
    for (i = 0; i < len/(sizeof(int)); i++)
            printf("%i\t", oldval[i]);
    printf("\n");*/
    return SUCCESS;
}


int main(int argc, char **argv)
{
        return 0;
}