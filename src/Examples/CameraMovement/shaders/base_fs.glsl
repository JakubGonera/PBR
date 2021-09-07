#version 410

in vec4 vertex_color;

layout(std140) uniform Modifiers {
    float  strength;
    vec3   light;
};

layout(location=0) out vec4 vFragColor;

void main() {
    vFragColor.a = vertex_color.a;
    vFragColor.rgb = strength*light*vertex_color.rgb;
}
