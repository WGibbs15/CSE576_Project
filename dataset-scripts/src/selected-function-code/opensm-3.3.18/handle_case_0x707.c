static
bool handle_case_0x707(struct torus *t, int i, int j, int k)
{
	int ip1 = canonicalize(i + 1, t->x_sz);
	int jp1 = canonicalize(j + 1, t->y_sz);
	int kp1 = canonicalize(k + 1, t->z_sz);

	if (install_tswitch(t, ip1, j, k,
			    tfind_face_corner(t->sw[ip1][jp1][k],
					      t->sw[ip1][jp1][kp1],
					      t->sw[ip1][j][kp1]))) {
		return true;
	}
	log_no_crnr(t, 0x707, i, j, k, ip1, j, k);

	if (install_tswitch(t, i, jp1, k,
			    tfind_face_corner(t->sw[ip1][jp1][k],
					      t->sw[ip1][jp1][kp1],
					      t->sw[i][jp1][kp1]))) {
		return true;
	}
	log_no_crnr(t, 0x707, i, j, k, i, jp1, k);
	return false;
}