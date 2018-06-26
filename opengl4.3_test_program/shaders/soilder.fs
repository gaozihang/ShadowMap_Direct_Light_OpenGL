#version 430 core
out vec4 FragColor;

uniform vec3 worldViewPos;
uniform samplerCube skybox;

in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPos;

struct Material
{
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	float shininess;
};
uniform Material material;

// use point to calculate direction
struct Point_Light
{
	vec3 lightColor;
	vec3 position;
	
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Point_Light light;

void main()
{
	vec3 normalDir = normalize(Normal);
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 viewDir = normalize(worldViewPos - fragPos);
	
	float diff_factor = max(0.0f, dot(normalDir, lightDir));
	vec3 diffuse = texture(material.texture_diffuse0, TexCoords).rgb * diff_factor;

	vec3 normalHalf = normalize(lightDir + viewDir);
	float spec_factor = pow(max(0.0f, dot(normalHalf, normalDir)), 16.0f);
	vec3 specular = texture(material.texture_specular0, TexCoords).rgb * spec_factor;

	vec3 col = light.lightColor * (diffuse + specular);

	FragColor = vec4(col, 1.0);
}


