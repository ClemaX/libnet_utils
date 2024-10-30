#include <icmp_packet.h>

icmp_echo_packet	*icmp_echo_request(const struct icmp_echo_params *params, uint16_t sequence)
{
	static icmp_echo_packet	packet =
	{
		.ip_header =
		{
			.version = 4,
			.ihl = sizeof(packet.ip_header) / 4,
			.tos = 0,
			.tot_len = sizeof(icmp_echo_packet),
			.id = 0,
			.frag_off = 0,
			.ttl = 64,
			.protocol = IPPROTO_ICMP,
		},
		.icmp_header =
		{
			.type = ICMP_ECHO,
			.code = 0,
		},
		.payload = ICMP_ECHO_PAYLOAD,
	};

	packet.ip_header.daddr = params->destination.sin_addr.s_addr;

	packet.ip_header.frag_off = htons(0 | IP_DF);
	packet.ip_header.ttl = params->time_to_live;
	packet.ip_header.tos = params->type_of_service;

	packet.icmp_header.un.echo.id = htons(params->id);
	packet.icmp_header.un.echo.sequence = htons(sequence);

	packet.icmp_header.checksum = 0;
	packet.icmp_header.checksum = ip_checksum(&packet.icmp_header,
		sizeof(packet.icmp_header) + sizeof(packet.payload));

	return &packet;
}

const char	*icmp_type_strerror(uint8_t type)
{
	const char	*type_strs[NR_ICMP_TYPES] = {
		[ICMP_DEST_UNREACH] = "Destination unreachable",
		[ICMP_SOURCE_QUENCH] = "Source quench",
		[ICMP_REDIRECT] = "Redirect (change route)",
		[ICMP_TIME_EXCEEDED] = "Time to live exceeded",
		[ICMP_PARAMETERPROB] = "Parameter problem"
	};

	return type_strs[type];
}
