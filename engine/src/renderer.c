#include <stdio.h>

#include <glad/gl.h>
#include <renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define FAST_OBJ_IMPLEMENTATION
#include <fastobj/fast_obj.h>


bool create_gl_context(window_t* win, bool vsync) {
    glfwMakeContextCurrent(win->handle);
    glfwSwapInterval(vsync);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        return false;

    return true;
}

// :shaders
shader_t create_shader(const char* vs_src, const char* fs_src) {
    shader_t shd;

    uint32_t vs, fs;
    int32_t result; char info_log[1024];

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vs, 1024, NULL, info_log);
        printf("vertex shader compilation failed, error: %s\n", info_log);
        return 0;
    }

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fs, 1024, NULL, info_log);
        printf("fragment shader compilation failed, error: %s\n", info_log);
        return 0;
    }

    shd = glCreateProgram();
    glAttachShader(shd, vs);
    glAttachShader(shd, fs);
    glLinkProgram(shd);
    glValidateProgram(shd);
    glGetProgramiv(shd, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shd, 1024, NULL, info_log);
        printf("shader program linking failed, error: %s\n", info_log);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shd;
}

void shader_use(shader_t shd) {
    glUseProgram(shd);
}

int32_t shader_get_uniform_loc(shader_t shd, const char* name) {
    return glGetUniformLocation(shd, name);
}

void shader_set_vec3(int32_t loc, const vec3 value) {
    glUniform3fv(loc, 1, value);
}

void shader_set_mat4(int32_t loc, const mat4 matrix) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix[0]);
}

void destroy_shader(shader_t shd) {
    glDeleteProgram(shd);
}

// :buffers
buffer_t create_buffer(const buffer_type_t type, const buffer_usage_t usage) {
    buffer_t buf = { .type = type, .usage = usage };

    glGenBuffers(1, &buf.id);

    return buf;
}

void buffer_use(buffer_t* buf) {
    glBindBuffer(buf->type, buf->id);
}

void buffer_data(buffer_t* buf, size_t size, const void* data) {
    glBindBuffer(buf->type, buf->id);
    glBufferData(buf->type, size, data, buf->usage);
}

void destroy_buffer(buffer_t* buf) {
    glDeleteBuffers(1, &buf->id);
}

// :attribs
vertex_attrib_t create_vertex_attrib() {
    vertex_attrib_t attrib;

    glGenVertexArrays(1, &attrib.id);

    return attrib;
}

void vertex_attrib_use(vertex_attrib_t attrib) {
    glBindVertexArray(attrib.id);
}

void vertex_attrib_new(vertex_attrib_t* attrib, const vertex_attrib_desc_t* desc) {
    glBindVertexArray(attrib->id);
    glVertexAttribPointer(
        desc->index,
        desc->size,
        desc->type,
        GL_FALSE,
        desc->stride,
        desc->p
    );
    glEnableVertexAttribArray(desc->index);
}

// :textures
texture_t* create_texture(const char* path) {
    texture_t* tex = malloc(sizeof(texture_t));

    int32_t channel;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path, &tex->w, &tex->h, &channel, 0);
    assert(data != NULL);
    int32_t format = channel > 3 ? GL_RGBA : GL_RGB;

    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex->w, tex->h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return tex;
}

