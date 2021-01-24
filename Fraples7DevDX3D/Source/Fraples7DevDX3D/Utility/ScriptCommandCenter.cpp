#include "ScriptCommandCenter.h"
#include <sstream>
#include <fstream>
#include "TexturePreprocessor.h"
#include "Json/nlohmann/json.hpp"
#include "QualityAssurance/TestingQA.h"
#include <filesystem>
namespace FraplesDev
{
	using namespace std::string_literals;

#define SCRIPT_ERROR(msg) Exception(__LINE__,__FILE__,scriptPath,(msg))

	ScriptCommander::ScriptCommander(const std::vector<std::string>& args)
	{

		if (args.size() >= 2 && args[0] == "--commands")
		{
			const auto scriptPath = args[1];
			std::ifstream script(scriptPath);

			if (!script.is_open())
			{
				throw SCRIPT_ERROR("Unable to open script file"s);
			}
			nlohmann::json top;
			script >> top;

			if (top.at("enabled"))
			{
				bool abort = false;
				for (const auto& j : top.at("commands"))
				{
					const auto commandName = j.at("command").get<std::string>();
					const auto params = j.at("params");
					if (commandName == "flip-y")
					{
						const auto source = params.at("source");
						cmd::TexturePreprocessor::FlipYNormalMap(source, params.value("dest", source));
						abort = true;
					}
					else if (commandName == "flip-y-obj")
					{
						cmd::TexturePreprocessor::FlipYAllNormalMapsInObj(params.at("source"));
						abort = true;
					}
					else if (commandName == "validate-nmap")
					{
						cmd::TexturePreprocessor::ValidateNormalMap(params.at("source"), params.at("min"), params.at("max"));
						abort = true;
					}
					else if (commandName == "makes-stripes")
					{
						cmd::TexturePreprocessor::MakeStripes(params.at("dest"), params.at("size"), params.at("stripeWidth"));
						abort = true;
					}
					else if (commandName == "publish")
					{
						Publish(params.at("dest"));
						abort = true;
					}
					else if (commandName == "run-tests")
					{
						QA::TestDynamicMeshLoading();
						QA::TestDynamicConstant();
						QA::TestScaleMatrixTranslation();
						QA::TestNumpy();
						abort = true;
					}
					else
					{
						throw SCRIPT_ERROR("Unknown Cmmand: "s + commandName);
					}
					if (abort)
					{
						throw Completion("Command(s) completed successfully.");
					}
				}
			}
		}
	}
	void ScriptCommander::Publish(std::string path) const
	{
		std::filesystem::create_directory(path);
		//copy executable
		std::filesystem::copy_file(R"(Fraples7DevDX3D.exe)", path + R"(\Fraples7DevDX3D.exe)", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("..\\..\\Fraples7DevDX3D\\imgui_default.ini", path + R"(\imgui_default.ini)", std::filesystem::copy_options::overwrite_existing);

		// copy all dlls
		for (auto& p : std::filesystem::directory_iterator(std::filesystem::current_path()))
		{
			if (p.path().extension() == L".dll")
			{
				std::filesystem::copy_file(p.path(), path + "\\" + p.path().filename().string(), std::filesystem::copy_options::overwrite_existing);
			}
		}
		// copy compiled shaders
		std::filesystem::copy("..\\..\\Fraples7DevDX3D\\Shaders", path + R"(\Shaders)", std::filesystem::copy_options::overwrite_existing);
		// copy assets
		std::filesystem::copy("..\\..\\Fraples7DevDX3D\\Images", path + R"(\Images)", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		std::filesystem::copy("..\\..\\Fraples7DevDX3D\\Models", path + R"(\Models)", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);


	}
	ScriptCommander::Completion::Completion(const std::string& content )noexcept
		:FraplesException(69,"@ScriptCommanderAbort"),_mContent(content)
	{

	}
	const char* ScriptCommander::Completion::what()const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << std::endl << _mContent;
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* ScriptCommander::Completion::GetType()const noexcept
	{
		return "Script Command Completed";
	}
	ScriptCommander::Exception::Exception(int line, const char* file, const std::string& script, const std::string& message)noexcept
		:FraplesException(69, "@ScriptCommanderAbort"),script(script),message(message)
	{

	}
	const char* ScriptCommander::Exception::what()const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << "[Script File]" << script << std::endl << message;
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* ScriptCommander::Exception::GetType()const noexcept
	{
		return "Script Command Error";
	}
}