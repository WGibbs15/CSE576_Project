static void
configure_button_layout (guchar   *buttons,
                         gint      n_buttons,
                         gboolean  left_handed)
{
        const gint left_button = 1;
        gint right_button;
        gint i;

        /* if the button is higher than 2 (3rd button) then it's
         * probably one direction of a scroll wheel or something else
         * uninteresting
         */
        right_button = MIN (n_buttons, 3);

        /* If we change things we need to make sure we only swap buttons.
         * If we end up with multiple physical buttons assigned to the same
         * logical button the server will complain. This code assumes physical
         * button 0 is the physical left mouse button, and that the physical
         * button other than 0 currently assigned left_button or right_button
         * is the physical right mouse button.
         */

        /* check if the current mapping satisfies the above assumptions */
        if (buttons[left_button - 1] != left_button &&
            buttons[left_button - 1] != right_button)
                /* The current mapping is weird. Swapping buttons is probably not a
                 * good idea.
                 */
                return;

        /* check if we are left_handed and currently not swapped */
        if (left_handed && buttons[left_button - 1] == left_button) {
                /* find the right button */
                for (i = 0; i < n_buttons; i++) {
                        if (buttons[i] == right_button) {
                                buttons[i] = left_button;
                                break;
                        }
                }
                /* swap the buttons */
                buttons[left_button - 1] = right_button;
        }
        /* check if we are not left_handed but are swapped */
        else if (!left_handed && buttons[left_button - 1] == right_button) {
                /* find the right button */
                for (i = 0; i < n_buttons; i++) {
                        if (buttons[i] == left_button) {
                                buttons[i] = right_button;
                                break;
                        }
                }
                /* swap the buttons */
                buttons[left_button - 1] = left_button;
        }
}