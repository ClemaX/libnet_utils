#include <sys/time.h>

#include <stdio.h>
#include <errno.h>

#include <netinet/in.h>

#include <socket_utils.h>
#include <icmp_echo.h>

static int	icmp_echo_send(int sd, const icmp_echo_params *params,
	struct timeval *time)
{
	icmp_packet	*request;
	ssize_t		ret;
	int			status;

	request = icmp_echo_request(params);

	gettimeofday(time, NULL);

	ret = sendto(sd, &request->icmp_header,
		sizeof(*request) - sizeof(request->ip_header), 0,
		(const struct sockaddr*)&params->destination,
		sizeof(params->destination));

	status = ret == -1;

	if (status != 0)
	{
		status = ICMP_ECHO_ESEND;

		if (errno == EAGAIN || errno == EWOULDBLOCK)
			status |= ICMP_ECHO_ETIMEO;
	}

	return status;
}

static int	icmp_echo_recv(int sd, struct icmp_packet *response, struct timeval *time)
{
	static struct sockaddr_in	src_addr;
	static struct iovec			frames[] =
	{
		{NULL, sizeof(response->icmp_header)},
		{NULL, sizeof(response->payload)},
	};
	struct msghdr *const		message = socket_msghdr(&src_addr, frames,
		sizeof(frames) / sizeof(*frames));
	ssize_t						ret;
	int							status;

	frames[0].iov_base = &response->icmp_header;
	frames[1].iov_base = &response->payload;

	ret = recvmsg(sd, message, 0);

	status = ret != sizeof(*response) - sizeof(response->ip_header);

	status = icmp_echo_error(status, response->icmp_header);

	if (status == 0)
	{
		socket_packet_stat(message, time, &response->ip_header.ttl);
		response->ip_header.saddr = ((struct sockaddr_in*)message->msg_name)->sin_addr.s_addr;
	}

	return status;
}

int			icmp_echo_dgram(int sd, const icmp_echo_params *params,
	icmp_packet *response, struct timeval t[2])
{
	int	status;

	status = icmp_echo_send(sd, params, &t[0]);

	if (status == 0)
		status = icmp_echo_recv(sd, response, &t[1]);

	return status;
}
