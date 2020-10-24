static int strhash(string)
register const char *string;
{
	register int c;

#ifdef HASH_ELFHASH
	register unsigned int h = 0, g;

	while ((c = *string++) != '\0') {
		h = (h << 4) + c;
		if (g = h & 0xF0000000)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
#elif HASH_PERL
	register int val = 0;

	while ((c = *string++) != '\0') {
		val = val * 33 + c;
	}

	return val + (val >> 5);
#else
	register int val = 0;

	while ((c = *string++) != '\0') {
		val = val * 997 + c;
	}

	return val + (val >> 5);
#endif
}