struct ComputeData
{
    float4 color;
};

RWStructuredBuffer<ComputeData> data : register(u0);
[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{

}