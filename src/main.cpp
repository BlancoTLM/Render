#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "iostream"
#include "thread"
#include "glm/ext/matrix_clip_space.hpp"


int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut
    glEnable(GL_DEPTH_TEST);

    auto const shader = gl::Shader{{ //Shader
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const plane_shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/plane_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/plane_fragment.glsl"},
    }};

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
        .layout = {
            gl::VertexAttribute::Position3D{0},
            gl::VertexAttribute::UV{1}
        },
        .data = {
        0,0,0, //0
        0,0,
        0,0,1, //1
        0,0,
        0,1,0, //2
        0,0,
        0,1,1, //3
        1,0,
        1,0,0, //4
        0,0,
        1,0,1, //5
        0,1,
        1,1,0, //6
        0,0,
        1,1,1, //7
        1,1,
        },
        }},
        .index_buffer = {
            // Face avant (Z = 0)
            0, 2, 6,
            0, 6, 4,
                
            // Face arrière (Z = 1)
            1, 5, 7,
            1, 7, 3,
                
            // Face gauche (X = 0)
            0, 1, 3,
            0, 3, 2,
                
            // Face droite (X = 1)
            4, 6, 7,
            4, 7, 5,
                
            // Face haut (Y = 1)
            2, 3, 7,
            2, 7, 6,
                
            // Face bas (Y = 0)
            0, 4, 5,
            0, 5, 1
        },
    }};

    auto const plane_mesh = gl::Mesh{{
        .vertex_buffers = {{
        .layout = {
            gl::VertexAttribute::Position3D{0},
            gl::VertexAttribute::UV{1}
        },
        .data = {
        0,0,0, //0
        0,0,
        0,1,0, //1
        0,1,
        1,0,0, //2
        1,0,
        1,1,0, //3
        1,1,
        },
        }},
        .index_buffer = {
            // Face avant (Z = 0)
            0, 1, 2,
            1, 2, 3,
        },
    }};

    auto const texture = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path           = "res/square.jpg",
            .flip_y         = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width          = gl::framebuffer_width_in_pixels(),
        .height         = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format  = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter  = gl::Filter::NearestNeighbour, // On va toujours afficher la texture à la taille de l'écran,
                    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
                    .wrap_x               = gl::Wrap::ClampToEdge,
                    .wrap_y               = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format  = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter  = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x               = gl::Wrap::ClampToEdge,
                .wrap_y               = gl::Wrap::ClampToEdge,
            },
        },
    }};

    float speed = 2.0f;
    const int target_fps = 60;
    const double frame_time = 1.0 / target_fps;

    auto camera = gl::Camera{};
    auto last_time = std::chrono::high_resolution_clock::now();
    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
            if(e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
                render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });
    
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
        // float positionX = std::sin(time * speed);
        float positionX = 0;

        glm::mat4 view_matrix = camera.view_matrix();
        glm::mat4 projection_matrix = glm::infinitePerspective(45.0f, gl::framebuffer_aspect_ratio(), 0.001f);

        render_target.render([&]() {
            glClearColor(0.f, 1.f, 0.f, 0.1f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT);
            
            shader.bind();
            shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
            shader.set_uniform("offset", positionX);
            shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
            shader.set_uniform("square_image", texture);
            triangle_mesh.draw();
        });

        glm::vec3 camera_position = camera.position();  // Utiliser la position de la caméra

        plane_shader.bind();
        plane_shader.set_uniform("camera_position", camera_position);
        plane_shader.set_uniform("square_image", render_target.depth_stencil_texture());
        plane_shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
        plane_mesh.draw();
    }
}