
in vec4 color;
in vec2 UV;

out vec4 FragColor;
uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	//color = texture( myTextureSampler, UV ) * particlecolor;
	FragColor = color;

}