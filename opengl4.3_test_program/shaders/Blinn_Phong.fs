#version 430 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D floorTexture;
uniform sampler2D shadowTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

float inShadow()
{
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z > 1.0f ? 0.0f : projCoords.z;
	float bias = 0.005;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	return shadow / 9.0f;
}

void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
	vec3 ambient = 0.05 * color;
	
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * color;
	
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0f;
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(halfwayDir, normal), 0.0f), 32.0f);
	vec3 specular = lightColor * spec;
	
	float distance = length(lightPos - fs_in.FragPos);
	float attenuation = 1 / (1.0f + 0.045f * distance + 0.0075f * distance * distance);
	
	//FragColor = vec4((ambient + (1.0 - inShadow()) * (diffuse + specular)) * attenuation, 1.0f);
	FragColor = vec4(ambient + (1.0 - inShadow()) * (diffuse + specular), 1.0f);
}