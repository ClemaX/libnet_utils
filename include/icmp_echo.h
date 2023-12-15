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

typedef int (icmp_echo_fun(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2]));


int			icmp_echo(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2]);

int			icmp_echo_error(int status, const icmp_header header);

int			icmp_echo_raw(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2]);

int			icmp_echo_dgram(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2]);

#endif
