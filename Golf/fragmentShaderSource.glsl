#version 330

uniform vec4 vColor;
in vec3 out_Color; //--- ���ؽ� ���̴����Լ� ���� ����
in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor; //--- ���� ���
uniform bool TextureOn; // �ؽ�ó ���� ����:���ϸ� �Ϲ� �����ڵ��

void main(void) 
{
	if (TextureOn) {
        FragColor = texture(ourTexture, TexCoord);  // �ؽ�ó ���
    }
    else {
        FragColor = vec4(out_Color, 1.0);  // �ؽ�ó�� ������� ������ ���ؽ� ���� ���
    }

    if (vColor.w > 0) {
        FragColor = vColor;  // vColor ���� ���� ���� vColor�� ���
    }
}
