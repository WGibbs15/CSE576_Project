void osm_pkey_rcv_process(IN void *context, IN void *data)
{
	osm_sm_t *sm = context;
	osm_madw_t *p_madw = data;
	ib_pkey_table_t *p_pkey_tbl;
	ib_smp_t *p_smp;
	osm_port_t *p_port;
	osm_physp_t *p_physp;
	osm_node_t *p_node;
	osm_pkey_context_t *p_context;
	ib_net64_t port_guid;
	ib_net64_t node_guid;
	uint8_t port_num;
	uint16_t block_num;

	CL_ASSERT(sm);

	OSM_LOG_ENTER(sm->p_log);

	CL_ASSERT(p_madw);

	p_smp = osm_madw_get_smp_ptr(p_madw);

	p_context = osm_madw_get_pkey_context_ptr(p_madw);
	p_pkey_tbl = ib_smp_get_payload_ptr(p_smp);

	port_guid = p_context->port_guid;
	node_guid = p_context->node_guid;

	CL_ASSERT(p_smp->attr_id == IB_MAD_ATTR_P_KEY_TABLE);

	if (ib_smp_get_status(p_smp)) {
		OSM_LOG(sm->p_log, OSM_LOG_DEBUG,
			"MAD status 0x%x received\n",
			cl_ntoh16(ib_smp_get_status(p_smp)));
		goto Exit2;
	}

	cl_plock_excl_acquire(sm->p_lock);
	p_port = osm_get_port_by_guid(sm->p_subn, port_guid);
	if (!p_port) {
		OSM_LOG(sm->p_log, OSM_LOG_ERROR, "ERR 4806: "
			"No port object for port with GUID 0x%" PRIx64
			"\n\t\t\t\tfor parent node GUID 0x%" PRIx64
			", TID 0x%" PRIx64 "\n", cl_ntoh64(port_guid),
			cl_ntoh64(node_guid), cl_ntoh64(p_smp->trans_id));
		goto Exit;
	}

	p_node = p_port->p_node;
	CL_ASSERT(p_node);

	block_num = (uint16_t) ((cl_ntoh32(p_smp->attr_mod)) & 0x0000FFFF);
	/* in case of a non switch node the attr modifier should be ignored */
	if (osm_node_get_type(p_node) == IB_NODE_TYPE_SWITCH) {
		port_num =
		    (uint8_t) (((cl_ntoh32(p_smp->attr_mod)) & 0x00FF0000) >>
			       16);
		p_physp = osm_node_get_physp_ptr(p_node, port_num);
	} else {
		p_physp = p_port->p_physp;
		port_num = p_physp->port_num;
	}

	/*
	   We do not care if this is a result of a set or get -
	   all we want is to update the subnet.
	 */
	OSM_LOG(sm->p_log, OSM_LOG_VERBOSE,
		"Got GetResp(PKey) block:%u port_num %u with GUID 0x%"
		PRIx64 " for parent node GUID 0x%" PRIx64 ", TID 0x%"
		PRIx64 "\n", block_num, port_num, cl_ntoh64(port_guid),
		cl_ntoh64(node_guid), cl_ntoh64(p_smp->trans_id));

	/*
	   Determine if we encountered a new Physical Port.
	   If so, ignore it.
	 */
	if (!p_physp) {
		OSM_LOG(sm->p_log, OSM_LOG_ERROR, "ERR 4807: "
			"Got invalid port number %u\n", port_num);
		goto Exit;
	}

	osm_dump_pkey_block_v2(sm->p_log, port_guid, block_num, port_num,
			       p_pkey_tbl, FILE_ID, OSM_LOG_DEBUG);

	osm_physp_set_pkey_tbl(sm->p_log, sm->p_subn,
			       p_physp, p_pkey_tbl, block_num,
			       p_context->set_method);

Exit:
	cl_plock_release(sm->p_lock);

Exit2:
	OSM_LOG_EXIT(sm->p_log);
}