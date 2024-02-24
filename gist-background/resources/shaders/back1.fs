const float sp= 0.3;		//speed of movement

//based on https://www.shadertoy.com/view/XlfSzN

#define pi 3.141592653589793238462643383279
const float B = 4./pi;
const float C = -4./(pi*pi);

uniform sampler2D texture0;
varying vec2 v_TexCoord;

vec2 sine(vec2 x)
{
   x = fract( (x+pi)/(2.*pi) )*(2.*pi)-pi;
   return B*x + C*x*abs(x);
}

float sine(float x)
{
   x = fract( (x+pi)/(2.*pi) )*(2.*pi)-pi;
   return B*x + C*x*abs(x);
}

vec2 cosine( vec2 x )
{
   return sine( x+(pi/2.) );
}

vec3 rgbplasma(vec2 uv, float w4_u_time)
{
   uv+=0.3*sine(4.0*uv.yx+w4_u_time);
   uv+=0.2*sine(10.0*uv.yx+w4_u_time);
   uv+=0.15*sine(20.0*uv.yx+w4_u_time);
   uv+=0.1*sine(30.0*uv.yx+w4_u_time);

   return vec3(cosine(uv), sine(uv.y+uv.x));
}

void w4_main()
{
   vec2 uv = v_TexCoord-0.5;
   gl_FragColor = vec4(rgbplasma(4.*uv,w4_u_time*sp), 1.0);

}

