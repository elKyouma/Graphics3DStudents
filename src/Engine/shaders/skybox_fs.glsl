layout(location = 0) in vec3 direction;

layout(location = 0) out vec4 vFragColor;

uniform samplerCube environment_map;

vec3 srgb_gamma_correction(vec3 color) {
   color = clamp(color, 0.0, 1.0);
   color = mix(color * 12.92, (1.055 * pow(color, vec3(1.0 / 2.4))) - 0.055, step(0.0031308, color));
   return color;
}


void main()
{
	vec3 color = texture(environment_map, direction).rgb;
	color = srgb_gamma_correction(color);
	vFragColor = vec4(color, 1.0);
}