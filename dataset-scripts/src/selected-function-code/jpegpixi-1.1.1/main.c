int
main (int argc, char *argv [])
{
    int i;
    const char *infilename, *outfilename;
    struct jpeg_error_mgr errmgr;
    struct jpeg_decompress_struct injpg;
    FILE *infile, *outfile;
    struct rbtree points;
    unsigned int threshold = DENOM / 10;



    /* Store program invocation name.  */
    invocation_name = argv [0] != 0 && *(argv [0]) != '\0' ? argv [0] : PROGRAM_NAME;


    /* Set up Native Language Support.  */
#if HAVE_NLS
    if (setlocale (LC_ALL, "") != 0)
    {
        if (bindtextdomain (PACKAGE_TARNAME, LOCALEDIR) == 0 || textdomain (PACKAGE_TARNAME) == 0)
            mem_alloc_failed ();
    }
    else
    {
        fprintf (stderr, "%s: cannot set locale\n"
                 "Make sure the `LC_*' and `LANG' environment variables are set to valid values.\n", invocation_name);
        exit (1);
    }
#endif


   /* Parse command line options.  */
    if ((i = parse_options (invocation_name, argc, argv)) == -1)
        goto error_try_help;

    if (opt_help)
    {
        display_help_text ();
        exit (0);
    }

    if (opt_version)
    {
        display_version_text ();
        exit (0);
    }

    if (opt_threshold)
    {
        const char *p = arg_threshold;

        while (isspace ((unsigned char)*p))
            ++p;

        if (parse_number (&p, &threshold) != 0)
        {
          error_threshold:
            fprintf (stderr, _("%s: invalid threshold `%s'\n"), invocation_name, arg_threshold);
            goto error_try_help;
        }

        while (isspace ((unsigned char)*p))
            ++p;
        if (*p != '\0')
            goto error_threshold;
    }


    /* Parse non-option command line arguments.  */
    if (i < argc)
    {
        infilename = argv [i++];
        if (strcmp (infilename, "-") == 0)
            infilename = 0;
    }
    else
    {
        fprintf (stderr, _("%s: source filename missing\n"), invocation_name);
      error_try_help:
        fprintf (stderr, _("Try `%s --help' for more information.\n"), invocation_name);
        exit (1);
    }

    if (i < argc)
    {
        outfilename = argv [i++];

        if (i < argc)
        {
            fprintf (stderr, _("%s: unexpected argument `%s'\n"), invocation_name, argv [i]);
            goto error_try_help;
        }

        if (strcmp (outfilename, "-") == 0)
            outfilename = 0;
    }
    else
        outfilename = 0;


    /* Initialize jpeglib objects.  */
    infilename1 = infilename;

    jpeg_std_error (&errmgr);
    errmgr.output_message = &display_error_message;

    jpeg_create_decompress (&injpg);
    injpg.err = &errmgr;


    /* Open input file, read header, and start decompression.  */
    if (infilename != 0)
    {
        if ((infile = fopen (infilename, "rb")) == 0)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, infilename, strerror (errno));
            exit (1);
        }
    }
    else
        infile = stdin;

    jpeg_stdio_src (&injpg, infile);
    jpeg_read_header (&injpg, TRUE);
    jpeg_start_decompress (&injpg);


    /* Find hot pixels and store them in a tree.  */
    {
        unsigned int y;
        JSAMPLE *samples = xmalloc (injpg.output_width * injpg.output_components * sizeof *samples);
        const int threshold_value = REL_TO_ABS (threshold, MAXJSAMPLE);

        rbtree_create (&points);

        for (y = 0; injpg.output_scanline < injpg.output_height; ++y)
        {
            unsigned int x;
            JSAMPLE *s;

            jpeg_read_scanlines (&injpg, &samples, 1);

            for (x = 0, s = samples; x < (unsigned int)injpg.output_width; ++x)
            {
                size_t icomp;
                int value;

                /* Find maximum component value.  Invert sample values if the `--invert' command line option
                   has been specified.  */
                for (icomp = 0, value = 0; icomp < (size_t)injpg.output_components; ++icomp, ++s)
                {
                    const int sample_value = opt_invert ? MAXJSAMPLE - GETJSAMPLE (*s) : GETJSAMPLE (*s);

                    if (sample_value > value)
                        value = sample_value;
                }

                if (value > threshold_value)
                {
                    struct point_lumi_s point;

                    point.p.x = x;
                    point.p.y = y;
                    point.p.x_size = 1;
                    point.p.y_size = 1;
                    point.lumi = ((2 * DENOM) / MAXJSAMPLE) * value / 2;

                    if (rbtree_insert (&points, &point, sizeof point, point_s_cmp, 0) == 0)
                        mem_alloc_failed ();
                }
            }
        }

        free (samples);
        consolidate_pixels (&points, point_lumi_s_validate_and_consolidate);
    }


    /* Finish processing, close input file, and delete jpeglib object.  */
    jpeg_finish_decompress (&injpg);

    if (infilename != 0 && fclose (infile) == EOF)
    {
        fprintf (stderr, _("%s: %s: %s\n"), invocation_name, infilename, strerror (errno));
        exit (1);
    }

    jpeg_destroy_decompress (&injpg);



    /* Write output file.  */
    if (outfilename != 0)
    {
        if ((outfile = fopen (outfilename, "w")) == 0)
        {
            fprintf (stderr, _("%s: %s: %s\n"), invocation_name, outfilename, strerror (errno));
            exit (1);
        }
    }
    else
        outfile = stdout;

    {
        const struct point_lumi_s *point = rbtree_first (&points);

        while (point != 0)
        {
            fprintf (outfile, "%u,%u,%u,%u", point->p.x, point->p.y, point->p.x_size, point->p.y_size);
            if (opt_comments)
            {
                const unsigned int lumi_per_10000 = (point->lumi / DENOM_SQRT) * 10000 / DENOM_SQRT;
                fprintf (outfile, _("\t# luminosity: %u.%02u%%"), lumi_per_10000 / 100, lumi_per_10000 % 100);
            }
            putc ('\n', outfile);
            if (ferror (outfile))
            {
                fprintf (stderr, _("%s: %s: %s\n"), invocation_name, outfilename != 0 ? outfilename : _("STDOUT"),
                         strerror (errno));
                exit (1);
            }

            point = rbtree_next (point);
        }
    }

    if (outfilename != 0 && fclose (outfile) == EOF)
    {
        fprintf (stderr, _("%s: %s: %s\n"), invocation_name, outfilename, strerror (errno));
        exit (1);
    }


    exit (0);
}