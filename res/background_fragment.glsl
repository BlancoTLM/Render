#version 410

out vec4 frag_color;
uniform float fade_alpha; // Niveau de transparence du fond

void main()
{
    frag_color = vec4(0.0, 0.0, 1.0, fade_alpha); // Bleu semi-transparent
}