static int determinant(lash_t *p_lash, int deg, int rank, int ***m, int *p)
{
	int i, j, k;
	int *q;
	int sign = 1;

	/*
	 * handle simple case of 1x1 matrix
	 */
	if (rank == 1) {
		for (i = 0; i <= deg; i++)
			p[i] += m[0][0][i];
	}

	/*
	 * handle simple case of 2x2 matrix
	 */
	else if (rank == 2) {
		for (i = 0; i <= deg; i++) {
			if (m[0][0][i] == 0)
				continue;

			for (j = 0; j <= deg; j++) {
				if (m[1][1][j] == 0)
					continue;

				p[i+j] += m[0][0][i]*m[1][1][j];
			}
		}

		for (i = 0; i <= deg; i++) {
			if (m[0][1][i] == 0)
				continue;

			for (j = 0; j <= deg; j++) {
				if (m[1][0][j] == 0)
					continue;

				p[i+j] -= m[0][1][i]*m[1][0][j];
			}
		}
	}

	/*
	 * handle the general case
	 */
	else {
		for (i = 0; i < rank; i++) {
			if (sub_determinant(p_lash, deg, rank, 0, i, m, &q))
				return -1;

			for (j = 0; j <= deg; j++) {
				if (m[0][i][j] == 0)
					continue;

				for (k = 0; k <= deg; k++) {
					if (q[k] == 0)
						continue;

					p[j+k] += sign*m[0][i][j]*q[k];
				}
			}

			free(q);
			sign = -sign;
		}
	}

	return 0;
}