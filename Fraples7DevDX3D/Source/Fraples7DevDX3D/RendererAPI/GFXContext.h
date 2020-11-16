#pragma once
#include "GraphicAPI/Graphics.h"
#include "GraphicAPI/GraphicsResource.h"
#include <memory>
namespace FraplesDev
{
    class GfxContext : public GraphicsResource
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
    };
    class CloningContext : public GfxContext
    {
    public:
        virtual std::unique_ptr<CloningContext>Clone()const noexcept = 0;
    };
}
