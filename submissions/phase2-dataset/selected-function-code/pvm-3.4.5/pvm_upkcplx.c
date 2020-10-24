int
pvm_upkcplx(xp, cnt, std)
	float *xp;
	int cnt;
	int std;
{
	int cc;
	long ad;
	TEV_DECLS

	if (TEV_EXCLUSIVE) {
		if (TEV_DO_TRACE(TEV_UPKCPLX,TEV_EVENT_ENTRY)) {
			ad = (long)xp;
			TEV_PACK_LONG( TEV_DID_PDA, TEV_DATA_SCALAR, &ad, 1, 1 );
			TEV_PACK_INT( TEV_DID_PC, TEV_DATA_SCALAR, &cnt, 1, 1 );
			TEV_PACK_INT( TEV_DID_PSD, TEV_DATA_SCALAR, &std, 1, 1 );
			TEV_FIN;
		}
	}

	if (cnt < 0)
		cc = PvmBadParam;
	else if (!pvmrbuf)
		cc = PvmNoBuf;
	else
		cc = (pvmrbuf->m_codef->dec_cplx)
				(pvmrbuf, (void*)xp, cnt, std, sizeof(float)*2);

	if (TEV_AMEXCL) {
		if (TEV_DO_TRACE(TEV_UPKCPLX,TEV_EVENT_EXIT)) {
			TEV_PACK_INT( TEV_DID_CC, TEV_DATA_SCALAR, &cc, 1, 1 );
			TEV_FIN;
		}
		TEV_ENDEXCL;
	}
	return (cc < 0 ? lpvmerr("pvm_upkcplx", cc) : PvmOk);
}