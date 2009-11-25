//
// Extrude Vertex Shader.
//
// Extrudes a vertex along a line between itself and a light point
// if that vertex has a w coordinate of 0. It is assumed that the
// provided light position is in local space to the object.
//

uniform vec4 lightPos;

void main()
{
	vec4 v = gl_Vertex;

	if (v.w == 0.0)
	{
	// These transformations handle point and directional lights by
	// using the w component of all vertices.
		v.x = lightPos.w * gl_Vertex.x - lightPos.x;
		v.y = lightPos.w * gl_Vertex.y - lightPos.y;
		v.z = lightPos.w * gl_Vertex.z - lightPos.z;
	}

	gl_Position = gl_ModelViewProjectionMatrix * v;
	gl_FrontColor = gl_Color;
}

