#include <camera.h>

#define CAM_UP (vec3){0.f, 1.f, 0.f}


camera_t create_camera(const float fov, const vec2 size) {
    camera_t cam = {0};

    glm_mat4_identity(cam.view);
    glm_vec3_copy((vec3){0}, cam.pos);
    glm_perspective(glm_rad(fov), size[0] / size[1], .1f, 100.f, cam.proj);
    glm_vec3_copy((vec3){0.f, 0.f, -1.f}, cam.front);
    cam.first_mouse = true;

    return cam;
}

void camera_update(camera_t* cam) {
    glm_mat4_identity(cam->view);
    vec3 center;
    glm_vec3_add(cam->pos, cam->front, center);
    glm_lookat(cam->pos, center, CAM_UP, cam->view);
}

void camera_fly_controller(float dt, camera_t* cam, window_t* win) {
    const float speed = cam->speed * dt;
    vec3 v;

    if (glfwGetKey(win->handle, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_scale(cam->front, speed, v);
        glm_vec3_add(cam->pos, v, cam->pos);
    }
    if (glfwGetKey(win->handle, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_scale(cam->front, -speed, v);
        glm_vec3_add(cam->pos, v, cam->pos);
    }
    if (glfwGetKey(win->handle, GLFW_KEY_D) == GLFW_PRESS) {
        glm_cross(cam->front, CAM_UP, v);
        glm_vec3_normalize(v);
        glm_vec3_scale(v, speed, v);
        glm_vec3_add(cam->pos, v, cam->pos);
    }
    if (glfwGetKey(win->handle, GLFW_KEY_A) == GLFW_PRESS) {
        glm_cross(cam->front, CAM_UP, v);
        glm_vec3_normalize(v);
        glm_vec3_scale(v, speed, v);
        glm_vec3_sub(cam->pos, v, cam->pos);
    }
    if (glfwGetKey(win->handle, GLFW_KEY_E) == GLFW_PRESS) {
        glm_vec3_scale(CAM_UP, speed, v);
        glm_vec3_add(cam->pos, v, cam->pos);
    }
    if (glfwGetKey(win->handle, GLFW_KEY_Q) == GLFW_PRESS) {
        glm_vec3_scale(CAM_UP, -speed, v);
        glm_vec3_add(cam->pos, v, cam->pos);
    }

    double mouse_x, mouse_y;
    glfwGetCursorPos(win->handle, &mouse_x, &mouse_y);

    if (cam->first_mouse) {
        cam->last_x = mouse_x;
        cam->last_y = mouse_y;
        cam->first_mouse = false;
    }

    float x_offset = mouse_x - cam->last_x;
    float y_offset = cam->last_y - mouse_y;
    cam->last_x = mouse_x;
    cam->last_y = mouse_y;

    float sens = .1f;
    x_offset *= sens;
    y_offset *= sens;

    cam->yaw += x_offset;
    cam->pitch += y_offset;

    if (cam->pitch > 89.f)  cam->pitch = 89.f;
    if (cam->pitch < -89.f) cam->pitch = -89.f;

    vec3 dir;
    dir[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    dir[1] = sinf(glm_rad(cam->pitch));
    dir[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    glm_normalize(dir);
    glm_vec3_copy(dir, cam->front);
}