#version 410

layout(location = 0) in vec2 in_position;  // Position en 2D
layout(location = 1) in vec2 in_uv;        // Coordonnées UV
uniform mat4 view_projection_matrix;       // Matrice de vue + projection
uniform vec3 camera_position;              // Position de la caméra
out vec2 uv_position;                      // Sortie pour les coordonnées UV

void main()
{
    // Définir le plan à une distance fixe de la caméra
    float distance_to_camera = 20.0;  // Distance constante du plan à la caméra

    // Calculer la position du plan en 3D (ici, on place le plan sur l'axe Z)
    vec3 position_3d = vec3(in_position.x, in_position.y, distance_to_camera);

    // Calculer la direction du plan vers la caméra
    vec3 to_camera = normalize(camera_position - position_3d);
    
    // Appliquer la transformation de la vue et de la projection
    gl_Position = view_projection_matrix * vec4(position_3d, 1.0);

    // Passer les coordonnées UV au fragment shader
    uv_position = in_uv;
}