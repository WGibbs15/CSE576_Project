ib_api_status_t
osmt_get_service_by_name(IN osmtest_t * const p_osmt,
			 IN char *sr_name,
			 IN uint32_t rec_num,
			 OUT ib_service_record_t * p_out_rec)
{

	ib_api_status_t status = IB_SUCCESS;
	osmtest_req_context_t context;
	osmv_query_req_t req;
	ib_service_record_t *p_rec;
	ib_svc_name_t service_name;
	uint32_t num_recs = 0;

	OSM_LOG_ENTER(&p_osmt->log);

	OSM_LOG(&p_osmt->log, OSM_LOG_VERBOSE,
		"Getting service record: name: %s\n", sr_name);

	/*
	 * Do a blocking query for this record in the subnet.
	 * The result is returned in the result field of the caller's
	 * context structure.
	 *
	 * The query structures are locals.
	 */
	memset(&req, 0, sizeof(req));
	memset(&context, 0, sizeof(context));

	context.p_osmt = p_osmt;

	/* prepare the data used for this query */
	req.query_type = OSMV_QUERY_SVC_REC_BY_NAME;
	req.timeout_ms = p_osmt->opt.transaction_timeout;
	req.retry_cnt = p_osmt->opt.retry_count;
	req.flags = OSM_SA_FLAGS_SYNC;
	req.query_context = &context;
	req.pfn_query_cb = osmtest_query_res_cb;
	req.sm_key = 0;

	memset(service_name, 0, sizeof(service_name));
	memcpy(service_name, sr_name, (strlen(sr_name) + 1) * sizeof(char));
	req.p_query_input = service_name;

	if (p_osmt->opt.with_grh) {
		req.with_grh = 1;
		memcpy(&req.gid, &p_osmt->sm_port_gid, 16);
	}

	status = osmv_query_sa(p_osmt->h_bind, &req);
	if (status != IB_SUCCESS) {
		OSM_LOG(&p_osmt->log, OSM_LOG_ERROR, "ERR 4A0E: "
			"ib_query failed (%s)\n", ib_get_err_str(status));
		goto Exit;
	}

	status = context.result.status;
	num_recs = context.result.result_cnt;

	if (status != IB_SUCCESS) {
		char mad_stat_err[256];

		/*  If the failure is due to IB_SA_MAD_STATUS_NO_RECORDS and rec_num is 0,
		   then this is fine */
		if (status == IB_REMOTE_ERROR)
			strcpy(mad_stat_err,
			       ib_get_mad_status_str(osm_madw_get_mad_ptr
						     (context.result.
						      p_result_madw)));
		else
			strcpy(mad_stat_err, ib_get_err_str(status));

		if (status == IB_REMOTE_ERROR &&
		    !strcmp(mad_stat_err, "IB_SA_MAD_STATUS_NO_RECORDS") &&
		    rec_num == 0) {
			OSM_LOG(&p_osmt->log, OSM_LOG_ERROR,
				"IS EXPECTED ERROR ^^^^\n");
			status = IB_SUCCESS;
		} else {
			OSM_LOG(&p_osmt->log, OSM_LOG_ERROR, "ERR 4A0F: "
				"Query failed: %s (%s)\n",
				ib_get_err_str(status), mad_stat_err);
			goto Exit;
		}
	}

	if (rec_num && num_recs != rec_num) {
		OSM_LOG(&p_osmt->log, OSM_LOG_ERROR, "ERR 4A10: "
			"Unmatched number of records: expected: %d, received: %d\n",
			rec_num, num_recs);
		status = IB_REMOTE_ERROR;
		goto Exit;
	}

	p_rec = osmv_get_query_svc_rec(context.result.p_result_madw, 0);
	*p_out_rec = *p_rec;

	if (num_recs) {
		OSM_LOG(&p_osmt->log, OSM_LOG_VERBOSE,
			"Found service record: name: %s id: 0x%016" PRIx64 "\n",
			sr_name, cl_ntoh64(p_rec->service_id));

		osm_dump_service_record(&p_osmt->log, p_rec, OSM_LOG_DEBUG);
	}

Exit:
	OSM_LOG(&p_osmt->log, OSM_LOG_VERBOSE,
		"Expected and found %d records\n", rec_num);

	if (context.result.p_result_madw != NULL) {
		osm_mad_pool_put(&p_osmt->mad_pool,
				 context.result.p_result_madw);
		context.result.p_result_madw = NULL;
	}

	OSM_LOG_EXIT(&p_osmt->log);
	return status;
}