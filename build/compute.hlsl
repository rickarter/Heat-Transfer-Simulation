struct ComputeData
{
    float4 color;
};

RWStructuredBuffer<ComputeData> data : register(u0);
[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // data[1].color.y = data[1].color.y / 2;
    //data[1].color.y += 2.0f;
    //
    //data[1].color.y = 0.3f;
    //data[1].color.y = 0 == 0;
}