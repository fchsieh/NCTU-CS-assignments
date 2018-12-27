#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;

out vec2 Texcoord;

void main() {
    Texcoord = texcoord;
    gl_Position = Projection * ModelView * vec4(pos, 1.0);
}