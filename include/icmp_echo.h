#ifndef ICMP_ECHO_H
# define ICMP_ECHO_H

# include <sys/time.h>

# include <icmp_packet.h>

# define ICMP_ECHO_ESEND			0b00000001
# define ICMP_ECHO_ERECV			0b00000010
# define ICMP_ECHO_ETIMEO			0b00000100
# define ICMP_ECHO_EDEST_UNREACH	0b00001000
# define ICMP_ECHO_ESOURCE_QUENCH	0b00010000
# define ICMP_ECHO_EREDIRECT		0b00100000
# define ICMP_ECHO_ECHECKSUM		0b01000000

typedef int (icmp_echo_send_fun(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time));
typedef int (icmp_echo_recv_fun(int sd, struct icmp_packet *response, struct timeval *time));

int			icmp_echo_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_recv(int sd, const icmp_echo_params *params, struct icmp_packet *response,
	struct timeval *time);

int			icmp_echo_raw_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_raw_recv(int sd, struct icmp_packet *response, struct timeval *time);

int			icmp_echo_dgram_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_dgram_recv(int sd, struct icmp_packet *response, struct timeval *time);

#endif
