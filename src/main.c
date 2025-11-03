#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipLib.h"


typedef struct prefix_s
{
    union
    {
        unsigned char baseTab[4];
        unsigned int base;
    }ip;
    char mask;
}prefix_t;

prefix_t prefixFromStr(const char *s) 
{
    prefix_t prefix = {.ip.base = 0, .mask = 0};
    sscanf(s, "%hhu.%hhu.%hhu.%hhu/%hhu", &prefix.ip.baseTab[3],
                                 &prefix.ip.baseTab[2],
                                 &prefix.ip.baseTab[1],
                                 &prefix.ip.baseTab[0],
                                 &prefix.mask);
    return prefix;
}



void main(void)
{
    prefix_t prefix;
    int status = 0;

    /* Unit tests for add() and del() functions*/

    printf("- Case where prefix database is empty and we trying remove doesn't exists prefix.: ");
    prefix = prefixFromStr("11.20.0.0/16");
    status = del(prefix.ip.base, prefix.mask);
    if(status == -1) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- Case where we add first prefix to prefix database.: ");
    prefix = prefixFromStr("11.20.0.0/16");
    status = add(prefix.ip.base, prefix.mask);
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- Case where we add existed prefix in prefix database.: ");
    prefix = prefixFromStr("11.20.0.0/16");
    status = add(prefix.ip.base, prefix.mask);
    if(status == -1) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- Case where we have one element in prefix base and trying remove doesn't exists prefix.: ");
    prefix = prefixFromStr("11.20.0.0/17");
    status = del(prefix.ip.base, prefix.mask);
    if(status == -1) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- Case where we have one element in prefix base and trying remove exists prefix.: ");
    prefix = prefixFromStr("11.20.0.0/16");
    status = del(prefix.ip.base, prefix.mask);
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- The case where we add several prefixes to the prefix database.: ");
    char *prefixes[] = {"11.20.0.0/16",
                        "11.20.0.0/15",
                        "11.20.0.0/17",
                        "255.255.255.0/31",
                        "32.64.128.0/20",
                        "32.64.128.0/21",
                        "32.64.128.0/19",
                        "192.168.0.0/16"};

    for(int i = 0; i < sizeof(prefixes)/sizeof(prefixes[0]); i++)
    {
        prefix = prefixFromStr(prefixes[i]);
        status = add(prefix.ip.base, prefix.mask);
        if(status != 0) {break;}
    }
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }
    

    printf("- The case where we delete one prefix from prefix database.: ");
    prefix = prefixFromStr("11.20.0.0/17");
    status = del(prefix.ip.base, prefix.mask);
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- The case where we add one prefix with 0 mask to prefix database.: ");
    prefix = prefixFromStr("192.168.1.0/0");
    status = add(prefix.ip.base, prefix.mask);
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- The case where we add again one prefix with 0 mask to prefix database.: ");
    prefix = prefixFromStr("192.168.1.0/0");
    status = add(prefix.ip.base, prefix.mask);
    if(status == -1) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- The case where we add prefix with 32 mask to prefix database.: ");
    prefix = prefixFromStr("192.167.1.0/32");
    status = add(prefix.ip.base, prefix.mask);
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    /* Unit tests for check() function*/

    printf("THE CASES WHERE WE CHECK PREFIX.\n");

    char expected = 0; 
    char returned = 0;

    /*
        32.64.128.0/21 -> <32.64.128.0 - 32.64.135.255>
        32.64.128.0/20 -> <32.64.128.0 - 32.64.143.255>
    */

    printf("- The case where we check least IP for address network: 32.64.128.0.: ");
    expected = 21;
    prefix = prefixFromStr("32.64.128.0/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("- The case where we check frop IP pool range <32.64.128.0 - 32.64.135.255> for address network: 32.64.128.0.: ");
    expected = 21;
    prefix = prefixFromStr("32.64.128.100/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("- The case where we check the biggest IP for address network: 32.64.128.0.: ");
    expected = 21;
    prefix = prefixFromStr("32.64.135.255/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("- The case where we check the beyond the scope IP <32.64.128.0 - 32.64.135.255> for address network: 32.64.128.0.: ");
    expected = 20;
    prefix = prefixFromStr("32.64.136.0/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }
    
    printf("- The case where we check the IP address for a prefix with a zero mask.: ");
    expected = 0;
    prefix = prefixFromStr("1.2.3.4/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }
    
    printf("- The case where we check the IP address for a prefix with mask = 32.: ");
    expected = 32;
    prefix = prefixFromStr("192.167.1.0/32");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("- The case where we check the IP address for a prefix with mask = 31.: \n");
    printf("    - The case where we check the IP 255.255.255.0.: ");
    expected = 31;
    prefix = prefixFromStr("255.255.255.0/31");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("    - The case where we check the IP 255.255.255.1.: ");
    expected = 31;
    prefix = prefixFromStr("255.255.255.1/31");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    printf("    - The case where we check the IP 255.255.255.2 and this IP belong to zero mask.: ");
    expected = 0;
    prefix = prefixFromStr("255.255.255.2/31");
    returned = check(prefix.ip.base);
    if(returned == expected) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d, expected: %d)\n", returned, expected); }

    /* delete all elements from database */
    printf("- The case where we delete all IP's from proxy database.: ");
    char *prefixesToDelete[] = {"192.168.1.0/0",
                                "192.168.0.0/16",
                                "192.167.1.0/32",
                                "32.64.128.0/21",
                                "32.64.128.0/20",
                                "32.64.128.0/19",
                                "255.255.255.0/31",
                                "11.20.0.0/16",
                                "11.20.0.0/15"};
    for(int i = 0; i < sizeof(prefixesToDelete)/sizeof(prefixesToDelete[0]); i++)
    {
        prefix = prefixFromStr(prefixesToDelete[i]);
        status = del(prefix.ip.base, prefix.mask);
        printf("-----------------------\n");
        if(status != 0) {break;}
    }
    if(status == 0) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    printf("- The case where we delete doesn't exists IP and prefix database is empty.: ");
    prefix = prefixFromStr("32.64.128.0/21");
    status = del(prefix.ip.base, prefix.mask);
    if(status == -1) { printf("PASS\n"); }
    else { printf("FAIL (returned: %d)\n", status); }

    removeMem();//this method is only used to clean up the heap after the application has finished working
}