void texture_use(texture_t* tex) {
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void destroy_texture(texture_t* tex) {
    glDeleteTextures(1, &tex->id);
}

// ========== 3D ==========
// :mesh
const char* mesh_vs_src = GLSL_VERSION
"layout (location = 0) in vec3 a_pos;"
"layout (location = 1) in vec3 a_normal;"
"layout (location = 2) in vec2 a_texcoord;"
"out vec3 normal;"
"out vec2 texcoord;"
"uniform mat4 model, view, proj;"
"void main() {"
    "gl_Position = proj * view * model * vec4(a_pos, 1.0);"
    "texcoord = a_texcoord;"
    "normal = a_normal;"
"}\0";
const char* mesh_fs_src = GLSL_VERSION
"out vec4 out_color;"
"in vec3 normal;"
"in vec2 texcoord;"
"uniform sampler2D smp;"
"void main() {"
    "out_color = texture(smp, texcoord);"
"}\0";

mesh_t* create_mesh(const char* path, const char* tex_path) {
    mesh_t* mesh = malloc(sizeof(mesh_t));

    mesh->shd = create_shader(mesh_vs_src, mesh_fs_src);
    shader_use(mesh->shd);
    mesh->loc.model = shader_get_uniform_loc(mesh->shd, "model");
    mesh->loc.view = shader_get_uniform_loc(mesh->shd, "view");
    mesh->loc.proj = shader_get_uniform_loc(mesh->shd, "proj");

    mesh->tex = create_texture(tex_path);
    
    fastObjMesh* mesh_obj = fast_obj_read(path);

    size_t vertex_count = 0;
    uint32_t* indices = malloc(sizeof(uint32_t) * mesh_obj->face_count * 3);
    vertex_t* vertices = malloc(sizeof(vertex_t) * mesh_obj->index_count);

    for (int32_t i = 0; i < mesh_obj->index_count; ++i) {
        fastObjIndex idx = mesh_obj->indices[i];

        vertex_t v = {0};
        memcpy(v.pos, &mesh_obj->positions[idx.p * 3], sizeof(float) * 3);
        if (idx.n >= 0)
            memcpy(v.normal, &mesh_obj->normals[idx.n * 3], sizeof(float) * 3);
        if (idx.t >= 0)
            memcpy(v.texcoord, &mesh_obj->texcoords[idx.t * 2], sizeof(float) * 2);

        vertices[vertex_count] = v;
        indices[i] = vertex_count;
        vertex_count++;
    }

    mesh->idx_count = mesh_obj->index_count;

    fast_obj_destroy(mesh_obj);

    mesh->attrib = create_vertex_attrib();
    vertex_attrib_use(mesh->attrib);
    
    mesh->vbo = create_buffer(BUFFER_TYPE_VERTEX, BUFFER_USAGE_STATIC);
    buffer_data(&mesh->vbo, vertex_count * sizeof(vertex_t), vertices);
    mesh->ibo = create_buffer(BUFFER_TYPE_INDEX, BUFFER_USAGE_STATIC);
    buffer_data(&mesh->ibo, mesh_obj->index_count * sizeof(uint32_t), indices);

    vertex_attrib_new(&mesh->attrib, &(vertex_attrib_desc_t){
        .index = 0,
        .size = 3,
        .type = GL_FLOAT,
        .stride = sizeof(vertex_t),
        .p = (void*)0
    });
    vertex_attrib_new(&mesh->attrib, &(vertex_attrib_desc_t){
        .index = 1,
        .size = 3,
        .type = GL_FLOAT,
        .stride = sizeof(vertex_t),
        .p = (void*)(offsetof(vertex_t, normal))
    });
    vertex_attrib_new(&mesh->attrib, &(vertex_attrib_desc_t){
        .index = 2,
        .size = 2,
        .type = GL_FLOAT,
        .stride = sizeof(vertex_t),
        .p = (void*)(offsetof(vertex_t, texcoord))
    });

    return mesh;
}

void destroy_mesh(mesh_t* mesh) {
    destroy_shader(mesh->shd);
    destroy_buffer(&mesh->vbo);
    destroy_buffer(&mesh->ibo);

    free(mesh);
}

// :renderer
renderer_t* create_renderer(window_t* win) {
    renderer_t* r = malloc(sizeof(renderer_t));

    r->cam = create_camera(60.f, (vec2){win->w, win->h});

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return r;
}

void renderer_begin(renderer_t* renderer) {
    camera_update(&renderer->cam);
}

void renderer_end(renderer_t* renderer) {
}

void renderer_draw_mesh(renderer_t* renderer, mesh_t* mesh) {
    shader_use(mesh->shd);
    vertex_attrib_use(mesh->attrib);

    texture_use(mesh->tex);

    glm_mat4_identity(mesh->model);
    glm_translate(mesh->model, mesh->pos);
    glm_scale(mesh->model, mesh->scale);

    shader_set_mat4(mesh->loc.model, mesh->model);
    shader_set_mat4(mesh->loc.view, renderer->cam.view);
    shader_set_mat4(mesh->loc.proj, renderer->cam.proj);

    glDrawElements(GL_TRIANGLES, mesh->idx_count, GL_UNSIGNED_INT, NULL);
}