#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "iostream"
#include "thread"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);

    auto const shader = gl::Shader{{ //Shader
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const background_shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/background_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/background_fragment.glsl"},
    }};

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
        .layout = {gl::VertexAttribute::Position2D{0}},
        .data = {
           -0.5f, -0.5f,
           -0.5f, 0.5f,
           0.5f, -0.5f,
           0.5f, 0.5f,
        },
        }},
        .index_buffer = {
            0, 1, 2,
            1, 3, 2
        },
    }};

    auto const background_mesh = gl::Mesh{{
        .vertex_buffers = {{
        .layout = {gl::VertexAttribute::Position2D{0}},
        .data = {
            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f,
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

    auto last_time = std::chrono::high_resolution_clock::now();
    
    while (gl::window_is_open())
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double>(current_time - last_time).count();

        if (elapsed_time < frame_time) 
        {
            double time_to_wait = frame_time - elapsed_time;
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_wait));
            continue; // On saute le reste de la boucle tant que le temps d'attente n'est pas fini
        }

        last_time = std::chrono::high_resolution_clock::now();

        float time = gl::time_in_seconds();
        float positionX = std::sin(time * speed);

        //glClearColor(0.f, 0.f, 1.f, 0.1f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        //glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus

        background_shader.bind();
        background_shader.set_uniform("fade_alpha", 0.05f);
        background_mesh.draw();

        shader.bind();
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", positionX);
        triangle_mesh.draw();
    }
}