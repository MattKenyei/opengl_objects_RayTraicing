#version 330 core
in vec3 fragColor;
out vec4 outColor;
in vec2 texCoords;
uniform sampler2D outTex;
uniform bool wireframeMode;
uniform bool useEffect;
uniform float time;

vec3 rainbow(float t) {
    float r = 0.35 * sin(t * 2.0 * 3.14159) + 0.65;  
    float g = 0.35 * sin(t * 2.0 * 3.14159 + 2.0) + 0.65;  
    float b = 0.35 * sin(t * 2.0 * 3.14159 + 4.0) + 0.65;   
    return vec3(r, g, b);
}

void main()
{
    if (wireframeMode) {
        outColor = vec4(fragColor, 1.0f);
    } else {
             if(useEffect)
             {
               float waveFrequency = 10.0;
               float waveAmplitude = 0.02;
               vec2 waveTexCoords = texCoords;
               waveTexCoords.y += sin(waveTexCoords.x * waveFrequency + time * 2.0) * waveAmplitude;
               vec4 texColor = texture(outTex, waveTexCoords);        
               vec3 rainbowColor =  rainbow(time);
               vec3 finalColor = fragColor * rainbowColor;
               outColor = texColor * vec4(finalColor, 1.0f);
             }
             else outColor = texture(outTex, texCoords) * vec4(fragColor, 1.0f);
            }        
    
}

// Лупа
//        vec2 center = vec2(0.5 + 0.5 * sin(time), 0.5 + 0.5 * cos(time)); // центр лупы перемещается по кругу
//        float radius = 0.2; // радиус лупы
//        float magnification = 2.0; // увеличение лупы
//
//        vec2 offset = texCoords - center;
//        if (length(offset) < radius) {
//            offset *= magnification;
//        }
//
//        vec2 magnifiedTexCoords = center + offset;
//
//        vec4 texColor = texture(outTex, magnifiedTexCoords);
//
//        vec3 rainbowColor = rainbow(time);
//        vec3 finalColor = fragColor * rainbowColor;
//        
//        outColor = texColor * vec4(finalColor, 1.0f);