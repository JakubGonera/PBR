#version 410

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec3 a_vertex_normal;
layout(location=2) in  vec3 a_vertex_tangent;
layout(location=3) in  vec2 a_vertex_uv;

layout(std140)  uniform Transformations {
    mat4 projection;
    mat4 view;
    mat4 model;
};

out vec3 normal;
out vec3 tangent;
out vec3 fragPos;
out vec2 uv;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    normal = normalMatrix * a_vertex_normal;
    tangent = normalMatrix * a_vertex_tangent;
    gl_Position = projection * view * model * a_vertex_position;
    fragPos = vec3(model * a_vertex_position);
    uv = a_vertex_uv;
}
