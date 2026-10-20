#version 450 core

layout(location=0) in vec4 v_Color;
layout(location=1) in vec2 v_TexCoord;
layout(location=2) in vec3 v_Normal;
layout(location=3) in vec3 v_Position;
layout(location=4) in vec4 vertex_tangent_vs;

layout(location=0) out vec4 vFragColor;

uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ka;
uniform sampler2D map_Ns;
uniform sampler2D map_bump;

uniform samplerCube environment_map;
uniform bool use_environment_map;
uniform mat3 view_to_world;

layout(std140, binding=0) uniform BlinnPhongMaterial {
    vec4 Ka;
    vec4 Kd;
    vec4 Ks;
    float Ns;
    bool use_vertex_colors;
    bool use_map_Kd;
    bool use_map_Ka;
    bool use_map_Ks;
    bool use_map_Ns;
    bool use_map_bump;
    int illum;
};

struct PointLight {
    vec3 position;
    float radius;
    vec3 color;
    float intensity;
};


const float INV_PI = 0.3183098861837907;
const int MAX_POINT_LIGHTS = 16;

layout (std140, binding = 2) uniform Lights {
    vec3 ambient;
    int n_lights;
    PointLight lights[MAX_POINT_LIGHTS];
};

vec3 srgb_gamma_correction(vec3 color) {
   color = clamp(color, 0.0, 1.0);
   color = mix(color * 12.92, (1.055 * pow(color, vec3(1.0 / 2.4))) - 0.055, step(0.0031308, color));
   return color;
}

void main() {
    vec4 texture_color = texture(map_Kd, v_TexCoord);

    vec4 color = use_vertex_colors ? v_Color * Kd : Kd;
    if (use_map_Kd) {
        color *= texture_color;
    }

    vec3 normal = normalize(v_Normal);

    if (use_map_bump) {
        vec3 tangent = vertex_tangent_vs.xyz;
        vec3 bitangent = vertex_tangent_vs.w * cross(normal, tangent);
        vec3 n_ts = 2.0 * texture(map_bump, v_TexCoord).rgb - 1.0;
        normal = n_ts.x * tangent + n_ts.y * bitangent + n_ts.z * normal;
    }
    normal = normalize(normal);
    
    if (!gl_FrontFacing) {
        normal = -normal;
    }


    vec3 ambient_color = Ka.rgb;
    if (use_map_Ka) {
        ambient_color *= texture(map_Ka, v_TexCoord).rgb;
    }


    vFragColor.a = color.a;
    vec3 frag_color = ambient_color * ambient;
    
    float shininess = Ns;
    if (use_map_Ns) {
        shininess *= texture(map_Ns, v_TexCoord).r;
    }

    vec3 specularColor = Ks.rgb;
    if (use_map_Ks) {
        specularColor *= texture(map_Ks, v_TexCoord).rgb;
    }

    vec3 view_dir = -normalize(v_Position);

    for (int i = 0; i < n_lights; i++) {
        vec3 light_vector = lights[i].position - v_Position;
        float light_distance = length(light_vector);
        vec3 light_dir = light_vector / light_distance;
        float r = max(lights[i].radius, light_distance);
        float attenuation = 1.0 / (r * r);

        vec3 half_vector = normalize(light_dir + view_dir);
        float specular = pow(max(dot(half_vector, normal), 0.0), shininess);
        float diffuse = max(dot(normal, light_dir), 0.0);
        vec3 light_energy =  lights[i].color * lights[i].intensity * attenuation * diffuse;
        if(illum >= 2)
        {
            frag_color += (shininess + 8.0) * INV_PI / 8 * specular * specularColor.rgb * light_energy;
        }

        frag_color += INV_PI * color.rgb * light_energy;
    }
    if (illum == 3 && use_environment_map) {
        vec3 reflected_vs = reflect(-view_dir, normal);
        frag_color += specularColor * texture(environment_map, view_to_world * reflected_vs).rgb;
    }
    vFragColor.rgb = srgb_gamma_correction(frag_color);
}
