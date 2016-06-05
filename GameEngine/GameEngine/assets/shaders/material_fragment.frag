#version 140

in vec2 textcoord;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

    // Output color = color of the texture at the specified UV
    color = texture( myTextureSampler, textcoord ) * vec4(1, 1, 1, 1);
}