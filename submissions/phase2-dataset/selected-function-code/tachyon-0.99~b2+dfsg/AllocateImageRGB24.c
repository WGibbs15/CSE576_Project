rawimage * AllocateImageRGB24(const char * filename, int xs, int ys, int zs, unsigned char * rgb) {
  rawimage * newimage = NULL;
  int i, len, intable;

  intable=0;
  if (numimages!=0) {
    for (i=0; i<numimages; i++) {
      if (!strcmp(filename, imagelist[i]->name)) {
        newimage=imagelist[i];
        intable=1;
      }
    }
  }

  if (!intable) {
    newimage=malloc(sizeof(rawimage));
    newimage->loaded=1;
    newimage->xres=xs;
    newimage->yres=ys;
    newimage->zres=zs;
    newimage->bpp=3;
    newimage->data=rgb;
    len=strlen(filename);
    if (len > 80) 
      return NULL;
    strcpy(newimage->name, filename);

    imagelist[numimages]=newimage;  /* add new one to the table       */ 
    numimages++;                    /* increment the number of images */
  }
 
  return newimage;
}