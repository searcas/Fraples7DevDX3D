#pragma once
#include <string>
#include <vector>
#include <array>
#include "RendererAPI/RenderGraph/PassInput.h"
#include "RendererAPI/RenderGraph/PassOutput.h"
#include "GraphicAPI/Graphics.h"
#include "Job.h"
namespace FraplesDev
{
	class Pass
	{
	public:
		Pass(std::string name)noexcept;
		virtual ~Pass() = default;
		void Reset()noexcept(!IS_DEBUG);
		const std::string& GetName()const noexcept;
		virtual void Finalize();
		void Accept(Job job)noexcept;
		virtual void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) = 0;
		virtual void Reset()noexcept(!IS_DEBUG);
		const std::vector<std::unique_ptr<PassInput>>& GetInputs()const;
		PassOutput& GetOutput(const std::string& registeredName)const;
		PassInput& GetInput(const std::string& registeredName)const;
		void SetInputSource(const std::string& registeredName, const std::string& target);
		void RegisterInput(std::unique_ptr<PassInput>input);
		void RegisterOutput(std::unique_ptr<PassOutput>&ouput);
		void BindBufferResources(Graphics& gfx )const noexcept(!IS_DEBUG);
		std::shared_ptr<RenderTarget>_mRenderTarget;
		std::shared_ptr<DepthStencil>_mDepthStencil;
	private:
		std::vector<std::unique_ptr<PassInput>>_mInputs;
		std::vector<std::unique_ptr<PassOutput>>_mOutputs;
		std::string _mName;
	};
}