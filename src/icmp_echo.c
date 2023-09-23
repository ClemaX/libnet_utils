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
