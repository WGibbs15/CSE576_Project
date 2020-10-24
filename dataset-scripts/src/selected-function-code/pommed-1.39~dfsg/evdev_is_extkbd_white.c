static int
evdev_is_extkbd_white(unsigned short *id)
{
  unsigned short product = id[ID_PRODUCT];

  if (id[ID_BUS] != BUS_USB)
    return 0;

  if (id[ID_VENDOR] != USB_VENDOR_ID_APPLE)
    return 0;

  if (product == USB_PRODUCT_ID_APPLE_EXTKBD_WHITE)
    {
      logdebug(" -> External Apple USB keyboard (white)\n");

      kbd_set_fnmode();

      return 1;
    }

  return 0;
}