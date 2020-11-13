#pragma once
#include "../GraphicAPI/Graphics.h"
#include <memory>
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
        virtual void Accept(class TechniqueProbe&) {};
    protected:
        static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
        static ID3D11Device* GetDevice(Graphics& gfx)noexcept;
        static DxgiInfoManager& GetInfoManager(Graphics& gfx);

    };
    class CloningContext : public GfxContext
    {
    public:
        virtual std::unique_ptr<CloningContext>Clone()const noexcept = 0;
    };
}
