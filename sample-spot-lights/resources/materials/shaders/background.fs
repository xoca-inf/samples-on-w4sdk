uniform sampler2D texture0;

varying vec2 VTexCoords;

void w4_main()
{
    vec2 textureOffset = vec2(-0.5);
    float rotation = w4_u_time;
    float zoom = 0.5;

    vec2 uv = VTexCoords.xy;
    uv = vec2((uv.x - 0.5) * (w4_u_resolution.x / w4_u_resolution.y), uv.y - 0.8);
    uv *= (2. - zoom) / 2.;
    uv *= mat2(cos(rotation), sin(rotation), -sin(rotation), cos(rotation));
    vec4 color = texture2D(texture0, uv - textureOffset);
    gl_FragColor = color;
}