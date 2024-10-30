#ifndef icmp_echo_packet_H
# define icmp_echo_packet_H

# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>

# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <ip_utils.h>

# define ICMP_ECHO_PAYLOAD \
	"ABCDEF**************************************************"

typedef struct iphdr	ip_header;
typedef struct icmphdr	icmp_header;

typedef struct					icmp_echo_packet
{
	ip_header	ip_header;
	icmp_header	icmp_header;
	uint8_t		payload[56];
}	__attribute__((__packed__))	icmp_echo_packet;


typedef struct					icmp_dest_unreach_payload
{
	ip_header	ip_header;
	uint8_t		data[64];
}	__attribute__((__packed__))	icmp_dest_unreach_payload;

typedef struct					icmp_response_packet
{
	ip_header	ip_header;
	icmp_header	icmp_header;
	union {
		uint8_t						echo[56];
		icmp_dest_unreach_payload	dest_unreach;
	}			payload;
}	__attribute__((__packed__))	icmp_response_packet;

typedef struct	icmp_echo_params
{
	struct sockaddr_in	destination;
	int					socket_type;
	uint16_t			id;
	uint8_t				time_to_live;
	uint8_t				type_of_service;
}				icmp_echo_params;

icmp_echo_packet	*icmp_echo_request(const struct icmp_echo_params *params,
	uint16_t sequence);
void			icmp_echo_dump(FILE *file, const icmp_echo_packet *packet,
	bool dump_ip_header);

const char		*icmp_type_strerror(uint8_t type);

#endif
