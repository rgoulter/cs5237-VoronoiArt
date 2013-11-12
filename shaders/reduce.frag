#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect InputTex;
uniform float InputCols;
uniform float InputRows;
uniform float ReduceFact;
uniform int PassCount;


void main()
{
	if ( PassCount == 0 )
	{
		float m = texture2DRect( InputTex, gl_FragCoord.xy ).a;
		gl_FragColor = vec4( sin( m ) );
	}
	else
	{
		float sum = 0.0;
		vec2 inputTexPos = ReduceFact * ( gl_FragCoord.xy - vec2( 0.5 ) );
		
		vec2 completeOutputColsRows = floor( vec2( InputCols, InputRows ) / ReduceFact );
		vec2 extraInputColsRows = vec2( InputCols, InputRows ) - completeOutputColsRows * ReduceFact;
		
		vec2 range;
		range.x = ( gl_FragCoord.x < completeOutputColsRows.x ) ? ReduceFact : extraInputColsRows.x;
		range.y = ( gl_FragCoord.y < completeOutputColsRows.y ) ? ReduceFact : extraInputColsRows.y;
		
		for ( float dx = 0.5; dx < range.x; dx += 1.0 )
			for ( float dy = 0.5; dy < range.y; dy += 1.0 )
				sum += texture2DRect( InputTex, inputTexPos + vec2( dx, dy ) ).a;
		
		gl_FragColor = vec4( sum );
	}
}
