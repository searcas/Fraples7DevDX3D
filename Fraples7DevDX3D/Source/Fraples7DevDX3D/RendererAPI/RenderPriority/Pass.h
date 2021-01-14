#pragma once
#include <string>
#include <vector>
#include <array>
#include "RendererAPI/RenderGraph/Sync.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "GraphicAPI/Graphics.h"
#include "Job.h"
namespace FraplesDev
{
	class DepthStencil;
	class Pass
	{
	public:
		Pass(std::string name)noexcept;
		virtual ~Pass() = default;
		const std::string& GetName()const noexcept;
		virtual void Finalize();
		virtual void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) = 0;
		virtual void Reset()noexcept(!IS_DEBUG);
		const std::vector<std::unique_ptr<Sync>>& GetSyncs()const;
		Source& GetSource(const std::string& registeredName)const;
		Sync& GetSync(const std::string& registeredName)const;
		void SetSyncLinkage(const std::string& registeredName, const std::string& target);
		void RegisterSync(std::unique_ptr<Sync>input);
		void RegisterSource(std::unique_ptr<Source>ouput);
	private:
		std::vector<std::unique_ptr<Sync>>_mSyncs;
		std::vector<std::unique_ptr<Source>>_mSources;
		std::string _mName;
	};
}