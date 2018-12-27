#version 450
precision highp float;
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in vec2 Texcoord[];

out vec2 TexCoord;

void main() {
    for(int i = 0; i < gl_in.length(); i++) {
        TexCoord = Texcoord[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}