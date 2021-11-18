attribute vec3 vertPosition;
attribute vec3 vertColor;
attribute vec3 normalVert;
attribute vec2 vertTexture;
uniform mat4 mCube;
uniform mat4 mView;
uniform mat4 mProj;
varying vec3 fragColor;
varying vec3 normal;
varying vec3 fragPos;
varying vec2 fragTexture;
void main()
{
	fragColor=vertColor;
	fragTexture=vertTexture;
	//normal=mat3(transpose(inverse(mView)))*normalVert;
	normal=vec3(mCube*vec4(normalVert, 1.0));
	fragPos = vec3(mCube*vec4(vertPosition, 1.0));
	gl_Position = mProj*mView*mCube*vec4(vertPosition,1.0);
}