#ifndef MINIWINDOW_H_
#define MINIWINDOW_H_

#include <stdint.h>

struct mw_window;

typedef struct mw_window *mw_window_t;

mw_window_t mw_create_window(const char *title, uint32_t width, uint32_t height);
void mw_destroy_window(mw_window_t window);

#endif // MINIWINDOW_H_
