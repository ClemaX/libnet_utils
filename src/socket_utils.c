#include <stdio.h>
#include <unistd.h>


#include <socket_utils.h>

int	socket_icmp(int *socket_type, uint8_t ttl, uint8_t tos)
{
	const int	opt_true = 1;
	int			sd;
	int			setsockopt_err;
	(void)		ttl;
	(void)		tos;

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sd != -1)
	{
		*socket_type = SOCK_RAW;

		setsockopt_err = setsockopt(sd, IPPROTO_IP, IP_HDRINCL,
			&opt_true, sizeof(opt_true));

		if (!setsockopt_err)
			*socket_type = SOCK_RAW;
		else
		{
			close(sd);
			sd = -1;
		}
	}

	if (sd == -1)
	{
#if SOCKET_ICMP_USE_DGRAM
		sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

		if (sd != -1)
		{
			setsockopt_err =
				setsockopt(sd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))
			||	setsockopt(sd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos))
			||	setsockopt(sd, IPPROTO_IP, IP_RECVTTL,
					&opt_true, sizeof(opt_true));

			if (!setsockopt_err)
				*socket_type = SOCK_DGRAM;
		}
		else
#endif
			perror("socket");
	}

	if (sd != -1)
	{
		if (!setsockopt_err)
			setsockopt_err = setsockopt(sd, SOL_SOCKET, SO_TIMESTAMP,
				&opt_true, sizeof(opt_true));

		if (setsockopt_err)
		{
			close(sd);
			sd = -1;
		}
	}

	if (setsockopt_err)
		perror("setsockopt");

	return sd;
}

struct msghdr *socket_msghdr(struct sockaddr_in *src_addr,
	struct iovec *frames, size_t frame_count)
{
	static uint8_t			control[1024];
	static struct msghdr	message = {
		.msg_namelen = sizeof(*src_addr),
		.msg_control = control,
		.msg_controllen = sizeof(control),
		.msg_flags = 0,
	};

	message.msg_name = src_addr;
	message.msg_iov = frames;
	message.msg_iovlen = frame_count;

	return &message;
}

void	socket_packet_stat(struct msghdr *message,
	struct timeval *timestamp, uint8_t *ttl)
{
	*timestamp = (struct timeval){0, 0};

	if (ttl != NULL)
		*ttl = 0;

	for (struct cmsghdr *cframe = CMSG_FIRSTHDR(message); cframe != NULL;
		cframe = CMSG_NXTHDR(message, cframe))
	{
		switch (cframe->cmsg_level)
		{
			case SOL_SOCKET:
				if (cframe->cmsg_type == SO_TIMESTAMP)
					*timestamp = *(struct timeval *)CMSG_DATA(cframe);

				break;

			case SOL_IP:
				if (cframe->cmsg_type == IP_TTL && ttl != NULL)
					*ttl = *(uint8_t*)CMSG_DATA(cframe);

				break;
		}
	}
}
