#version 410

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D phongBuffer;
uniform float mixRatio;

void main(){
    const float gamma = 2.2;
    vec3 hdrColor = mix(texture(hdrBuffer, TexCoords).rgb, texture(phongBuffer, TexCoords).rgb, mixRatio);
    
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0/gamma));

    FragColor = vec4(mapped, 1);
}