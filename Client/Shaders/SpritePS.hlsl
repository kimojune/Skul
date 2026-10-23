#include "globals.hlsli"

struct VSOut
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
};

float4 main(VSOut In) : SV_TARGET
{
    // 사각형 안에서 이 픽셀이 몇 번째인지 (SV_Position 은 픽셀 가운데 x.5)
    int2 pixel = int2(floor(In.Pos.xy - DestRect.xy));
    int2 destSize = int2(DestRect.zw);
    if (FlipX != 0)
        pixel.x = destSize.x - 1 - pixel.x;

    // 늘리거나 줄여 그릴 때 GDI StretchBlt 와 같은 규칙으로 원본 텍셀을 고른다: floor((2k + 1) * 원본 / (2 * 대상))
    // uv 를 보간해서 고르면 GPU 의 서브텍셀 정밀도 때문에 경계에 가까운 줄이 한 칸 밀릴 수 있다
    int2 texel = int2(SourceRect.xy) + ((2 * pixel + 1) * int2(SourceRect.zw)) / (2 * destSize);

    // 텍셀 가운데를 점 샘플링. 텍스처는 premultiplied BGRA, 마젠타 키 색은 올릴 때 (0, 0, 0, 0) 으로 바꿔 두었다
    float4 color = albedoTexture.Sample(pointSampler, (texel + 0.5f) / TextureSize);
    color *= In.Color;

    // 알파가 0 인 픽셀은 버린다. GDI AlphaBlend 도 알파 0 픽셀은 색이 있어도 건너뛴다
    if (color.a <= 0.0f)
        discard;

    return color;
}
