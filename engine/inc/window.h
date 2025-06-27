#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>


typedef struct {
    int32_t w, h;
    GLFWwindow* handle;
} window_t;

window_t create_window(int32_t w, int32_t h, const char* title, bool resizable);

bool window_should_close(window_t* win);
void window_poll_events(void);
void window_handle_resize(window_t* win);
void window_clear(void);
void window_swap_buffers(window_t* win);
double window_get_time(void);
void window_show_cursor(window_t* win, bool show);

void destroy_window(window_t* win);