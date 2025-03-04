#version 430 core

out vec4 fragColor;

uniform vec2 uResolution;
uniform vec2 uOffset;
uniform sampler2D image;

void main() {
	vec2 fragCoord = gl_FragCoord.xy - uOffset;

	vec2 uv = fragCoord / uResolution;
	fragColor = vec4(texture(image, uv).rgb, 1.0);
	//fragColor = vec4(uv, 0.0, 1.0);
}