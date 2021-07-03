void xrdb_read()
{
    Display *display;
    char * resm;
    XrmDatabase xrdb;
    char *type;
    XrmValue value;

    display = XOpenDisplay(NULL);
    if (display != NULL) {
        resm = XResourceManagerString(display);
        if (resm != NULL) {
            xrdb = XrmGetStringDatabase(resm);
            if (xrdb != NULL) {
                xrdb_colors(xrdb);
            }
        }
    }
    XCloseDisplay(display);
}
