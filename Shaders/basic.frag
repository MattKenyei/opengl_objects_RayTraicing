#version 330 core
in vec3 fragColor;
out vec4 outColor;
in vec2 texCoords;
uniform sampler2D outTex;
uniform bool wireframeMode;
//uniform vec3 uniformColor;
void main()
{
	outColor = vec4(fragColor, 1.0f);
//    if(wireframeMode)
//        outColor = vec4(fragColor, 1.0f);
//    else
//            outColor = texture(outTex, texCoords) * vec4(fragColor, 1.0f);
}