#version 450

layout(binding = 0) uniform sampler2D Texture;

in vec2 TexCoord;

out vec4 outColor;

void main() {
    outColor = texture2D(Texture, TexCoord);
}