uniform sampler2D mysampler;
in vec2 texcoord;

void main (void)  
{  vec4 color = texture2D(mysampler, texcoord);       
   
   if (color.rgb == vec3(1.0,0.0,0.0))
      discard; 
   
   gl_FragColor = color;
}    
