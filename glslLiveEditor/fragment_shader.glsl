#ifdef GL_ES
precision highp float;
#endif

uniform vec3 unResolution;
uniform float time;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D fft;
uniform vec4 unPar;
uniform vec4 unPos;
uniform vec3 unBeatBassFFT;

void main() {
  vec3 col = vec3(sin(gl_FragCoord.x / 100.0)
                * cos(gl_FragCoord.y / 100.0) * sin(time*10.0));
  
  gl_FragColor = vec4(col, 1.0);
}

