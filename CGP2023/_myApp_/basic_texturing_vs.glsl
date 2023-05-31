#version 430 core							
								
layout (location = 0) in vec3 pos;	
layout (location = 1) in vec2 texCoord;		
layout (location = 2) in vec3 normal;

uniform mat4 model;						
																	
out vec2 vsTexCoord;						
											
void main(void)								
{											
	gl_Position = model*vec4(pos, 1.0);	
															
	vsTexCoord = texCoord;					
}											