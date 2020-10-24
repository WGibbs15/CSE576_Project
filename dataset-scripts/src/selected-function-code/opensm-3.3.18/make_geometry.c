static void make_geometry(lash_t *p_lash, int sw)
{
	osm_log_t *p_log = &p_lash->p_osm->log;
	int num_switches = p_lash->num_switches;
	int sw1, sw2;
	switch_t *s, *s1, *s2, *seed;
	unsigned int i, j, k, l, n, m;
	unsigned int change;

	OSM_LOG_ENTER(p_log);

	s = p_lash->switches[sw];

	if (!s->node->matrix)
		goto done;

	/*
	 * assign axes to seed switch
	 */
	seed_axes(p_lash, sw);
	seed = p_lash->switches[sw];

	/*
	 * induce axes in other switches until
	 * there is no more change
	 */
	do {
		change = 0;

		/* phase 1 opposites */
		for (sw1 = 0; sw1 < num_switches; sw1++) {
			s1 = p_lash->switches[sw1];
			n = s1->node->num_links;

			/*
			 * ignore chain fragments
			 */
			if (n < seed->node->num_links && n <= 2)
				continue;

			/*
			 * only process 'mesh' switches
			 */
			if (!s1->node->matrix)
				continue;

			for (i = 0; i < n; i++) {
				if (!s1->node->axes[i])
					continue;

				/*
				 * can't tell across if more than one
				 * likely looking link
				 */
				m = 0;
				for (j = 0; j < n; j++) {
					if (j == i)
						continue;

					if (s1->node->matrix[i][j] != 2)
						m++;
				}

				if (m != 1) {
					continue;
				}

				for (j = 0; j < n; j++) {
					if (j == i)
						continue;

					/* Rule out opposite nodes when distance greater than 4 */
					if (s1->node->matrix[i][j] != 2 &&
					    s1->node->matrix[i][j] <= 4) {
						if (s1->node->axes[j]) {
							if (s1->node->axes[j] != opposite(seed, s1->node->axes[i])) {
								OSM_LOG(p_log, OSM_LOG_DEBUG,
									"phase 1 mismatch\n");
							}
						} else {
							s1->node->axes[j] = opposite(seed, s1->node->axes[i]);
							change++;
						}
					}
				}
			}
		}

		/* phase 2 switch to switch */
		for (sw1 = 0; sw1 < num_switches; sw1++) {
			s1 = p_lash->switches[sw1];
			n = s1->node->num_links;

			if (!s1->node->matrix)
				continue;

			for (i = 0; i < n; i++) {
				int l2 = s1->node->links[i]->link_id;

				if (!s1->node->axes[i])
					continue;

				if (l2 == -1) {
					OSM_LOG(p_log, OSM_LOG_DEBUG,
						"no reverse link\n");
					continue;
				}

				sw2 = s1->node->links[i]->switch_id;
				s2 = p_lash->switches[sw2];

				if (!s2->node->matrix)
					continue;

				if (!s2->node->axes[l2]) {
					/*
					 * set axis to opposite of s1->axes[i]
					 */
					s2->node->axes[l2] = opposite(seed, s1->node->axes[i]);
					change++;
				} else {
					if (s2->node->axes[l2] != opposite(seed, s1->node->axes[i])) {
						OSM_LOG(p_log, OSM_LOG_DEBUG,
							"phase 2 mismatch\n");
					}
				}
			}
		}

		/* Phase 3 corners */
		for (sw1 = 0; sw1 < num_switches; sw1++) {
			s = p_lash->switches[sw1];
			n = s->node->num_links;

			if (!s->node->matrix)
				continue;

			for (i = 0; i < n; i++) {
				if (!s->node->axes[i])
					continue;

				for (j = 0; j < n; j++) {
					if (i == j || !s->node->axes[j] || s->node->matrix[i][j] != 2)
						continue;

					s1 = p_lash->switches[s->node->links[i]->switch_id];
					s2 = p_lash->switches[s->node->links[j]->switch_id];

					/*
					 * find switch (other than s1) that neighbors i and j
					 * have in common
					 */
					for (k = 0; k < s1->node->num_links; k++) {
						if (s1->node->links[k]->switch_id == sw1)
							continue;

						for (l = 0; l < s2->node->num_links; l++) {
							if (s2->node->links[l]->switch_id == sw1)
								continue;

							if (s1->node->links[k]->switch_id == s2->node->links[l]->switch_id) {
								if (s1->node->axes[k]) {
									if (s1->node->axes[k] != s->node->axes[j]) {
										OSM_LOG(p_log, OSM_LOG_DEBUG, "phase 3 mismatch\n");
									}
								} else {
									s1->node->axes[k] = s->node->axes[j];
									change++;
								}

								if (s2->node->axes[l]) {
									if (s2->node->axes[l] != s->node->axes[i]) {
										OSM_LOG(p_log, OSM_LOG_DEBUG, "phase 3 mismatch\n");
									}
								} else {
									s2->node->axes[l] = s->node->axes[i];
									change++;
								}
								goto next_j;
							}
						}
					}
next_j:
					;
				}
			}
		}
	} while (change);

done:
	OSM_LOG_EXIT(p_log);
}