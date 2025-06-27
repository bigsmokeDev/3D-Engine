#include <window.h>
#include <renderer.h>

#include <stdio.h>

#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

int main(void) {
    window_t win = create_window(1280, 720, "Sandbox", true);
    window_show_cursor(&win, false);
    create_gl_context(&win, true);

    struct nk_glfw glfw = {0};
    struct nk_context* ctx;
    bool debug_mode = false;

    ctx = nk_glfw3_init(&glfw, win.handle, NK_GLFW3_INSTALL_CALLBACKS);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);

    renderer_t* renderer = create_renderer(&win);
    renderer->cam.speed = 2.5f;

    mesh_t* skybox = create_mesh("res/sky.obj", "res/skybox.png");
    glm_vec3_copy((vec3){.5f, .5f, .5f}, skybox->scale);

    mesh_t* backpack = create_mesh("res/backpack/backpack.obj", "res/backpack/texture.jpeg");
    glm_vec3_copy((vec3){.01f, .01f, .01f}, backpack->scale);
    glm_vec3_zero(backpack->pos);

    float dt = 0.f, last_frame = 0.f;

    while (!window_should_close(&win)) {
        float current_frame = window_get_time();
        dt = current_frame - last_frame;
        last_frame = current_frame;

        if (glfwGetKey(win.handle, GLFW_KEY_INSERT) == GLFW_PRESS) {
            debug_mode = !debug_mode;
            window_show_cursor(&win, debug_mode);
        }
        
        if (!debug_mode) camera_fly_controller(dt, &renderer->cam, &win);

        window_poll_events();
        window_handle_resize(&win);
        window_clear();

        renderer_begin(renderer);

        glm_vec3_copy(renderer->cam.pos, skybox->pos);
        renderer_draw_mesh(renderer, skybox);

        renderer_draw_mesh(renderer, backpack);

        renderer_end(renderer);

        if (debug_mode) {
            nk_glfw3_new_frame(&glfw);
            
            if (nk_begin(ctx, "Backpack", nk_rect(50.f, 50.f, 500.f, 200.f),
            NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
                nk_layout_row_dynamic(ctx, 20.f, 1);
                nk_label(ctx, "Position:", NK_TEXT_ALIGN_LEFT);
                nk_layout_row_dynamic(ctx, 20.f, 3);
                nk_slider_float(ctx, -50.f, &backpack->pos[0], 50.f, .5f);
                nk_slider_float(ctx, -50.f, &backpack->pos[1], 50.f, .5f);
                nk_slider_float(ctx, -50.f, &backpack->pos[2], 50.f, .5f);
                
                nk_end(ctx);
            }
            
            nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        }
       
        window_swap_buffers(&win);
    }
    
    destroy_window(&win);

    return 0;
}