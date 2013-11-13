#extension GL_ARB_texture_rectangle : require

uniform sampler2D InputTex;
uniform float InputCols;
uniform float InputRows;
uniform float ReduceFact;
uniform int PassCount;

uniform int OffsetX;
uniform int OffsetY;

uniform sampler2D PolyMaskTex;
uniform sampler2D ImageTex;


void main()
{
	if (PassCount == 0) {
		// Copy the texture from the ImageTex
        // Because of the way this shader is drawn, we add offset x&y..
        
        // Texture color
        vec3 col = vec3(texture2D(ImageTex, gl_FragCoord.xy + vec2(OffsetX, OffsetY)));
        
        // Mask value
        float b = texture2D(PolyMaskTex, gl_FragCoord.xy + vec2(OffsetX, OffsetY)).r;
        
        gl_FragColor = vec4(col, b);
	} else {
		vec4 sum = vec4(0);
		vec2 inputTexPos = ReduceFact * (gl_FragCoord.xy - vec2(0.5));
		
		vec2 completeOutputColsRows = floor(vec2(InputCols, InputRows) / ReduceFact);
		vec2 extraInputColsRows = vec2(InputCols, InputRows) - completeOutputColsRows * ReduceFact;
		
		vec2 range;
		range.x = ( gl_FragCoord.x < completeOutputColsRows.x ) ? ReduceFact : extraInputColsRows.x;
		range.y = ( gl_FragCoord.y < completeOutputColsRows.y ) ? ReduceFact : extraInputColsRows.y;
		
        int numNonZero = 0;
        
		for (float dx = 0.5; dx < range.x; dx += 1.0) {
			for (float dy = 0.5; dy < range.y; dy += 1.0) {
                vec4 tmpCol = texture2D(InputTex, inputTexPos + vec2(dx, dy));
                
                if (tmpCol.a > 0.0f) {
                    numNonZero += 1;
                }
            
				sum += tmpCol;
            }
        }
		
        float a = numNonZero > 0 ? 1.0f : 0.0f;
        numNonZero = numNonZero > 0 ? numNonZero : 1;
        
		gl_FragColor = vec4( vec3(sum), a) / float(numNonZero);
	}
}
