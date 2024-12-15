#version 330

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Color;
layout (location = 2) in vec2 aTexCoord;
out vec3 out_Color;
out vec2 TexCoord;

uniform mat4 modelTransform;
uniform mat4 projectionTransform;
uniform mat4 viewTransform;

uniform bool isSphere; // 구 여부 확인 변수
uniform vec3 sphereColor; // 구 색상

void main(void) 
{
	if (isSphere){ // 구 색상
		out_Color = sphereColor;
	}
	else { // 직육면체 색상
		out_Color = in_Color;
	}

	gl_Position =  projectionTransform *viewTransform * modelTransform*vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
	TexCoord = aTexCoord;

}