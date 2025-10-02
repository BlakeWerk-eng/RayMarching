#version 330 core

out vec4 FragColor;

uniform vec2 iResolution;
uniform float iTime;
uniform vec3 viewPos;
uniform vec3 Front;
uniform vec3 Right;
uniform vec3 Up;
uniform sampler2D matcap;
uniform vec2 mouse;
float posY;

vec3 palette(float t) {
	vec3 a = vec3(0.5, 0.5, 0.5);
	vec3 b = vec3(0.5, 0.5, 0.5);
	vec3 c = vec3(1.0, 1.0, 1.0);
	vec3 d = vec3(0.263, 0.416, 0.557);

	return a + b*cos( 6.28318*(c*t*d) );
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec2 getmatcap(vec3 eye, vec3 normal) {
  vec3 reflected = reflect(eye, normal);
  float m = 2.8284271247461903 * sqrt( reflected.z+1.0 );
  return reflected.xy / m + 0.5;
}

vec3 rotate(vec3 v, vec3 axis, float angle) {
	mat4 m = rotationMatrix(axis, angle);
	return (m * vec4(v, 1.0)).xyz;
}

float sdSphere(vec3 p, float s) {
	return length(p) - s;
}

float sdBox(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x,max(q.y,q.z)), 0.0);
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}


float opRepetition( in vec3 p, in vec3 s )
{
   vec3 q = p - s*round(p/s); 
   return sdTorus(q, vec2(0.5, 0.2)); 
}


float opTwist(in vec3 p )
{
    const float k = 2.0; // or some other amount
    float c = cos(k*p.y);
    float s = sin(k*p.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xz,p.y);
    return sdTorus(q, vec2(0.5, 0.2));
}


float smin(float a, float b, float k) {
	float h = max(k-abs(a-b), 0.0)/k;
	return min(a,b) - h*h*h*k*(1.0/6.0);
}

mat2 rot2D(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	return mat2(c, -s, s, c);
}

float map(vec3 p) {

	vec3 p1 = rotate(p, vec3(1.,1.0,1.), 0.0);	// Rotate 3D

	vec3 spherePos = vec3(0, 0, 0);    // Sphere position

	float sphere = sdSphere(p, 50.0);   // Sphere SDF
	
	float ground = p.y + .75;	// Ground SDF

	float box = sdBox(vec3(p.x,p.y,p.z), vec3(10.0, 5.0, 7.0)); // Cube SDF

	float goal = sdBox(vec3(p.x + 10,p.y + 4,p.z), vec3(2.0,1.0, 2.5));
	
	float goal2 = sdBox(vec3(p.x - 10,p.y + 4,p.z), vec3(2.0,1.0, 2.5));

	float r = opTwist(p);

	// Closest distance to the scene
	return max(-goal2, max(-goal, -box));
}

vec3 getNormal(vec3 p) {
	const float eps = 0.0001;
	const vec2 h = vec2(eps,0);
	return normalize(vec3(map(p+h.xyy) - map(p-h.xyy),
			      map(p+h.yxy) - map(p-h.yxy),
			      map(p+h.yyx) - map(p-h.yyx)
	));
}

vec3 getLight(vec3 p, vec3 camera) {
	vec3 lightPos = vec3(0.0, 0.0, 0.0);
	vec3 L = normalize(camera - p);
	vec3 N = getNormal(p);
	return N * L * 20;
}


void main() {
	vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution.xy)
		/ iResolution.y;

	vec3 camera = vec3(viewPos.x, viewPos.y,  viewPos.z);


	// Initialization
	vec3 ro = camera;	// ray origin
	vec3 rd = mat3(Right, Up, Front) * normalize(vec3(uv, 1)); // ray direction
	vec3 col = vec3(0);

	float t = 0; // total distance travelled



	// Raymarching
	for(int i=0; i < 256; ++i) {
		vec3 p = ro + rd * t;	// position along the ray


		float d = map(p);	// current distnace to the scene


		if (d < .0001 || t > 100.) break;   // early stop 
		
		
		t += d;			// "march" the ray
	}



	// Coloring
	if(t<100.) {
		
		vec3 pos = ro + rd * t;

	//	vec2 matcapUV = getmatcap(rd, normal);
		col = getLight(pos, camera);
		//col = texture2D(matcap, matcapUV).rgb;

	}


	FragColor = vec4(col, 0.0);
}
