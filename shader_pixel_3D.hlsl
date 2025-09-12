struct PS_IN
{
    float4 PosH : SV_Position;
    float4 color : COLOR0;
};
Texture2D tex;
SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{
    return pi.color;
}
