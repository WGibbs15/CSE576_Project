static int
evdev_is_wellspring3(unsigned short *id)
{
  unsigned short product = id[ID_PRODUCT];

  if (id[ID_BUS] != BUS_USB)
    return 0;

  if (id[ID_VENDOR] != USB_VENDOR_ID_APPLE)
    return 0;

  if ((product == USB_PRODUCT_ID_WELLSPRING3_ANSI)
      || (product == USB_PRODUCT_ID_WELLSPRING3_ISO)
      || (product == USB_PRODUCT_ID_WELLSPRING3_JIS))
    {
      logdebug(" -> WellSpring III USB assembly\n");

      kbd_set_fnmode();

      return 1;
    }

  return 0;
}