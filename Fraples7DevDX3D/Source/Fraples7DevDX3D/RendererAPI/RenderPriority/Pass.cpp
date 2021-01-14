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
	const std::vector<std::unique_ptr<Sync>>& Pass::GetSyncs() const
	{
		return _mSyncs;
	}
	Source& Pass::GetSource(const std::string& registeredName) const
	{
		for (auto& out : _mSources)
		{
			if (out->GetName() == registeredName)
			{
				return *out;
			}
		}
		std::ostringstream oss;
		oss << "Output named [" << _mName << "] not found in pass: " << GetName();

		throw RGC_EXCEPTION(oss.str());
	}
	Sync& Pass::GetSync(const std::string& registeredName) const
	{
		for (auto& in : _mSyncs)
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
	void Pass::SetSyncLinkage(const std::string& registeredName, const std::string& target)
	{
		auto& input = GetSync(registeredName);
		auto targetSplit = Utility::SplitString(target, ".");
		if (targetSplit.size() != 2u)
		{
			throw RGC_EXCEPTION("Input target has incorrect format");
		}
		input.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}
	void Pass::RegisterSync(std::unique_ptr<Sync> input)
	{
		// check for overlap of input names
		for (auto& in : _mSyncs)
		{
			if (in->GetRegisteredName() ==input->GetRegisteredName())
			{
				throw RGC_EXCEPTION("Registered input overlaps with existing: " + input->GetRegisteredName());
			}
		}
		_mSyncs.push_back(std::move(input));
	}
	void Pass::RegisterSource(std::unique_ptr<Source> output)
	{
		// check for overlap of output names
		for (auto& out : _mSources)
		{
			if (out->GetName() == output->GetName())
			{
				throw RGC_EXCEPTION("Registered output overlaps with existing: " + output->GetName());
			}
		}
		_mSources.push_back(std::move(output));
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
		for (auto& in : _mSyncs)
		{
			in->PostLinkValidate();
		}
		for (auto& out : _mSources)
		{
			out->PostLinkValidation();
		}
	}
}
	