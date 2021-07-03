#include <X11/Xresource.h>

#define XCOLORS \
    void xrdb_colors(XrmDatabase xrdb) { \
        char *type; \
        XrmValue value;

#define XCOLORS_END }

#define XLOAD(V,R) \
    if (XrmGetResource(xrdb, R, NULL, &type, &value) == True) { \
        if (value.addr != NULL && strnlen(value.addr, 8) == 7 && value.addr[0] == '#') { \
            int i = 1; \
            for (; i <= 6; i++) { \
                if (value.addr[i] < 48) break; \
                if (value.addr[i] > 57 && value.addr[i] < 65) break; \
                if (value.addr[i] > 70 && value.addr[i] < 97) break; \
                if (value.addr[i] > 102) break; \
            } \
            if (i == 7) { \
                strncpy(V, value.addr, 7); \
                V[7] = '\0'; \
            } \
        } \
    }
