uniform sampler2D u_depthTexture;

varying vec2 v_textureCoordinates;

void main(void)
{
    float depth = texture2D(u_depthTexture, v_textureCoordinates).r;
    vec3 color = texture2D(u_depthTexture, v_textureCoordinates).rgb;
    gl_FragColor = vec4(vec3(depth), 1.0);
}
