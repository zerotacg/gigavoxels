#version 430

layout (location = 0) out vec4 frag_color;

uniform sampler3D brick_texture;
uniform sampler2D test_texture;

void main()
{
    vec2 uv = floor( gl_FragCoord.xy ) / 128.0;
    uv = floor( uv ) / 4.0;
    //vec4 color = texture( brick_texture, vec3( uv, 0.25 ) );
    vec4 color = texture( test_texture, uv );
    frag_color = color;
    //frag_color = vec4( 0.0, 1.0, 0.5, 1.0 );
}
