#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBOMatrices
{
	mat4 projection;
	mat4 view;
	mat4 Model;
	vec3 camPos;
} ubo;

layout (location = 0) out vec2 outUV;

out gl_PerVertex 
{
    vec4 gl_Position;   
};
void main()
{
	outUV = inUV;
	gl_Position = ubo.projection * ubo.view *vec4(inPos.xyz, 1.0);
}