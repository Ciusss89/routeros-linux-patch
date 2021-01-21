#ifndef __LINUX_POEOUT_H
#define __LINUX_POEOUT_H

struct poeout_port {
    int eth_port;
    int gpo_on;
    int gpi_status;
    int gpi_status_valid;
    int invert_gpo_on:1;
};

#endif
