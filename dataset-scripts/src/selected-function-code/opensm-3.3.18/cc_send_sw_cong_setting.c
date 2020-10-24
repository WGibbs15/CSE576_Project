static ib_api_status_t cc_send_sw_cong_setting(osm_sm_t * p_sm,
					       osm_node_t *p_node)
{
	osm_congestion_control_t *p_cc = &p_sm->p_subn->p_osm->cc;
	unsigned force_update;
	osm_physp_t *p_physp;
	osm_madw_t *p_madw = NULL;
	ib_cc_mad_t *p_cc_mad = NULL;
	ib_sw_cong_setting_t *p_sw_cong_setting = NULL;

	OSM_LOG_ENTER(p_sm->p_log);

	p_physp = osm_node_get_physp_ptr(p_node, 0);

	force_update = p_physp->need_update || p_sm->p_subn->need_update;

	if (!force_update
	    && !memcmp(&p_cc->sw_cong_setting,
		       &p_physp->cc.sw.sw_cong_setting,
		       sizeof(p_cc->sw_cong_setting)))
		return IB_SUCCESS;

	p_madw = osm_mad_pool_get(p_cc->mad_pool, p_cc->bind_handle,
				  MAD_BLOCK_SIZE, NULL);
	if (p_madw == NULL) {
		OSM_LOG(p_sm->p_log, OSM_LOG_ERROR, "ERR C101: "
			"failed to allocate mad\n");
		return IB_INSUFFICIENT_MEMORY;
	}

	p_cc_mad = osm_madw_get_cc_mad_ptr(p_madw);

	p_sw_cong_setting = ib_cc_mad_get_mgt_data_ptr(p_cc_mad);

	memcpy(p_sw_cong_setting,
	       &p_cc->sw_cong_setting,
	       sizeof(p_cc->sw_cong_setting));

	cc_mad_post(p_cc, p_madw, p_node, p_physp,
		    IB_MAD_ATTR_SW_CONG_SETTING, 0);

	OSM_LOG_EXIT(p_sm->p_log);

	return IB_SUCCESS;
}