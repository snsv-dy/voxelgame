#version 330 core

out vec4 FragColor;

//uniform vec3 light_position;

uniform bool selected;


 in vec3 Normal;
 in vec3 fragPos;



void main()
{
	vec3 lightColor = vec3(1.0f);
	
	vec3 light_position = vec3(0.0f, 2.0f, 5.0f);
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light_position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	
	vec3 diffuse = (diff) * lightColor;
	
	//vec3 objColor = vec3(1.0f, 0.0f, 0.0f);
	//vec3 objColor = vec3(1.0f, 0.5f, 0.0f);
	vec3 objColor = vec3(1.0f, 0.5f, 0.2f);
	
	vec3 result = (0.2 + diffuse) * objColor;
	if(selected){
		result = vec3(result.xy, 1.0f);
	}
	
	FragColor = vec4(result, 1.0);
	//FragColor = vec4(max(abs(norm), norm), 1.0);
}