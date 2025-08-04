#version 330 core

in vec3 Normal;
in float RelativeHeight;

out vec4 FragColor;

float ease_in_quartic(float x) {
	float a = x*x;
	return a*a;
}

void main()
{
    vec3 base_color = vec3(0.05,0.2,0.01);
	vec3 tip_color = vec3(0.5,0.7,0.1);
	FragColor = vec4(mix(base_color, tip_color, ease_in_quartic(RelativeHeight)), 1.0);
	FragColor *= mix(0.1, 1.0, RelativeHeight*RelativeHeight);

    //FragColor = vec4(0.0f, Normal.y, 0.0f, 1.0f);
}