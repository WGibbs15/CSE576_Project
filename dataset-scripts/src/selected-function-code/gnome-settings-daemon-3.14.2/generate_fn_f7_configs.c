static void
generate_fn_f7_configs (GsdXrandrManager *mgr)
{
        GPtrArray *array = g_ptr_array_new ();
        GnomeRRScreen *screen = mgr->priv->rw_screen;

        g_debug ("Generating configurations");

        /* Free any existing list of configurations */
        if (mgr->priv->fn_f7_configs) {
                int i;

                for (i = 0; mgr->priv->fn_f7_configs[i] != NULL; ++i)
                        g_object_unref (mgr->priv->fn_f7_configs[i]);
                g_free (mgr->priv->fn_f7_configs);

                mgr->priv->fn_f7_configs = NULL;
                mgr->priv->current_fn_f7_config = -1;
        }

        g_ptr_array_add (array, gnome_rr_config_new_current (screen, NULL));
        g_ptr_array_add (array, make_clone_setup (mgr, screen));
        g_ptr_array_add (array, make_xinerama_setup (mgr, screen));
        g_ptr_array_add (array, make_other_setup (screen));
        g_ptr_array_add (array, make_laptop_setup (mgr, screen));

        array = sanitize (mgr, array);

        if (array) {
                mgr->priv->fn_f7_configs = (GnomeRRConfig **)g_ptr_array_free (array, FALSE);
                mgr->priv->current_fn_f7_config = 0;
        }
}