#version 330

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Color;
out vec3 out_Color;

uniform mat4 modelTransform;
uniform mat4 projectionTransform;
uniform mat4 viewTransform;

void main(void) 
{
gl_Position =  projectionTransform *viewTransform * modelTransform*vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
out_Color = in_Color;
}