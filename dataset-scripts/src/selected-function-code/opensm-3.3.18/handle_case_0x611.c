static
bool handle_case_0x611(struct torus *t, int i, int j, int k)
{
	int jp1 = canonicalize(j + 1, t->y_sz);
	int jp2 = canonicalize(j + 2, t->y_sz);
	int kp1 = canonicalize(k + 1, t->z_sz);

	if (safe_y_perpendicular(t, i, jp1, k) &&
	    install_tswitch(t, i, j, k,
			    tfind_2d_perpendicular(t->sw[i][jp1][kp1],
						   t->sw[i][jp1][k],
						   t->sw[i][jp2][k]))) {
		return true;
	}
	log_no_perp(t, 0x611, i, j, k, i, jp1, k);

	if (safe_y_perpendicular(t, i, jp1, kp1) &&
	    install_tswitch(t, i, j, kp1,
			    tfind_2d_perpendicular(t->sw[i][jp1][k],
						   t->sw[i][jp1][kp1],
						   t->sw[i][jp2][kp1]))) {
		return true;
	}
	log_no_perp(t, 0x611, i, j, k, i, jp1, kp1);
	return false;
}