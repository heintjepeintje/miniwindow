#include "./miniwindow.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MW_TRACE(...) fprintf(stdout, "[TRACE]: "__VA_ARGS__)
#define MW_INFO(...) fprintf(stdout, "[INFO]: "__VA_ARGS__)
#define MW_ERROR(...) fprintf(stderr, "[ERROR]: "__VA_ARGS__)

#ifdef __linux__

#ifdef MINIWINDOW_WAYLAND
#	include <wayland-client.h>
#	include <wayland-client-protocol.h>
#	include <xdg-shell-client-protocol.h>
#else
#	error "Miniwindow does not yet support X11"
#endif

struct mw_window {
	struct wl_display* display;
	struct wl_registry* registry;
	struct wl_compositor* compositor;
	struct wl_shm* shm;
	struct wl_subcompositor* subcompositor;

	struct wl_surface* surface;

	struct xdg_wm_base* xdg_wm_base;
	struct xdg_surface* xdg_surface;
	struct xdg_toplevel *xdg_toplevel;
};

void on_registry_global(void *data, struct wl_registry* registry, uint name, const char* interface, uint version) {
	struct mw_window* window = ((struct mw_window*)data);
	if (registry != window->registry) return;

	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		window->compositor = wl_registry_bind(window->registry, name, &wl_compositor_interface, version);
		MW_TRACE("(%p)->compositor = %p (v%u)\n", window, window->compositor, version);
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		window->shm = wl_registry_bind(window->registry, name, &wl_shm_interface, version);
		MW_TRACE("(%p)->shm = %p (v%u)\n", window, window->shm, version);
	} else if (strcmp(interface, wl_subcompositor_interface.name) == 0) {
		window->subcompositor = wl_registry_bind(window->registry, name, &wl_subcompositor_interface, version);
		MW_TRACE("(%p)->subcompositor = %p (v%u)\n", window, window->subcompositor, version);
	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		window->xdg_wm_base = wl_registry_bind(window->registry, name, &xdg_wm_base_interface, version);
		MW_TRACE("(%p)->xdg_wm_base = %p (v%u)\n", window, window->xdg_wm_base, version);
	}
}

void on_xdg_surface_configure(void *data, struct xdg_surface* xdg_surface, uint32_t serial) {
	struct mw_window* window = ((struct mw_window*)data);
	if (xdg_surface != window->xdg_surface) return;

	xdg_surface_ack_configure(xdg_surface, serial);
}

struct wl_registry_listener registry_listener = {
	.global = on_registry_global,
	.global_remove = NULL
};

struct xdg_surface_listener xdg_surface_listener = {
	.configure = on_xdg_surface_configure
};

mw_window_t mw_create_window(const char *title, uint32_t width, uint32_t height) {
	struct mw_window *window = malloc(sizeof(struct mw_window));

	((void)title);
	((void)width);
	((void)height);

	window->display = wl_display_connect(NULL);
	if (window->display == NULL) {
		MW_ERROR("Failed to connect to wayland compositor.\n");
		free(window);
		return NULL;
	}
	MW_TRACE("(%p)->display = %p\n", window, window->display);

	window->registry = wl_display_get_registry(window->display);
	MW_TRACE("(%p)->registry = %p\n", window, window->registry);

	if (wl_registry_add_listener(window->registry, &registry_listener, window) == -1) {
		MW_ERROR("Failed to add registry listener.\n");
		free(window);
		return NULL;
	}

	if (wl_display_roundtrip(window->display) == -1) {
		MW_ERROR("Failed to do display roundtrip.\n");
		free(window);
		return NULL;
	}

	window->surface = wl_compositor_create_surface(window->compositor);
	MW_TRACE("(%p)->surface = %p\n", window, window->surface);

	window->xdg_surface = xdg_wm_base_get_xdg_surface(window->xdg_wm_base, window->surface);
	if (window->xdg_surface == NULL) {
		MW_ERROR("Failed to create xdg_surface.\n");
		free(window);
		return NULL;
	}

	if (xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window) == -1) {
		MW_ERROR("Failed to attach listener to xdg_surface.\n");
		free(window);
		return NULL;
	}

	window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
	if (window->xdg_toplevel == NULL) {
		MW_ERROR("Failed to obtain xdg_toplevel.\n");
		free(window);
		return NULL;
	}

	xdg_toplevel_set_title(window->xdg_toplevel, title);

	return ((mw_window_t)window);
}

void mw_destroy_window(mw_window_t window) {
	if (window == NULL) return;

	wl_surface_destroy(window->surface);
	wl_display_disconnect(window->display);
		
	free(window);
}

#endif
