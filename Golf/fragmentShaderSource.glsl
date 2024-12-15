#version 330

uniform vec4 vColor;
in vec3 out_Color; //--- 버텍스 세이더에게서 전달 받음
in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor; //--- 색상 출력
uniform bool TextureOn; // 텍스처 매핑 여부:안하면 일반 색상코드로

void main(void) 
{
	if (TextureOn) {
        FragColor = texture(ourTexture, TexCoord);  // 텍스처 사용
    }
    else {
        FragColor = vec4(out_Color, 1.0);  // 텍스처를 사용하지 않으면 버텍스 색상 사용
    }

    if (vColor.w > 0) {
        FragColor = vColor;  // vColor 값이 있을 때는 vColor를 사용
    }
}
