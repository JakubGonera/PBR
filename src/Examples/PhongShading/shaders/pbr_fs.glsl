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
    sampler2D roughness;
    sampler2D albedo;
    sampler2D metallic;
    sampler2D normal;
    sampler2D ao;
};

uniform Material material;
uniform samplerCube irradianceMap;
uniform samplerCube prefilteredMap;
uniform sampler2D brdfLUT;
uniform int ambientEnabled;

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

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness*roughness;
    float nom = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH * (a * a - 1.0) + 1);
    denom = PI * denom * denom;

    return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r * r)/8.0;
    return (NdotV/(NdotV * (1.0 - k) + k));
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return (F0 + (1 - F0) * pow(1.0 - cosTheta, 5.0));
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main() {
    vec3 N = calcNormalFromMap();

    vec3 V = normalize(viewPos.xyz - fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, texture(material.albedo, uv).rgb, texture(material.metallic, uv).x);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; i++){
        vec3 L = normalize(vec3(lightPos[i]) - fragPos);
        vec3 H = normalize(V + L);

        float attenuation = 1.0/(dot(vec3(lightPos[i]) - fragPos,vec3(lightPos[i]) - fragPos));
        vec3 radiance = light[i].xyz * attenuation * strength[i].x;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, texture(material.roughness, uv).x);
        float G = GeometrySmith(N, V, L, texture(material.roughness, uv).x);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - texture(material.metallic, uv).x;

        Lo += (kD * texture(material.albedo, uv).xyz / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    vec3 R = reflect(-V, N);

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, texture(material.roughness, uv).x);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - texture(material.metallic, uv).x;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * texture(material.albedo, uv).rgb;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilteredMap, R, texture(material.roughness, uv).x * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), texture(material.roughness, uv).x)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (diffuse * kD + specular) * texture(material.ao, uv).rgb * ambientEnabled;
    vec3 color = Lo + ambient;
    vFragColor.rgb = color;
}
