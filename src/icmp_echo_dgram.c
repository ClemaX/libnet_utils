#include <strings.h>

#include <socket_utils.h>
#include <icmp_echo.h>

int	icmp_echo_dgram_send(int sd, const struct sockaddr_in *destination,
	const icmp_echo_packet *request, struct timeval *time)
{
	ssize_t		ret;
	int			status;

	gettimeofday(time, NULL);
	ret = sendto(sd, &request->icmp_header,
		sizeof(*request) - sizeof(request->ip_header), 0,
		(const struct sockaddr*)destination,
		sizeof(*destination));

	status = ret == -1;

	return status;
}

int	icmp_echo_dgram_recv(int sd, struct icmp_response_packet *response,
	struct timeval *time)
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

	bzero(&response->ip_header, sizeof(response->ip_header));

	ret = recvmsg(sd, message, 0);

	status = ret == -1;

	if (status == 0)
	{
		response->size = ret;
		response->ip_header.saddr = src_addr.sin_addr.s_addr;
		socket_packet_stat(message, time, &response->ip_header.ttl);
	}
	else
		response->size = 0;

	return status;
}
