uniform mat4 inverse_PV_rotation;

layout(location = 0) out vec3 direction;

void main() {
	vec2 position = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;
	gl_Position = vec4(position, 1.0, 1.0);

	vec4 world = inverse_PV_rotation * gl_Position;
	direction = world.xyz / world.w;
}
