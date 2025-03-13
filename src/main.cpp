#include "opengl-framework/opengl-framework.hpp"
#include "iostream"
#include "thread"
#include "glm/ext/matrix_clip_space.hpp"
#include "tiny_obj_loader.h"

// Fonction de chargement du mesh
auto load_mesh(std::filesystem::path const& path) -> gl::Mesh
{
    // On lit le fichier avec tinyobj
    auto reader = tinyobj::ObjReader{};
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty())
        throw std::runtime_error("Failed to read 3D model:\n" + reader.Error());
    if (!reader.Warning().empty())
        std::cout << "Warning while reading 3D model:\n" + reader.Warning();

    auto const& attrib = reader.GetAttrib();
    auto const& shapes = reader.GetShapes();
    
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // On met tous les attributs dans un tableau
    for (auto const& shape : reader.GetShapes())
    {
        for (auto const& idx : shape.mesh.indices)
        {
            // Position
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]);

            // UV
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normale
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
        };
    }

    return gl::Mesh{{
        .vertex_buffers = {{
            .layout = {
                gl::VertexAttribute::Position3D{0},
                gl::VertexAttribute::UV{1},
                gl::VertexAttribute::Normal3D{2}
            },
            .data = vertices
        }},
        .index_buffer = indices,
    }};
}

int main()
{
    // Initialisation
    gl::init("TPs de Rendering");
    gl::maximize_window();
    glEnable(GL_DEPTH_TEST);

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    // Chargement du modèle 3D
    auto const mesh = load_mesh("res/fourareen.obj");

    auto const texture = gl::Texture{
        gl::TextureSource::File{
            .path           = "res/fourareen2K_albedo.jpg",
            .flip_y         = true,
            .texture_format = gl::InternalFormat::RGBA8,
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear,
            .magnification_filter = gl::Filter::Linear,
            .wrap_x               = gl::Wrap::Repeat,
            .wrap_y               = gl::Wrap::Repeat,
        }
    };

    float speed = 2.0f;
    const int target_fps = 60;
    const double frame_time = 1.0 / target_fps;

    auto camera = gl::Camera{};
    auto last_time = std::chrono::high_resolution_clock::now();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    gl::set_events_callbacks({camera.events_callbacks()});

    while (gl::window_is_open())
    {
        // Limitation du framerate
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double>(current_time - last_time).count();

        if (elapsed_time < frame_time) 
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(frame_time - elapsed_time));
            continue;
        }

        last_time = std::chrono::high_resolution_clock::now();

        float positionX = 0;

        glClearColor(1.f, 0.f, 1.f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(45.f, gl::framebuffer_aspect_ratio(), 0.001f);

        shader.bind();
        shader.set_uniform("light_direction", glm::vec3(1,1,1));
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", positionX);
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
        shader.set_uniform("square_image", 0);

        mesh.draw();
    }
}