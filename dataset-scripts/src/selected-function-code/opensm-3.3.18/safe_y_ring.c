static
bool safe_y_ring(struct torus *t, int i, int j, int k)
{
	int jm1, jp1, jp2;
	bool success = true;

	/*
	 * If this y-direction radix-4 ring has at least two links
	 * already installed into the torus,  then this ring does not
	 * prevent us from looking for x or z direction perpendiculars.
	 *
	 * It is easier to check for the appropriate switches being installed
	 * into the torus than it is to check for the links, so force the
	 * link installation if the appropriate switches are installed.
	 *
	 * Recall that canonicalize(n - 2, 4) == canonicalize(n + 2, 4).
	 */
	if (t->y_sz != 4 || (t->flags & Y_MESH))
		goto out;

	jm1 = canonicalize(j - 1, t->y_sz);
	jp1 = canonicalize(j + 1, t->y_sz);
	jp2 = canonicalize(j + 2, t->y_sz);

	if (!!t->sw[i][jm1][k] +
	    !!t->sw[i][jp1][k] + !!t->sw[i][jp2][k] < 2) {
		success = false;
		goto out;
	}
	if (t->sw[i][jp2][k] && t->sw[i][jm1][k])
		success = link_tswitches(t, 1,
					 t->sw[i][jp2][k],
					 t->sw[i][jm1][k])
			&& success;

	if (t->sw[i][jm1][k] && t->sw[i][j][k])
		success = link_tswitches(t, 1,
					 t->sw[i][jm1][k],
					 t->sw[i][j][k])
			&& success;

	if (t->sw[i][j][k] && t->sw[i][jp1][k])
		success = link_tswitches(t, 1,
					 t->sw[i][j][k],
					 t->sw[i][jp1][k])
			&& success;

	if (t->sw[i][jp1][k] && t->sw[i][jp2][k])
		success = link_tswitches(t, 1,
					 t->sw[i][jp1][k],
					 t->sw[i][jp2][k])
			&& success;
out:
	return success;
}