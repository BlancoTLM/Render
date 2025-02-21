#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "iostream"
#include "thread"
#include "glm/ext/matrix_clip_space.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    auto const shader = gl::Shader{{ //Shader
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
        .layout = {gl::VertexAttribute::Position3D{0}},
        .data = {
           -0.5f, -0.5f, 0.f,
           -0.5f, 0.5f, 0.f,
           0.5f, -0.5f, 0.f,
           0.5f, 0.5f, 0.f,
        },
        }},
        .index_buffer = {
            0, 1, 2,
            1, 3, 2
        },
    }};

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
        // Start limit framerate
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double>(current_time - last_time).count();

        if (elapsed_time < frame_time) 
        {
            double time_to_wait = frame_time - elapsed_time;
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_wait));
            continue;
        }

        last_time = std::chrono::high_resolution_clock::now();
        // End limit framerate

        float time = gl::time_in_seconds();
        float positionX = std::sin(time * speed);

        glClearColor(0.f, 0.f, 1.f, 0.1f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus

        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(45.f, gl::framebuffer_aspect_ratio(), 0.001f);

        shader.bind();
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", positionX);
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
        triangle_mesh.draw();
    }
}