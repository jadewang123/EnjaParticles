uniform sampler2D depthTex; // the texture with the scene you want to blur
uniform vec2 blurDir;
uniform float sig_range;
uniform float sig;
const float pi = 3.141592654;
const float maxDepth = 0.9999999;

void main(void)
{
	float depth = texture2D(depthTex,gl_TexCoord[0].xy).x;
	if(depth>maxDepth)
	{
		discard;
	}
	float wsum = 0.0;
    float sum = 0.0;	
    float denom1 = (1/(2.*sig*sig));
    float denom2 = (1/(2*sig_range*sig_range));
    //float gauss =(1./(sqrt(2.*pi)*sig));
	int width = int(2.*sig)+1;

	for(int x=-width; x<=width; x++)
	{
		float sample = texture2D(depthTex,gl_TexCoord[0].xy+x*blurDir).x;
		
		float w = exp(-float(x*x)/denom1);

		float r2 = (sample - depth);
		float g = exp(-(r2*r2)/denom2);
		sum += sample * w * g;
		wsum += w * g;
	}
	if(wsum > 0.0)
	{
		sum /= wsum;
	}

	gl_FragData[0] = vec4(sum,sum,sum,1.0);
	gl_FragDepth=sum;
}
