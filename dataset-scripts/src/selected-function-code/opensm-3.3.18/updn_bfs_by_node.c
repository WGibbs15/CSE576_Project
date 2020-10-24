static int updn_bfs_by_node(IN osm_log_t * p_log, IN osm_subn_t * p_subn,
			    IN osm_switch_t * p_sw)
{
	uint8_t pn, pn_rem;
	cl_qlist_t list;
	uint16_t lid;
	struct updn_node *u;
	updn_switch_dir_t next_dir, current_dir;

	OSM_LOG_ENTER(p_log);

	lid = osm_node_get_base_lid(p_sw->p_node, 0);
	lid = cl_ntoh16(lid);
	osm_switch_set_hops(p_sw, lid, 0, 0);

	OSM_LOG(p_log, OSM_LOG_DEBUG,
		"Starting from switch - port GUID 0x%" PRIx64 " lid %u\n",
		cl_ntoh64(p_sw->p_node->node_info.port_guid), lid);

	u = p_sw->priv;
	u->dir = UP;

	/* Update list with the new element */
	cl_qlist_init(&list);
	cl_qlist_insert_tail(&list, &u->list);

	/* BFS the list till no next element */
	while (!cl_is_qlist_empty(&list)) {
		u = (struct updn_node *)cl_qlist_remove_head(&list);
		u->visited = 0;	/* cleanup */
		current_dir = u->dir;
		/* Go over all ports of the switch and find unvisited remote nodes */
		for (pn = 1; pn < u->sw->num_ports; pn++) {
			osm_node_t *p_remote_node;
			struct updn_node *rem_u;
			uint8_t current_min_hop, remote_min_hop,
			    set_hop_return_value;
			osm_switch_t *p_remote_sw;

			p_remote_node =
			    osm_node_get_remote_node(u->sw->p_node, pn,
						     &pn_rem);
			/* If no remote node OR remote node is not a SWITCH
			   continue to next pn */
			if (!p_remote_node || !p_remote_node->sw)
				continue;
			/* Fetch remote guid only after validation of remote node */
			p_remote_sw = p_remote_node->sw;
			rem_u = p_remote_sw->priv;
			/* Decide which direction to mark it (UP/DOWN) */
			next_dir = updn_get_dir(u->rank, rem_u->rank,
						u->id, rem_u->id);

			/* Check if this is a legal step : the only illegal step is going
			   from DOWN to UP */
			if ((current_dir == DOWN) && (next_dir == UP)) {
				OSM_LOG(p_log, OSM_LOG_DEBUG,
					"Avoiding move from 0x%016" PRIx64
					" to 0x%016" PRIx64 "\n",
					cl_ntoh64(osm_node_get_node_guid(u->sw->p_node)),
					cl_ntoh64(osm_node_get_node_guid(p_remote_node)));
				/* Illegal step */
				continue;
			}
			/* Set MinHop value for the current lid */
			current_min_hop = osm_switch_get_least_hops(u->sw, lid);
			/* Check hop count if better insert into list && update
			   the remote node Min Hop Table */
			remote_min_hop =
			    osm_switch_get_hop_count(p_remote_sw, lid, pn_rem);
			if (current_min_hop + 1 < remote_min_hop) {
				set_hop_return_value =
				    osm_switch_set_hops(p_remote_sw, lid,
							pn_rem,
							current_min_hop + 1);
				if (set_hop_return_value) {
					OSM_LOG(p_log, OSM_LOG_ERROR, "ERR AA01: "
						"Invalid value returned from set min hop is: %d\n",
						set_hop_return_value);
				}
				/* Check if remote port has already been visited */
				if (!rem_u->visited) {
					/* Insert updn_switch item into the list */
					rem_u->dir = next_dir;
					rem_u->visited = 1;
					cl_qlist_insert_tail(&list,
							     &rem_u->list);
				}
			}
		}
	}

	OSM_LOG_EXIT(p_log);
	return 0;
}