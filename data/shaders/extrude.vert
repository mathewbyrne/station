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
  if (gl_Vertex.w == 0.0)
  {
    // These transformations handle point and directional lights by
    // using the w component of all vertices.
    gl_Vertex.x = lightPos.w * gl_Vertex.x - lightPos.x;
    gl_Vertex.y = lightPos.w * gl_Vertex.y - lightPos.y;
    gl_Vertex.z = lightPos.w * gl_Vertex.z - lightPos.z;
  }

  gl_Position = ftransform();
  gl_FrontColor = gl_Color;
}

