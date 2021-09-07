#version 410

#define PI 3.14159265

in vec3 normal;
in vec3 tangent;
in vec3 fragPos;
in vec2 uv;

layout(std140) uniform Modifiers {
    vec4   viewPos;
    int    lightCount;
    vec4   strength[8];
    vec4   light[8];
    vec4   lightPos[8];
};

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D roughness;
};

uniform Material material;

layout(location=0) out vec4 vFragColor;

vec3 calcNormalFromMap(){
    vec3 Normal = normalize(normal);
    vec3 Tangent = normalize(tangent);
    Tangent = normalize(Tangent - dot(Normal, Tangent) * Normal);
    vec3 bitangent = cross(Tangent, Normal);
    vec3 bumpMapNormal = texture(material.normal, uv).xyz;
    bumpMapNormal = 2 * bumpMapNormal - 1.0;
    vec3 outNormal;
    mat3 TBN = mat3(Tangent, bitangent, Normal);
    outNormal = TBN * bumpMapNormal;
    outNormal = normalize(outNormal);
    return outNormal;
}

void main() {
    vec3 newNormal = calcNormalFromMap();

    vec3 ambient = 0.03 * texture(material.diffuse, uv).rgb;
    vec3 color = ambient;

    vec3 viewDir = normalize(viewPos.xyz - fragPos);
    for(int i = 0; i < lightCount; i++){
        vec3 lightDir = normalize(vec3(lightPos[i]) - fragPos);
        vec3 H = normalize(lightDir + viewDir);

        float attenuation = 1.0/(dot(vec3(lightPos[i]) - fragPos,vec3(lightPos[i]) - fragPos));

        vec3 reflectDir = reflect(-lightDir, newNormal);
        float shininess = 1.0 - texture(material.roughness, uv).x;
        float spec = pow(max(dot(newNormal, H), 0.0), 256.0 * shininess);
        if(dot(newNormal, lightDir) <= 0){
            spec = 0;
        }
        vec3 specular = strength[i].x * spec * light[i].rgb * attenuation * texture(material.specular, uv).rgb * 0.3;
        
        float diff = max(dot(newNormal, lightDir), 0.0);
        vec3 diffuse = strength[i].x * diff * light[i].rgb * attenuation * texture(material.diffuse, uv).rgb * 0.3;
        
        color += specular + diffuse;
    }

    vFragColor.rgb = color;
}
