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
/// Invalid checksum
# define ICMP_ECHO_ECHECKSUM		0b00001000
/// Interruption
# define ICMP_ECHO_EINTR			0b00010000

typedef int (icmp_echo_send_fun(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time));
typedef int (icmp_echo_recv_fun(int sd, icmp_response_packet *response,
	size_t *size, struct timeval *time));

int			icmp_echo_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_recv(int sd, const icmp_echo_params *params,
	icmp_response_packet *response, size_t *response_size,
	struct timeval *time);

int			icmp_echo_raw_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_raw_recv(int sd, icmp_response_packet *response,
	size_t *size, struct timeval *time);

int			icmp_echo_dgram_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time);
int			icmp_echo_dgram_recv(int sd, icmp_response_packet *response,
	size_t *size, struct timeval *time);

#endif
