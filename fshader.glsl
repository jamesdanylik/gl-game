#version 140

in vec3  fN;
in vec3  fL;
in vec3  fE;
in vec4  color;

out vec4 fColor;

uniform vec4 ambientProduct, diffuseProduct, specularProduct;
uniform vec4 lightPosition;
uniform float shininess;
uniform int picking;

void main()
{
		vec3 N = normalize(fN);
		vec3 E = normalize(fE);
		vec3 L = normalize(fL);
		vec3 H = normalize(L+E);

		vec4 ambient = ambientProduct;

		float Kd = max(dot(L,N), 0.0);
		vec4 diffuse = Kd*diffuseProduct;

		float Ks = pow(max(dot(N, H), 0.0), shininess);
		vec4 specular = Ks*specularProduct;

		if(dot(L,N) < 0.0)
		{
			specular = vec4(0.0, 0.0, 0.0, 1.0);
		}

		fColor = ambient+diffuse+specular;
		fColor.a = 1.0;

		if(picking == 1)
			fColor = color;
}