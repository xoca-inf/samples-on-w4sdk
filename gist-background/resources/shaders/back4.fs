uniform sampler2D texture0;
varying vec2 v_TexCoord;

//based on https://www.shadertoy.com/view/4dBSRK + https://www.shadertoy.com/view/lsBXDW

void w4_main()
{
   vec2 px = 16.0*v_TexCoord;
   px.y *= w4_u_resolution.y/w4_u_resolution.x;

   float id = 0.5 + 0.5*cos(w4_u_time + sin(dot(floor(px+0.5),vec2(113.1,17.81)))*43758.545);

   vec3  co = 0.5 + 0.5*cos(w4_u_time + 3.5*id + vec3(0.0,1.57,3.14) );

   vec2  pa = smoothstep( 0.0, 0.2, id*(0.5 + 0.5*cos(6.2831*px)) );

   gl_FragColor = vec4( co*pa.x*pa.y, 1.0 );

}
