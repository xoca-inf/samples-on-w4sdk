varying vec3 v_normal;
varying vec3 v_toView;
varying vec3 v_toLight;

uniform sampler2D texture0;
varying vec2 v_TexCoord;

void w4_main()
{
    vec4 diffColor = texture2D(texture0, v_TexCoord);
    const vec4 specColor = vec4 ( 1.0, 1.0, 1.0, 1.0 );
    const float specPower = 5.0;
    vec3 n2 = normalize ( v_normal );
    vec3 v2 = normalize ( v_toView );
    vec3 r  = reflect   ( -v2, n2 );
    vec3 l2 = normalize ( v_toLight );
    vec4 diff = diffColor * max ( dot ( n2, l2 ), 0.0 );
    vec4 spec = specColor * pow ( max ( dot ( l2, r ), 0.0 ), specPower );
    gl_FragColor = vec4(diff.rgb, diffColor.a);
    gl_FragColor.rgb += spec.rgb;
}
