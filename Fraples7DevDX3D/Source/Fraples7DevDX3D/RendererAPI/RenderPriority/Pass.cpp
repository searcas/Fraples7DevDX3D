#include "Pass.h"
#include "RendererAPI/RenderGraph/Exception/RenderGraphCompileException.h"
#include "Utility/Fraples7Utility.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
#include <sstream>
namespace FraplesDev
{
	Pass::Pass(std::string name) noexcept
		:_mName(std::move(name))
	{

	}
	void Pass::Reset() noexcept(!IS_DEBUG)
	{
	}
	const std::vector<std::unique_ptr<PassInput>>& Pass::GetInputs() const
	{
		return _mInputs;
	}
	PassOutput& Pass::GetOutput(const std::string& registeredName) const
	{
		for (auto& out : _mOutputs)
		{
			if (out->GetName()==_mName)
			{
				return *out;
			}
		}
		std::ostringstream oss;
		oss << "Output named [" << _mName << "] not found in pass: " << GetName();

		throw RGC_EXCEPTION(oss.str());
	}
	PassInput& Pass::GetInput(const std::string& registeredName) const
	{
		for (auto& in : _mInputs)
		{
			if (in->GetRegisteredName() == registeredName)
			{
				return *in;
			}
		}
		std::ostringstream oss;
		oss << "Input named [" << registeredName << "] not found in pass: " << GetName();

		throw RGC_EXCEPTION(oss.str());
	}
	void Pass::SetInputSource(const std::string& registeredName, const std::string& target)
	{
		auto& input = GetInput(registeredName);
		auto targetSplit = Utility::SplitString(target, ".");
		if (targetSplit.size() != 2u)
		{
			throw RGC_EXCEPTION("Input target has incorrect format");
		}
		input.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}
	void Pass::RegisterInput(std::unique_ptr<PassInput> input)
	{
		// check for overlap of input names
		for (auto& in : _mInputs)
		{
			if (in->GetRegisteredName() ==input->GetRegisteredName())
			{
				throw RGC_EXCEPTION("Registered input overlaps with existing: " + input->GetRegisteredName());

			}
		}
		_mInputs.push_back(std::move(input));
	}
	void Pass::RegisterOutput(std::unique_ptr<PassOutput> output)
	{
		// check for overlap of output names
		for (auto& out : _mOutputs)
		{
			if (out->GetName() == output->GetName())
			{
				throw RGC_EXCEPTION("Registered output overlaps with existing: " + output->GetName());
			}
		}
		_mOutputs.push_back(std::move(output));
	}
	void Pass::BindBufferResources(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		if (_mRenderTarget)
		{
			_mRenderTarget->BindAsBuffer(gfx, _mDepthStencil.get());
		}
		else
		{
			_mDepthStencil->BindAsBuffer(gfx);
		}
	}
	const std::string& Pass::GetName()const noexcept
	{
		return _mName;
	}
	void Pass::Finalize()
	{
		for (auto& in : _mInputs)
		{
			in->PostLinkValidate();
		}
		for (auto& out : _mOutputs)
		{
			out->PostLinkValidation();
		}
	}
}
	