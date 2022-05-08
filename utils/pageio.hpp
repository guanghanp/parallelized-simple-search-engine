#include <stdio.h>

extern "C"
{
    #include <webpage.h>
}
int32_t pagesave(webpage_t *pagep, int id, char *dirnm);

/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm);