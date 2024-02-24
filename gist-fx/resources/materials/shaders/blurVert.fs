uniform sampler2D texture0;

varying vec2 vUv;

uniform vec3 offset; // = float[](0.0, 1.3846153846, 3.2307692308);
uniform vec3 weight; // = float[](0.2270270270, 0.3162162162, 0.0702702703);

void w4_main(void) {
    vec2 pxSize = 1.0 / w4_u_resolution;
    gl_FragColor = texture2D(texture0, vUv) * weight[0];

    gl_FragColor += texture2D(texture0, (vUv + vec2(0.0, offset.y * pxSize.y))) * weight.y;
    gl_FragColor += texture2D(texture0, (vUv - vec2(0.0, offset.y * pxSize.y))) * weight.y;

    gl_FragColor += texture2D(texture0, (vUv + vec2(0.0, offset.z * pxSize.y))) * weight.z;
    gl_FragColor += texture2D(texture0, (vUv - vec2(0.0, offset.z * pxSize.y))) * weight.z;
}
