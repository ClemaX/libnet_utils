#include <errno.h>

#include <socket_utils.h>
#include <icmp_echo.h>

static int	icmp_echo_recv_error(void)
{
	int	status;

	status = ICMP_ECHO_ERECV;

	if (errno == EAGAIN || errno == EWOULDBLOCK)
		status |= ICMP_ECHO_ETIMEO;
	else if (errno == ENETDOWN || errno == ENETUNREACH || errno == ENETRESET
	|| errno == EHOSTDOWN || errno == EHOSTUNREACH)
		status |= ICMP_ECHO_EDEST_UNREACH;

	return status;
}

static int	icmp_echo_validate_type(const icmp_packet *packet, uint16_t type)
{
	static const int	status_map[NR_ICMP_TYPES] = {
		[ICMP_DEST_UNREACH] = ICMP_ECHO_EDEST_UNREACH,
		[ICMP_SOURCE_QUENCH] = ICMP_ECHO_EDEST_UNREACH,
		[ICMP_REDIRECT] = ICMP_ECHO_EREDIRECT,
	};
	int	status;

	status = packet->icmp_header.type != type;

	if (status != 0)
	{
		dprintf(2, "Invalid header type %d!", packet->icmp_header.type);
		status = status_map[packet->icmp_header.type];

		status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

static int	icmp_echo_response_validate(const icmp_packet *response)
{
	int status;

	if (ip_checksum(&response->icmp_header,
		sizeof(response->icmp_header) + sizeof(response->payload)) != 0)
		status = ICMP_ECHO_ECHECKSUM;
	else {
		status = icmp_echo_validate_type(response, ICMP_ECHOREPLY);
	}

	return status;
}

int			icmp_echo_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time)
{
  icmp_echo_send_fun *send_fun;

#if SOCKET_ICMP_USE_DGRAM
	if (params->socket_type == SOCK_RAW)
        send_fun = icmp_echo_raw_send;
	else
		send_fun = icmp_echo_dgram_send;
#else
	(void)socket_type;
	send_fun = icmp_echo_raw;
#endif

	return send_fun(sd, params, sequence, time);
}

int			icmp_echo_recv(int sd, const icmp_echo_params *params,
	struct icmp_packet *response, struct timeval *time)
{
	int status;
	icmp_echo_recv_fun *recv_fun;

#if SOCKET_ICMP_USE_DGRAM
	if (params->socket_type == SOCK_RAW)
		recv_fun = icmp_echo_raw_recv;
	else
		recv_fun = icmp_echo_dgram_recv;
#else
	(void)socket_type;
	recv_fun = icmp_echo_raw;
#endif

	do {
		status = recv_fun(sd, response, time);

		if (status != 0)
			status = icmp_echo_recv_error();
		else
			status = icmp_echo_response_validate(response);
		// TODO: Maybe need to verify src_addr
	}
	while (status == 0
#if SOCKET_ICMP_USE_DGRAM
		&& params->socket_type == SOCK_RAW
#endif
		&& htons(response->icmp_header.un.echo.id) != params->id);

	return status;
}
