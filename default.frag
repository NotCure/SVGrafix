// default.frag
#version 330 core
out     vec4 FragColor;
uniform vec4 uFill;    // set from your SceneBuilder

void main() {
    FragColor = uFill;
}
