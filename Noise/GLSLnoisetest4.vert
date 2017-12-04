uniform float time;

/*
 * Both 2D and 3D texture coordinates are defined, for testing purposes.
 */
varying vec2 v_texCoord2D;
varying vec3 v_texCoord3D;
varying vec4 v_color;

void main( void )
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_texCoord2D = gl_MultiTexCoord0.xy;

  v_texCoord3D = gl_Vertex.xyz;
	
	v_color = gl_Color;

}

