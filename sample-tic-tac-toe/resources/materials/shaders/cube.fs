uniform sampler2D texture0;
uniform vec3 mainColor;

varying vec3 v_toView;
varying vec3 v_normal;
varying vec3 v_toLight;

varying vec2 vTexCoord;

// thanks to https://www.shadertoy.com/view/3s3GDn
float getGlow(float dist, float radius, float intensity){
		return pow(radius/dist, intensity);
	}

void w4_main()
{
   const vec2 center = vec2(0.5,0.5);

   vec2 uv = vTexCoord;

   vec2 pos = center - uv;

   float glow = getGlow(1./length(pos), 0.1, 0.5);

   float r = sqrt(pos.x*pos.x + pos.y*pos.y);
   float z = 1.0 + sin(r);

   vec3 diff = mainColor;
   diff *= z;

//   diff *= glow;

   // See comment by P_Malin
   diff = 1. - exp( -diff );

   vec3 n2 = normalize ( v_normal );
   vec3 l2 = normalize ( v_toLight );
   diff *= max ( dot ( n2, l2 ), 0.3 );

   // bottom-left
   vec2 bl = step(vec2(0.02),uv);
   float pct = bl.x * bl.y;

   // top-right
    vec2 tr = step(vec2(0.02),1.0-uv);
    pct *= tr.x * tr.y;

//   diff *= pct;
   diff += (1. - pct) * mainColor;

   diff += vec3(1., uv.y, 0.) * abs(cos(w4_u_time)*0.25);

   gl_FragColor = vec4(diff, 1.);

}
