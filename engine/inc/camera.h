#pragma once

#include <window.h>

#include <cglm/cglm.h>


// :camera
typedef struct {
    mat4 view, proj;
    vec3 pos;
    float speed;
    vec3 front;
    // mouse
    bool first_mouse;
    float last_x, last_y;
    float yaw, pitch;
} camera_t;

camera_t create_camera(const float fov, const vec2 size);
void camera_update(camera_t* cam);
void camera_fly_controller(float dt, camera_t* cam, window_t* win);