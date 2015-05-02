attribute float particleRadius;


void main(void)
{
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	float distance = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float attenuation = 3000.0 / distance;
	gl_PointSize = particleRadius * attenuation;
    gl_FrontColor = gl_Color;
}

/*
void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_PointSize = particleRadius;
	gl_FrontColor = gl_Color;
}
*/
