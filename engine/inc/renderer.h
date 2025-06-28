#pragma once

#define GL_VERSION_MAJOR 3
#define GL_VERSION_MINOR 3
#define GL_PROFILE GLFW_OPENGL_CORE_PROFILE
#define GLSL_VERSION "#version 330 core\n"

#include <stdint.h>


#include <window.h>
#include <camera.h>

#include <cglm/cglm.h>


bool create_gl_context(window_t* win, bool vsync);

// :shaders
typedef uint32_t shader_t;

shader_t create_shader(const char* vs_src, const char* fs_src);
void shader_use(shader_t shd);
int32_t shader_get_uniform_loc(shader_t shd, const char* name);
void shader_set_vec3(int32_t loc, const vec3 value);
void shader_set_mat4(int32_t loc, const mat4 matrix);
void destroy_shader(shader_t shd);

// :buffers
typedef enum {
    BUFFER_TYPE_VERTEX = 0x8892,
    BUFFER_TYPE_INDEX = 0x8893
} buffer_type_t;

typedef enum {
    BUFFER_USAGE_STATIC = 0x88E4,
    BUFFER_USAGE_DYNAMIC = 0x88E8
} buffer_usage_t;

typedef struct {
    uint32_t id;
    buffer_type_t type;
    buffer_usage_t usage;
} buffer_t;

buffer_t create_buffer(const buffer_type_t type, const buffer_usage_t usage);
void buffer_use(buffer_t* buf);
void buffer_data(buffer_t* buf, size_t size, const void* data);
void destroy_buffer(buffer_t* buf);

// :attribs
typedef struct {
    uint32_t id;
} vertex_attrib_t;

typedef struct {
    uint32_t index;
    int32_t size;
    GLenum type;
    size_t stride;
    void* p;
} vertex_attrib_desc_t;

vertex_attrib_t create_vertex_attrib();
void vertex_attrib_use(vertex_attrib_t attrib);
void vertex_attrib_new(vertex_attrib_t* attrib, const vertex_attrib_desc_t* desc);

// :textures
typedef struct {
    uint32_t id;
    uint32_t w, h;
} texture_t;

texture_t* create_texture(const char* path);
void texture_use(texture_t* tex);
void destroy_texture(texture_t* tex);

// ========== 3D ==========
// :mesh
typedef struct {
    vec3 pos, normal;
    vec2 texcoord;
} vertex_t;

typedef struct {
    shader_t shd;
    struct {
        int32_t model, view, proj;
    } loc;

    buffer_t vbo, ibo;
    int32_t idx_count;
    vertex_attrib_t attrib;
    texture_t* tex;

    mat4 model;
    vec3 pos, scale;
} mesh_t;

mesh_t* create_mesh(const char* path, const char* tex_path);
void destroy_mesh(mesh_t* mesh);

// :renderer
typedef struct {
    camera_t cam;
} renderer_t;

renderer_t* create_renderer(window_t* win);
void renderer_begin(renderer_t* renderer);
void renderer_end(renderer_t* renderer);
void renderer_draw_mesh(renderer_t* renderer, mesh_t* mesh);