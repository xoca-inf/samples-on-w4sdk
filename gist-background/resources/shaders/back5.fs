uniform sampler2D texture0;
varying vec2 v_TexCoord;

const vec2 iResolution = vec2(720., 1280.);

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.


// See here for more information on smooth iteration count:
//
// http://iquilezles.org/www/articles/mset_smooth/mset_smooth.htm


float mandelbrot( in vec2 c )
{
    #if 1
    {
        float c2 = dot(c, c);
        // skip computation inside M1 - http://iquilezles.org/www/articles/mset_1bulb/mset1bulb.htm
        if( 256.0*c2*c2 - 96.0*c2 + 32.0*c.x - 3.0 < 0.0 ) return 0.0;
        // skip computation inside M2 - http://iquilezles.org/www/articles/mset_2bulb/mset2bulb.htm
        if( 16.0*(c2+2.0*c.x+1.0) - 1.0 < 0.0 ) return 0.0;
    }
        #endif


    const float B = 256.0;
    float l = 0.0;
    vec2 z  = vec2(0.0);
    for( int i=0; i<512; i++ )
    {
        z = vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y ) + c;
        if( dot(z,z)>(B*B) ) break;
        l += 1.0;
    }

    if( l>511.0 ) return 0.0;

    // ------------------------------------------------------
    // smooth interation count
    //float sl = l - log(log(length(z))/log(B))/log(2.0);

    // equivalent optimized smooth interation count
    float sl = l - log2(log2(dot(z,z))) + 4.0;

    float al = smoothstep( -0.1, 0.0, sin(0.5*6.2831*w4_u_time ) );
    l = mix( l, sl, al );

    return l;
}

void w4_main()
{
//    vec3 col = vec3(0.0);
    vec3 col = vec3(.0,.0,.3);

    vec2 p = (2.0 * v_TexCoord) - 1.0;

    float zoo = 0.62 + 0.38*cos(.07*w4_u_time);
    float coa = cos( 0.15*(1.0-zoo)*w4_u_time );
    float sia = sin( 0.15*(1.0-zoo)*w4_u_time );
    zoo = pow( zoo,8.0);

    vec2 xy = vec2( p.x*coa-p.y*sia, p.x*sia+p.y*coa);

    vec2 c = vec2(-.745,.186) + xy*zoo;

    float l = mandelbrot(c);

    col += 0.5 + 0.5*cos( 3.0 + l*0.15 + vec3(.5,.34,.21));

    gl_FragColor = vec4( col, 1.0 );
}