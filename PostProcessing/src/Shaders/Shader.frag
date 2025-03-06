#version 430 core

out vec4 fragColor;

uniform float time;
uniform vec2 uResolution;
uniform vec2 uOffset;
uniform sampler2D image;

vec3 grayScale(vec3 color) {
	return vec3(color.r + color.g + color.b) / 3.0;
}

vec3 kernelEffect(float kernel[9], vec2 TexCoords) {
    const float offset = 1.0 / 300.0;  

	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(image, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    return col;
}

vec3 sharpen(vec2 TexCoords) {
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    return kernelEffect(kernel, TexCoords);
}

vec3 blur(vec2 TexCoords) {
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    return kernelEffect(kernel, TexCoords);
}

vec3 edgeDetection(vec2 TexCoords) {
    float kernel[9] = float[](
        1.0, 1.0, 1.0,
        1.0, -8.0, 1.0,
        1.0, 1.0, 1.0 
    );

    return kernelEffect(kernel, TexCoords);
}

vec3 inverseColor(vec3 color) {
    return 1.0 - color;
}

void main() {
	vec2 fragCoord = gl_FragCoord.xy - uOffset;
	vec2 uv = fragCoord / uResolution;

    vec3 color;

	color = texture(image, uv).rgb;
	//color = vec3(uv, 0.0);

    //color = sharpen(uv);
    //color = blur(uv);
    //color = edgeDetection(uv);
    //color = grayScale(color);
    color = mix(color, inverseColor(color), (sin(time) + 1.0) / 2.0);

    fragColor = vec4(color, 1.0);
}