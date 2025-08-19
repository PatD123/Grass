#version 330 core

in vec3 Normal;
in vec3 FragPos;
in float RelativeHeight;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;
uniform vec3 camDir;

float ease_in_quartic(float x) {
	float a = x*x;
	return a*a;
}

void main()
{
    vec3 base_color = vec3(0.05,0.2,0.01);
	vec3 tip_color = vec3(0.22,0.49,0.17);
	vec3 Color = mix(base_color, tip_color, ease_in_quartic(RelativeHeight));
	Color *= mix(0.1, 1.0, RelativeHeight*RelativeHeight);

	// Ambient

    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular

    float specularStrength = 0.9;
    vec3 viewDir = normalize(camPos - FragPos); // From frag to cam
    vec3 reflectDir = reflect(-lightDir, norm);  // Across norm, pointing away from frag.
    float spec = pow(max(dot(viewDir, reflectDir) * 1.05, 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4(Color * (ambient + diffuse + specular), 1.0);

    // FragColor = vec4(Color, 1.0);

    //FragColor = vec4(0.0f, Normal.y, 0.0f, 1.0f);
}