#include <sys/time.h>

#include <stdio.h>
#include <errno.h>

#include <netinet/in.h>

#include <socket_utils.h>
#include <icmp_echo.h>
#include <time_utils.h>


static int	icmp_echo_send(int sd, const icmp_echo_params *params,
	struct timeval *time)
{
	icmp_packet	*request;
	ssize_t		ret;
	int			status;

	request = icmp_echo_request(params);

	gettimeofday(time, NULL);

	ret = sendto(sd, request, sizeof(*request), 0,
		(const struct sockaddr*)params->destination,
		sizeof(*params->destination));

	status = ret == -1;

	if (status != 0)
	{
		status = ICMP_ECHO_ESEND;

		if (errno == EAGAIN || errno == EWOULDBLOCK)
			status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

static int	icmp_echo_recv(int sd, const struct sockaddr_in *addr,
	struct icmp_packet *response, struct timeval *time)
{
	static struct sockaddr_in	src_addr;
	static struct iovec			frames[] =
	{
		{NULL, sizeof(response->ip_header)},
		{NULL, sizeof(response->icmp_header)},
		{NULL, sizeof(response->payload)},
	};
	struct msghdr *const		message = socket_msghdr(&src_addr, frames,
		sizeof(frames) / sizeof(*frames));
	ssize_t						ret;
	int							status;

	frames[0].iov_base = &response->ip_header;
	frames[1].iov_base = &response->icmp_header;
	frames[2].iov_base = &response->payload;

	ret = recvmsg(sd, message, 0);

	status = ret != sizeof(*response);

	if (status == 0 && response->icmp_header.type == ICMP_ECHO)
	{
		ret = recvmsg(sd, message, 0);
		status = ret != sizeof(*response);
	}

	if (status == 0)
	{
		if (response->icmp_header.type == ICMP_ECHOREPLY
			&& response->ip_header.saddr == addr->sin_addr.s_addr)
			socket_packet_stat(message, time, &response->ip_header.ttl);
		else
		{
			status = ICMP_ECHO_ERECV;

			if (response->icmp_header.type == ICMP_TIME_EXCEEDED)
				status |= ICMP_ECHO_ETIMEO;
		}
	}
	else
	{
		status = ICMP_ECHO_ERECV;

		if (errno == EAGAIN || errno == EWOULDBLOCK)
			status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

int			icmp_echo_raw(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2])
{
	int	status;

	status = icmp_echo_send(sd, params, &t[0]);

	if (status == 0)
		status = icmp_echo_recv(sd, params->destination, response, &t[1]);

	return status;
}
