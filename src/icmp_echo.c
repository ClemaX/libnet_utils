#include <errno.h>

#include <socket_utils.h>
#include <icmp_echo.h>

static inline int		icmp_echo_recv_error(int error)
{
	int	status;

	if (error == EINTR)
		status = ICMP_ECHO_EINTR;
	else
	{
		status = ICMP_ECHO_ERECV;

		if (error == EAGAIN || error == EWOULDBLOCK)
			status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

int	icmp_echo_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time)
{
  icmp_echo_send_fun	*send_fun;
  int					status;

#if SOCKET_ICMP_USE_DGRAM
	if (params->socket_type == SOCK_RAW)
        send_fun = icmp_echo_raw_send;
	else
		send_fun = icmp_echo_dgram_send;
#else
	(void)socket_type;
	send_fun = icmp_echo_raw;
#endif

	status = send_fun(sd, params, sequence, time);

	if (status != 0)
		status = ICMP_ECHO_ESEND;

	return status;
}

int	icmp_echo_recv(int sd, const icmp_echo_params *params,
	icmp_response_packet *response, struct timeval *time)
{
	icmp_echo_recv_fun	*recv_fun;
	int					status;

#if SOCKET_ICMP_USE_DGRAM
	if (params->socket_type == SOCK_RAW)
		recv_fun = icmp_echo_raw_recv;
	else
		recv_fun = icmp_echo_dgram_recv;
#else
	(void)params;
	recv_fun = icmp_echo_raw;
#endif

	status = recv_fun(sd, response, time);

	if (status == 0)
	{
		const size_t checksum_size =
			response->icmp_header.type == ICMP_ECHOREPLY
			|| response->icmp_header.type == ICMP_ECHO
			? sizeof(response->icmp_header) + sizeof(response->payload.echo)
			: sizeof(response->icmp_header);

		response->is_valid = response->size >= checksum_size
			&& ip_checksum(&response->icmp_header, checksum_size) == 0;
	}
	else
	{
		gettimeofday(time, NULL);
		status = icmp_echo_recv_error(errno);
	}

	return status;
}
