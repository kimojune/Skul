// 상수 버퍼 배치는 yaGraphics.h 의 FrameCB, SpriteCB 와 같아야 한다

// 프레임마다 한 번
cbuffer Frame : register(b0)
{
    row_major matrix ViewMatrix;        // 카메라는 Camera::CaluatePos 로 이미 빼서 넘기므로 단위 행렬
    row_major matrix ProjectionMatrix;  // 화면 픽셀(왼쪽 위 0,0) -> NDC 직교 투영
    float4 Resolution;                  // xy: 화면 크기(px)
}

// 그릴 때마다
cbuffer Sprite : register(b1)
{
    row_major matrix WorldMatrix;       // 단위 사각형(0~1) -> 화면 픽셀 사각형
    float4 DestRect;                    // 화면에 그릴 사각형 (x, y, 너비, 높이) px
    float4 SourceRect;                  // 시트에서 가져올 사각형 (x, y, 너비, 높이) 텍셀
    float4 Tint;                        // premultiplied 색. 이미지 전체 알파 a 는 (a, a, a, a)
    float2 TextureSize;                 // 시트 크기 (텍셀)
    uint FlipX;                         // 1 이면 좌우 반전
    float SpritePad;
}

Texture2D albedoTexture : register(t0);
SamplerState pointSampler : register(s0);
