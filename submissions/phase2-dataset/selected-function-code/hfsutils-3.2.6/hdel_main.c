int hdel_main(int argc, char *argv[])
{
  hfsvol *vol;
  int fargc;
  char **fargv;
  int i, result = 0;

  if (argc < 2)
    {
      fprintf(stderr, "Usage: %s hfs-path [...]\n", argv0);
      return 1;
    }

  vol = hfsutil_remount(hcwd_getvol(-1), HFS_MODE_ANY);
  if (vol == 0)
    return 1;

  fargv = hfsutil_glob(vol, argc - 1, &argv[1], &fargc, &result);

  if (result == 0)
    {
      for (i = 0; i < fargc; ++i)
	{
	  if (hfs_delete(vol, fargv[i]) == -1)
	    {
	      hfsutil_perrorp(fargv[i]);
	      result = 1;
	    }
	}
    }

  hfsutil_unmount(vol, &result);

  if (fargv)
    free(fargv);

  return result;
}