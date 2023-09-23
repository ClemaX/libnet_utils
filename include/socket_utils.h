#ifndef SOCKET_UTILS_H
# define SOCKET_UTILS_H

#include <netinet/in.h>
#include <sys/time.h>

# define SOCKET_ICMP_USE_DGRAM 1

int				socket_icmp(int *socket_type, uint8_t ttl, uint8_t tos);

struct msghdr	*socket_msghdr(struct sockaddr_in *src_addr,
	struct iovec *frames, size_t frame_count);

void			socket_packet_stat(struct msghdr *message,
	struct timeval *timestamp, uint8_t *ttl);

#endif
