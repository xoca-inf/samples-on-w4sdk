uniform samplerCube cubemap0;
uniform vec3 color;

varying vec3 vN;
varying vec3 vE;

void w4_main()
{
    vec3 normal = normalize(vN);
    vec3 incident = normalize(-vE);
    vec3 reflectDir = normalize(reflect(incident, normal));
    vec3 reflectColor = textureCube(cubemap0, reflectDir).xyz;

    gl_FragColor = vec4(reflectColor * color, 1.0);
}
