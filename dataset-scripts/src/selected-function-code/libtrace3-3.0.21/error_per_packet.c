void error_per_packet(struct libtrace_packet_t *packet)
{
	struct libtrace_ip *ip = trace_get_ip(packet);
	struct libtrace_tcp *tcp = trace_get_tcp(packet);
	void *link = trace_get_packet_buffer(packet,NULL,NULL);
	if (!link) {
		++rx_errors;
	}
	
	/* This isn't quite as simple as it seems.
	 *
	 * If the packets were captured via wdcap's anonymisation module,
	 * the checksum is set to 0 when it is correct and 1 if incorrect.
	 *
	 * If a different capture method is used, there's a good chance the
	 * checksum has not been altered
	 */
	if (ip) {
		if (ntohs(ip->ip_sum)!=0)
			++ip_errors;
	}
	if (tcp) {
		if (ntohs(tcp->check)!=0)
			++tcp_errors;
	}
}