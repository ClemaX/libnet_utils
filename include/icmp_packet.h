#ifndef ICMP_PACKET_H
# define ICMP_PACKET_H

# include <stdint.h>

# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <ip_utils.h>

# define ICMP_ECHO_PAYLOAD \
	"********************************************************"

typedef struct iphdr	ip_header;
typedef struct icmphdr	icmp_header;

typedef struct					icmp_packet
{
	ip_header	ip_header;
	icmp_header	icmp_header;
	uint8_t		payload[56];
}	__attribute__((__packed__))	icmp_packet;

typedef struct	icmp_echo_params
{
	struct sockaddr_in	destination;
	int					socket_type;
	uint16_t			id;
	uint16_t			sequence;
	uint8_t				time_to_live;
	uint8_t				type_of_service;
}				icmp_echo_params;

icmp_packet		*icmp_echo_request(const struct icmp_echo_params *params);

#endif
