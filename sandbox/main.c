#include <window.h>
#include <renderer.h>

#include <stdio.h>

int main(void) {
    window_t win = create_window(1280, 720, "Sandbox", true);
    window_show_cursor(&win, false);
    create_gl_context(&win, true);

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
        window_poll_events();
        window_handle_resize(&win);
        window_clear();

        renderer_begin(renderer);
        camera_fly_controller(dt, &renderer->cam, &win);

        //glm_vec3_copy(skybox->pos, renderer->cam.pos);
        renderer_draw_mesh(renderer, skybox);

        renderer_draw_mesh(renderer, backpack);

        renderer_end(renderer);
       
        window_swap_buffers(&win);
    }
    
    destroy_window(&win);

    return 0;
}