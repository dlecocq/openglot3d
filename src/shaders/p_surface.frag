#version 110

varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;

varying vec3 light;
varying vec3 normal;

void main()
{
	// Defining The Material Colors
	vec4 AmbientColor = 0.4 * texture2D(my_color_texture, texture_coordinate);
	AmbientColor.a = 1.0;
	vec4 DiffuseColor =       texture2D(my_color_texture, texture_coordinate);

	// Scaling The Input Vector To Length 1
	vec3 norm_normal = normalize(normal);
	vec3 norm_light = normalize(light);

	// Calculating The Diffuse Term And Clamping It To [0;1]
	float DiffuseTerm = clamp(dot(norm_normal, norm_light), 0.0, 1.0);

	// Calculating The Final Color
	gl_FragColor = AmbientColor + DiffuseColor * DiffuseTerm;
	
	// This is pretty awesome - it just colors it like a normal map.
	gl_FragColor = vec4(norm_normal.x, norm_normal.y, norm_normal.z, 1.0);

}