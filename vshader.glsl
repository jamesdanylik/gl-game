#version 430

in  vec4 vPosition;
in  vec3 vNormal;

uniform  vec4 pickingColor;
uniform  vec4 ambientProduct;
uniform  vec4 diffuseProduct;
uniform  vec4 specularProduct;
uniform  float shininess;
uniform  vec4 lightPosition;
uniform  mat4 transMat;
uniform  mat4 finishMat;
uniform  mat4 projMat;
uniform  mat4 viewMat;
uniform  float scaleAmt;
uniform  float theta;
uniform  int picking;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec4 color;
out mat4 modelView;

void main() 
{
  mat4 scaleMat = mat4 (scaleAmt, 0.0, 0.0, 0.0,
						 0.0, scaleAmt, 0.0, 0.0,
						 0.0, 0.0, scaleAmt, 0.0,
						 0.0, 0.0, 0.0, 1.0);
  float psi = radians(theta);
  mat4 rotateMat = mat4 (cos(psi), 0.0, sin(psi), 0.0,
						0.0, 1.0, 0.0, 0.0,
						-sin(psi), 0.0, cos(psi), 0.0,
						0.0, 0.0, 0.0, 1.0);
  mat4 modelMat;

  modelMat = finishMat * rotateMat * transMat * scaleMat;
  modelView = viewMat * modelMat;

  fN = (modelMat*vec4(vNormal.x, vNormal.y, vNormal.z, 0.0)).xyz;
  fE = (modelView*vPosition).xyz;
  fL = lightPosition.xyz - (modelMat*vPosition).xyz;

  color = pickingColor;

  gl_Position = projMat * modelView * vPosition;

} 
