#ifndef LICENSE_PLATFORM_ID_HPP
#define LICENSE_PLATFORM_ID_HPP

#include <vector>
#include <string>
#include <cstdint>

#if defined(unix) || defined(__unix__) || defined(__unix)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>

#else
#include <winsock2.h>
#include <iphlpapi.h>

#endif

namespace license
{


static inline int get_mac_address(char mac_address[18], const std::string &if_name)
{
	std::uint8_t *mac = nullptr;

#if defined(unix) || defined(__unix__) || defined(__unix)
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		return 1;
	}

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, if_name.c_str(), IFNAMSIZ-1);

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) !=0) {
		return 1;
	}

	if (close(fd) != 0) {
		return 1;
	}

	mac = reinterpret_cast<std::uint8_t *>(ifr.ifr_hwaddr.sa_data);

#else
   IP_ADAPTER_INFO AdapterInfo[16];
   DWORD dwBufLen = sizeof (AdapterInfo);

   DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
   if (dwStatus != ERROR_SUCCESS) {
	   return 1;
   }

   PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
   do {
	   std::string adapter_name (pAdapterInfo->AdapterName);
	   if (if_name == adapter_name) {
		   mac = reinterpret_cast<std::uint8_t *>(pAdapterInfo->Address);
		   break;
	   }
      pAdapterInfo = pAdapterInfo->Next;
   } while (pAdapterInfo);

#endif

   if (mac == nullptr) {
	   return 1;
   }

	for(int i=0; i<6; i++) {
		sprintf(mac_address+i*3, "%02x", mac[i]);
		*(mac_address+i*3+2) = ':';
	}
	mac_address[17] = '\0';
	return 0;
}



}

#endif
