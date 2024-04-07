#include <asm-generic/errno.h>
#include <errno.h>

#include <socket_utils.h>
#include <icmp_echo.h>

int icmp_echo(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2])
{
	icmp_echo_fun	*echo_fun;

#if SOCKET_ICMP_USE_DGRAM
	if (params->socket_type == SOCK_RAW)
		echo_fun = icmp_echo_raw;
	else
		echo_fun = icmp_echo_dgram;
#else
	(void)socket_type;
	echo_fun = icmp_echo_raw;
#endif

	return echo_fun(sd, params, response, t);
}

static int icmp_echo_response_validate(const icmp_header header) {
	static const int	status_map[NR_ICMP_TYPES] = {
		[ICMP_DEST_UNREACH] = ICMP_ECHO_EDEST_UNREACH,
		[ICMP_SOURCE_QUENCH] = ICMP_ECHO_EDEST_UNREACH,
		[ICMP_REDIRECT] = ICMP_ECHO_EREDIRECT,
	};

	int	status;

	status = header.type != ICMP_ECHOREPLY;

	if (status != 0) {
		status = status_map[header.type];

		status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

static int icmp_echo_recv_error() {
	int	status;

	status = ICMP_ECHO_ERECV;

	if (errno == EAGAIN || errno == EWOULDBLOCK)
		status |= ICMP_ECHO_ETIMEO;
	else if (errno == ENETDOWN || errno == ENETUNREACH || errno == ENETRESET
	|| errno == EHOSTDOWN || errno == EHOSTUNREACH)
		status |= ICMP_ECHO_EDEST_UNREACH;

	return status;
}

int icmp_echo_error(int status, const icmp_header header)
{
	if (status == 0)
		status = icmp_echo_response_validate(header);
	else
		status = icmp_echo_recv_error();

	return status;
}
