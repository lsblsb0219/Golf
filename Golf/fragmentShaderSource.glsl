#version 330

uniform vec4 vColor;
in vec3 out_Color; //--- 버텍스 세이더에게서 전달 받음
out vec4 FragColor; //--- 색상 출력
void main(void) 
{
	FragColor = vec4 (out_Color, 1.0);
	if(vColor.w > 0){
	FragColor = vColor;
	}
}
