#ifndef ICMP_ECHO_H
# define ICMP_ECHO_H

# include <sys/time.h>

# include <icmp_packet.h>

/// Send error
# define ICMP_ECHO_ESEND			0b00000001
/// Receive error
# define ICMP_ECHO_ERECV			0b00000010
/// Time out
# define ICMP_ECHO_ETIMEO			0b00000100
/// Interruption
# define ICMP_ECHO_EINTR			0b00001000

typedef int (icmp_echo_send_fun(int sd, const struct sockaddr_in *destinaton,
	const icmp_echo_packet *request, struct timeval *time));
typedef int (icmp_echo_recv_fun(int sd, icmp_response_packet *response,
	struct timeval *time));

int			icmp_echo_send(int sd, const struct sockaddr_in *destination,
	const icmp_echo_params *params, uint16_t sequence, struct timeval *time);
int			icmp_echo_recv(int sd, const icmp_echo_params *params,
	icmp_response_packet *response, struct timeval *time);


int			icmp_echo_raw_send(int sd, const struct sockaddr_in *destination,
	const icmp_echo_packet *request, struct timeval *time);
int			icmp_echo_raw_recv(int sd, icmp_response_packet *response,
	struct timeval *time);

int			icmp_echo_dgram_send(int sd, const struct sockaddr_in *destination,
	const icmp_echo_packet *request, struct timeval *time);
int			icmp_echo_dgram_recv(int sd, icmp_response_packet *response,
	struct timeval *time);

#endif
