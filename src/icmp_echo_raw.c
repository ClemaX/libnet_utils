#include <errno.h>

#include <socket_utils.h>
#include <icmp_echo.h>

int	icmp_echo_raw_send(int sd, const icmp_echo_params *params,
	uint16_t sequence, struct timeval *time)
{
	const icmp_echo_packet	*request;
	ssize_t				ret;
	int					status;

	request = icmp_echo_request(params, sequence);

	gettimeofday(time, NULL);
	ret = sendto(sd, request, sizeof(*request), 0,
		(const struct sockaddr*)&params->destination,
		sizeof(params->destination));

	status = ret == -1;

	return status;
}

int	icmp_echo_raw_recv(int sd, icmp_response_packet *response,
	struct timeval *time)
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

	do {
		ret = recvmsg(sd, message, 0);
	} while (ret == -1
		&& errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR);

	status = ret == -1;

	if (status == 0)
	{
		response->size = ret - sizeof(response->ip_header);
		socket_packet_stat(message, time, NULL);
	}
	else
		response->size = 0;

	return status;
}
