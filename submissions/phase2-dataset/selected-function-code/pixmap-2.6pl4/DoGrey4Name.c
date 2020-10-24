void DoGrey4Name()
{
  char *g4_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);
  
  sprintf(message, "Color `%s' [%d] grey scale 4 display name:", 
	  color->c_name, current_color);
  
  if (PopupDialog(input_dialog, message, 
		  (color->g4_name ? color->g4_name : ""), 
		  &g4_name,
		  XtGrabExclusive) == Okay)
  {
      XColor xcolor;
      
      if (!XParseColor(dpy, cmap, g4_name, &xcolor))
	{
	  fprintf(stderr, "Warning: color %s could not be parsed!\n",
		  g4_name);
	}
      else
	PWUpdateColorInTable(pixmap_widget, 
			     current_color,
			     color->symbol,
			     color->s_name,
			     color->m_name,
			     g4_name,
			     color->g_name,
			     color->c_name);
  }
}