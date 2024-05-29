#version 330 core
in vec3 fragColor;
out vec4 outColor;
in vec2 texCoords;
uniform sampler2D outTex;
uniform bool wireframeMode;
uniform float time; // Добавляем переменную времени для анимации

// Функция для вычисления цвета радуги на основе времени
vec3 rainbow(float t) {
    float r = 0.5 * sin(t * 2.0 * 3.14159 + 0.0) + 0.5;
    float g = 0.5 * sin(t * 2.0 * 3.14159 + 2.0) + 0.5;
    float b = 0.5 * sin(t * 2.0 * 3.14159 + 4.0) + 0.5;
    return vec3(r, g, b);
}

void main()
{
    if (wireframeMode) {
        outColor = vec4(fragColor, 1.0f);
    } else {
        // Создаем волновой эффект
        float waveFrequency = 10.0;
        float waveAmplitude = 0.02;
        vec2 waveTexCoords = texCoords;
        waveTexCoords.y += sin(waveTexCoords.x * waveFrequency + time * 2.0) * waveAmplitude;
        
        vec4 texColor = texture(outTex, waveTexCoords);
        
        // Применяем эффект радуги к цвету
        vec3 rainbowColor = rainbow(time);
        vec3 finalColor = fragColor * rainbowColor;
        
        outColor = texColor * vec4(finalColor, 1.0f);
    }
}
