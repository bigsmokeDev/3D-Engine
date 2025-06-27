#include <assert.h>

#include <window.h>
#include <renderer.h>


window_t create_window(int32_t w, int32_t h, const char* title, bool resizable) {
    window_t win = {
        .w = w, .h = h
    };

    assert(glfwInit());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GL_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_SAMPLES, 4);

    win.handle = glfwCreateWindow(w, h, title, NULL, NULL);
    assert(win.handle != NULL);

    return win;
}

bool window_should_close(window_t* win) {
    return glfwWindowShouldClose(win->handle) == 1;
}

void window_poll_events(void) {
    glfwPollEvents();
}

void window_handle_resize(window_t* win) {
    glfwGetFramebufferSize(win->handle, &win->w, &win->h);
    glViewport(0, 0, win->w, win->h);
}

void window_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(.25f, .25f, .25f, 1.f);
}

void window_swap_buffers(window_t* win) {
    glfwSwapBuffers(win->handle);
}

double window_get_time(void) {
    return glfwGetTime();
}

void window_show_cursor(window_t* win, bool show) {
    if (!show)
        glfwSetInputMode(win->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(win->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void destroy_window(window_t* win) {
    glfwDestroyWindow(win->handle);
}