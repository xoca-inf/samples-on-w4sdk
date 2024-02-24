uniform samplerCube cubemap0;

varying vec2 vUV;
varying vec3 vN;
varying vec3 vP;
varying vec3 vE;
varying vec3 v_toView;

const float shininess = 32.0;

float large_waveheight      = 0.50; // change to adjust the "heavy" waves
float large_wavesize        = 4.;  // factor to adjust the large wave size
float small_waveheight      = .6;  // change to adjust the small random waves
float small_wavesize        = .5;   // factor to ajust the small wave size
vec3 watercolor             = vec3(0.33, 0.45, 0.56); // deep-water color (RGB, should be darker than the low-water color)

// calculate random value
float hash( float n )
{
    return fract(sin(n)*43758.5453123);
}

// 2d noise function
float noise( in vec2 x )
{
    vec2 p  = floor(x);
    vec2 f  = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);
}

const mat2 m = mat2( 0.72, -1.60,  1.60,  0.72 );

float water_map( vec2 p, float height )
{
    vec2 p2 = p*large_wavesize;
    vec2 shift1 = 0.001*vec2( w4_u_time*160.0*2.0, w4_u_time*120.0*2.0 );
    vec2 shift2 = 0.001*vec2( w4_u_time*190.0*2.0, -w4_u_time*130.0*2.0 );

    float f = 0.6000*noise( p );
    f += 0.2500*noise( p*m );
    f += 0.1666*noise( p*m*m );
    float wave = sin(p2.x*0.622+p2.y*0.622+shift2.x*4.269)*large_waveheight*f*height*height ;

    p *= small_wavesize;
    f = 0.;
    float amp = 1.0, s = .5;
    for (int i=0; i<9; i++)
    {
        p = m*p*.947;
        f -= amp*abs(sin((noise( p+shift1*s )-.5)*2.));
        amp = amp*.59;
        s*=-1.329;
    }

    return wave+f*small_waveheight;
}

void w4_userLightModel(inout vec3 results[2], vec3 vectorToLight, vec3 vectorToEye, vec3 N, vec3 lColor, float shadowFactor, float fading)
{
    w4_BlinnLightModel(results, vectorToLight, vectorToEye, N, lColor, fading, shadowFactor, shininess);
}

void w4_main()
{
	vec2 uv = vUV;

    vec3 col = vec3(0., 0., 0.);
    float waveheight = 1.5;

    vec2 dif = vec2(.0, .01);
    vec2 pos = vec2(uv.x*15., uv.y*150.) + vec2(w4_u_time*.01);
    float h1 = water_map(pos-dif,waveheight);
    float h2 = water_map(pos+dif,waveheight);
    float h3 = water_map(pos-dif.yx,waveheight);
    float h4 = water_map(pos+dif.yx,waveheight);
    vec3 N = normalize(vec3(h3-h4, .125, h1-h2));

    vec3 v2 = normalize ( v_toView );
    vec3 r  = reflect   ( -v2, N );

    vec3 lightFactors[2];
    w4_calculateLightFactor(lightFactors, vP, vE, N);
    vec3 color = lightFactors[0] + lightFactors[1];

    vec4 reflectColor = textureCube(cubemap0, r) * 0.5;

	gl_FragColor = vec4(color + reflectColor.rgb, 1.0);
}
