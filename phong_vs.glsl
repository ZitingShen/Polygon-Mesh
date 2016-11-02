#version 150

in vec4 vPosition;
in vec3 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 Projection;
uniform mat4 Transformation;

void main(){
    vec4 pos = ModelView * Transformation* vPosition;
    vec4 pos_light = ModelView * LightPosition;
	fN = (Transformation * ModelView * vec4(vNormal, 0.0)).xyz;
	fE = -pos.xyz;
	fL = pos_light.xyz;
	if( pos_light.w != 0.0 ) {
		fL = pos_light.xyz - pos.xyz;
	}
	gl_Position = Projection*pos;
}