#define MINIWINDOW_WAYLAND
#include <miniwindow.h>

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	mw_window_t window = mw_create_window("Hello Miniwindow", 1920 / 2, 1080 / 2);

	mw_destroy_window(window);
	return 0;
}
