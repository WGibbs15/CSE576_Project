static
struct f_switch *alloc_fswitch(struct fabric *f,
			       guid_t sw_id, unsigned port_cnt)
{
	size_t new_sw_sz;
	unsigned cnt_max;
	struct f_switch *sw = NULL;
	void *ptr;

	if (f->switch_cnt >= f->switch_cnt_max) {

		cnt_max = 16 + 5 * f->switch_cnt_max / 4;
		ptr = realloc(f->sw, cnt_max * sizeof(*f->sw));
		if (!ptr) {
			OSM_LOG(&f->osm->log, OSM_LOG_ERROR,
				"ERR 4E02: realloc: %s\n", strerror(errno));
			goto out;
		}
		f->sw = ptr;
		f->switch_cnt_max = cnt_max;
		memset(&f->sw[f->switch_cnt], 0,
		       (f->switch_cnt_max - f->switch_cnt)*sizeof(*f->sw));
	}
	new_sw_sz = sizeof(*sw) + port_cnt * sizeof(*sw->port);
	sw = calloc(1, new_sw_sz);
	if (!sw) {
		OSM_LOG(&f->osm->log, OSM_LOG_ERROR,
			"ERR 4E03: calloc: %s\n", strerror(errno));
		goto out;
	}
	sw->port = (void *)(sw + 1);
	sw->n_id = sw_id;
	sw->port_cnt = port_cnt;
	f->sw[f->switch_cnt++] = sw;
out:
	return sw;
}