#version 330

uniform sampler2D texture0;
uniform vec2      lightPosition;

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

const float MIN_BRIGHTNESS = 0.05;
const float LINEAR_TERM    = 0.00005;
const float QUADRATIC_TERM = 0.000008;

float attenuate(float dist)
{
    float attenuation = 1.0 / (1.0 +
                               LINEAR_TERM    * dist +
                               QUADRATIC_TERM * dist * dist);
    if (attenuation < MIN_BRIGHTNESS)
        return MIN_BRIGHTNESS;
    return attenuation;
}

void main()
{
    float dist       = distance(lightPosition, fragPosition);
    float brightness = attenuate(dist);
    vec4  color      = texture(texture0, fragTexCoord);
    finalColor       = vec4(color.rgb * brightness, color.a);
}
