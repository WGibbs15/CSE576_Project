int save_gen_s(GtkButton *button, FILE *file_p) {

	GtkWidget *bt1, *bt2, *bt5, *bt6, *bt7;
	gchar *bt1_t, *bt2_t;
	long count = 0, del = 0;
        int i = 0, timeflag = 0;
        unsigned char pkttable[10][71];
        long int partable[10][5];
	char buff4[101];

	bt1 = lookup_widget(GTK_WIDGET (button), "radiobutton36");
	bt2 = lookup_widget(GTK_WIDGET (button), "radiobutton37");
	bt5 = lookup_widget(GTK_WIDGET (button), "checkbutton36");
	bt6 = lookup_widget(GTK_WIDGET (button), "entry151");
	bt7 = lookup_widget(GTK_WIDGET (button), "entry152");

	bt1_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt6));
	bt2_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt7));

	if (GTK_TOGGLE_BUTTON(bt1)->active)
		timeflag = 1;
	else
		timeflag = 0;

	if (GTK_TOGGLE_BUTTON(bt5)->active) {
		count = -3;
	}
	else {
		/* there can be rubbish in this field */
		if (check_digit(bt1_t, strlen(bt1_t),
					  "Error: Number of sequences to send field") == -1)
			return -1;

		count = strtol(bt1_t, (char **)NULL, 10);
		/* we allow to send 9999999 max */
		if ( (count > 9999999) || (count < 1) ) {
			//printf("Error: Number of sequences to send field\n");
			error("Error: Number of sequences to send field (1 - 9999999)");
			return -1;
		}
	}

	
	if (count == 1)
		del = 0;
	else {
		/* there can be rubbish in this field */
		if (check_digit(bt2_t, strlen(bt2_t), "Error: Delay between sequences field") == -1)
			return -1;

		del = strtol(bt2_t, (char **)NULL, 10);
		/* max delay 999,999999 s */
		if ( (del > 999999999) || (del < 0) ) {
			//printf("Error: Delay between sequences field\n");
			error("Error: Delay between sequences field (0 - 999999999)");
			return -1;
		}
	}

	for (i=0; i<10; i++) {

		/* name of the packet and packet contents */
		snprintf(buff4, 100, "entry%d", 111+i);
		bt1 = lookup_widget(GTK_WIDGET (button), buff4);
		bt1_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt1));

		/* if there is no name, skip it */
		if ( strlen(bt1_t) == 0 )  {
			partable[i][0] = 0;
			continue;
		}
		else
			partable[i][0] = 1;
		/* copy the name in the table */
		strncpy(&pkttable[i][0], bt1_t, 70);

		/* number of packets to send */
		snprintf(buff4, 100, "entry%d", 121+i);
		bt2 = lookup_widget(GTK_WIDGET (button), buff4);
		bt2_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt2));
		snprintf(buff4, 100, "Error: Number of packets field in row %d", i+1);
		if (check_digit(bt2_t,strlen(bt2_t), buff4) == -1)
			return -1;

		partable[i][1] = strtol(bt2_t, (char **)NULL, 10);
		/* we allow to send 9999999 max */
		if ( (partable[i][1] > 9999999) || (partable[i][1] < 0) ) {
			snprintf(buff4, 100, "Error: number of packets value in row %d", i+1);
			//printf("Error: number of packets value in row %d\n", i+1);
			error(buff4);
			return -1;
		}

		/* delay between packets */
		snprintf(buff4, 100, "entry%d", 131+i);
		bt2 = lookup_widget(GTK_WIDGET (button), buff4);
		bt2_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt2));
		snprintf(buff4, 100, "Error: Delay between packets field in row %d", i+1);
		if (check_digit(bt2_t,strlen(bt2_t), buff4) == -1)
			return -1;
		
		 partable[i][2] = strtol(bt2_t, (char **)NULL, 10);
		/* max delay 999,999999 s */
		if ( (partable[i][2] > 999999999) || (partable[i][2] < 0) ) {
			snprintf(buff4, 100, "Error: delay between value in row %d", i+1);
			//printf("Error: delay between value in row %d\n", i+1);
			error(buff4);
			return -1;
		}

		/* delay to next sequence */
		snprintf(buff4, 100, "entry%d", 141+i);
		bt2 = lookup_widget(GTK_WIDGET (button), buff4);
		bt2_t = (char *) gtk_entry_get_text(GTK_ENTRY(bt2));
		snprintf(buff4, 100, "Error: Delay to next value in row %d", i+1);
		if (check_digit(bt2_t,strlen(bt2_t), buff4) == -1)
			return -1;

		partable[i][3] = strtol(bt2_t, (char **)NULL, 10);
		/* max delay 999,999999 s */
		if ( (partable[i][3] > 999999999) || (partable[i][3] < 0) ) {
			snprintf(buff4, 100, "Error: delay to next value in row %d", i+1);
			//printf("Error: delay to next value in row %d\n", i+1);
			error(buff4);
			return -1;
		}

		  /* enable or disable */
		snprintf(buff4, 100, "checkbutton%d", 25+i);
		bt1 = lookup_widget(GTK_WIDGET(button), buff4);
		if (GTK_TOGGLE_BUTTON(bt1)->active)
			partable[i][4] = 0;
		else
			partable[i][4] = 1;
	}

	fprintf(file_p, "#configuration parameters for send sequence generator\n");
	fprintf(file_p, "#absolute/relative timing (1/0), number (-3=infinite), delay (1=max)\n");
	fprintf(file_p, "%d,%ld,%ld\n", timeflag, count,del);
		fprintf(file_p, "#parameters for each sequence should have:\n#name, number of packets, delay between packets, delay to next sequence, enable/disable\n");
	for (i=0; i<10; i++) {
		fprintf(file_p, "#parameters for sequence number %d\n", i);
		if (partable[i][0] == 0) {
			fprintf(file_p, "0\n");
		}
		else
			fprintf(file_p, "%s,%ld,%ld,%ld,%ld\n", &pkttable[i][0], partable[i][1],
						partable[i][2], partable[i][3],partable[i][4]);
                }

	return 1;
}