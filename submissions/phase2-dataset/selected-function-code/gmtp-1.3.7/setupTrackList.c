void setupTrackList(GtkTreeView *treeviewFiles) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Artist
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Artist"), renderer,
            "text", COL_ARTIST,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewFiles), column);
    gtk_tree_view_column_set_sort_column_id(column, COL_ARTIST);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_spacing(column, 5);

    // Album column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Album"), renderer,
            "text", COL_ALBUM,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewFiles), column);
    gtk_tree_view_column_set_sort_column_id(column, COL_ALBUM);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_spacing(column, 5);

    // Folder/FileID column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Object ID", renderer,
            "text", COL_TRACKID,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewFiles), column);
    gtk_tree_view_column_set_visible(column, FALSE);

    // Track column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Track"), renderer,
            "text", COL_TRACKNAME,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewFiles), column);
    gtk_tree_view_column_set_sort_column_id(column, COL_TRACKNAME);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_visible(column, TRUE);

    // Track Duration
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Duration"), renderer,
            "text", COL_TRACKDURATION,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewFiles), column);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_visible(column, TRUE);


}