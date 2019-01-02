#ifndef LICENSE_HW_RESOURCES_HPP
#define PRIVATE_HW_RESOURCES_HPP

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

namespace license
{

static inline int get_mac_address(char mac_address[18], const char *if_name)
{
	int fd;
	struct ifreq ifr;
	std::uint8_t *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		return -1;
	}

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) !=0) {
		return -1;
	}

	if (close(fd) != 0) {
		return -1;
	}

	mac = (uint8_t *) ifr.ifr_hwaddr.sa_data;

	for(int i=0; i<6; i++) {
		sprintf(mac_address+i*3, "%02x", mac[i]);
		*(mac_address+i*3+2) = ':';
	}
	mac_address[17] = '\0';

	return 0;
}

}

#endif
