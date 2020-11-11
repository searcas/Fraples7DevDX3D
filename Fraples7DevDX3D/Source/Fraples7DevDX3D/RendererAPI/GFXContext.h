#pragma once
#include "../GraphicAPI/Graphics.h"

namespace FraplesDev
{
    class GfxContext
    {
    public:
        virtual void Bind(Graphics& gfx) noexcept = 0;
        virtual ~GfxContext() = default;
        virtual std::string GetUID()const noexcept
        {
            assert(false);
            return "";
        }
        virtual void InitializeParentReference(const class Renderer& parent)noexcept {};

    protected:
        static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
        static ID3D11Device* GetDevice(Graphics& gfx)noexcept;
        static DxgiInfoManager& GetInfoManager(Graphics& gfx);

    };

}