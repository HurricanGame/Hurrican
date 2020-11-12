/* Input */
in vec4 v_Color;
in vec2 v_Texcoord0;
uniform sampler2D u_Texture0;
uniform int u_WindowHeight;
/* Output */
out vec4 v_FragColor;

void main()
{
    /* Sample the current texture */
    vec4 p = texture( u_Texture0, v_Texcoord0 ) * v_Color;
    /* add scanlines */
    float r = float(u_WindowHeight / 216);
    float t = float(u_WindowHeight / 360);
    if (mod(gl_FragCoord.y, r) < t)
        v_FragColor = p;
    else
        v_FragColor = vec4(p.rgb * 0.5, p.a);
}
