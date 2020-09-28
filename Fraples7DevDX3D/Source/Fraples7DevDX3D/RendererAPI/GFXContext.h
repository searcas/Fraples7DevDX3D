#pragma once
#include "../GraphicAPI/Graphic.h"

namespace FraplesDev
{
    class GfxContext
    {
    public:
        virtual void Bind(Graphics& gfx) noexcept = 0;
        virtual ~GfxContext() = default;

    protected:
        static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
        static ID3D11Device* GetDevice(Graphics& gfx)noexcept;
        static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept;

    };

}