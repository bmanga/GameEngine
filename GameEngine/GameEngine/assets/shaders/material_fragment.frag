#version 140

in vec2 texcoord;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D sampler;

void main(){

    // Output color = color of the texture at the specified UV
    color = texture( sampler, texcoord ) * vec4(1, 1, 1, 1);
}