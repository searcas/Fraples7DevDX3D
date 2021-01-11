#pragma once
#include "GraphicAPI/GraphicsResource.h"
#include <assert.h>
#include <string>
#include <memory>


namespace FraplesDev
{
    class GfxContext : public GraphicsResource
    {
    public:
        virtual ~GfxContext() = default;
        virtual std::string GetUID()const noexcept
        {
            assert(false);
            return "";
        }
        virtual void InitializeParentReference(const class Renderer& parent)noexcept {};
        virtual void Accept(class TechniqueProbe&) {};
        virtual void Bind(Graphics& gfx) noexcept = 0;
    };

    class CloningContext : public GfxContext
    {
    public:
        virtual std::unique_ptr<CloningContext>Clone() const noexcept = 0;
    };
}
