// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D texture1;

in vec2 UV;

void main()
{
	float depth = texture(texture1, UV).r;
	color = vec4(depth,depth,depth,1.0);
}