#include "RendererAPI/LayoutCodex.h"
#include "Core/MetaProgramming/DynamicConstant.h"

namespace FraplesDev
{

	namespace QA
	{


		void TestDynamicConstant()
		{
			using namespace	std::string_literals;
			{
				MP::RawLayout s;
				s.Add<MP::Struct>("butts");
				s["butts"].Add<MP::Float3>("pubes");
				s["butts"].Add<MP::Float>("dank");
				s.Add<MP::Float>("woot");
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Struct>(4);
				s["arr"].T().Add<MP::Float3>("twerk");
				s["arr"].T().Add<MP::Array>("werk");
				s["arr"].T()["werk"].Set<MP::Float>(6);
				s["arr"].T().Add<MP::Array>("meta");
				s["arr"].T()["meta"].Set<MP::Array>(6);
				s["arr"].T()["meta"].T().Set<MP::Matrix>(4);
				s["arr"].T().Add<MP::Bool>("booly");
				//fails: duplicate symbol name
				//s.Add<MP::Bool>("arr"s);
				//failes: bad symbol name
				//s.Add<MP::Bool>("69Man");

				auto b = MP::Buffer(std::move(s));
				const auto sig = b.GetRootLayoutElement().GetSignature();
				{
					auto exp = 69.0f;
					b["woot"] = exp;
					float act = b["woot"];
					assert(act == exp);
				}
				{
					auto exp = 690.0f;
					b["butts"]["dank"s] = exp;
					float act = b["butts"]["dank"s];
					assert(act == exp);
				}
				{
					auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
					b["butts"]["pubes"s] = exp;
					DirectX::XMFLOAT3 act = b["butts"]["pubes"s];
					assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT3)));
				}
				{
					DirectX::XMFLOAT4X4 exp;
					DirectX::XMStoreFloat4x4(&exp, DirectX::XMMatrixIdentity());
					b["arr"][2]["meta"s][5][3] = exp;
					DirectX::XMFLOAT4X4 act = b["arr"][2]["meta"s][5][3];
					assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT4)));

				}
				{
					auto exp = true;
					b["arr"][2]["booly"s] = exp;
					bool act = b["arr"][2]["booly"s];
					assert(act == exp);
				}
				{
					auto exp = false;
					b["arr"][2]["booly"s] = exp;
					bool act = b["arr"][2]["booly"s];
					assert(act == exp);
				}
				//exists
				{
					assert(b["butts"s]["pubes"s].Exists());
					assert(!b["butts"s]["fubar"s].Exists());
					if (auto ref = b["butts"s]["pubes"s]; ref.Exists())
					{
						DirectX::XMFLOAT3 f = ref;
						assert(f.x == 69.0f);
					}
				}
				const auto& cb = b;
				{
					DirectX::XMFLOAT4X4 act = cb["arr"][2]["meta"s][5][3];
					assert(act._11 == 1.0f);
				}
				//this doesn't compile buffer is const 
				//cb["arr"][2]["booly"s] = true;

				//this failes assertion: array out of bounds
				//cb["arr"s][200];
			}
			//size test array of arrays
			{
				MP::RawLayout s;
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Array>(6);
				s["arr"].T().Set<MP::Matrix>(4);
				MP::Buffer b = MP::Buffer(std::move(s));

				auto act = b.GetSizeInBytes();
				assert(act == 16u * 4u * 4u * 6u);
			}
			//size test array of structs with padding
			{
				MP::RawLayout s;
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Struct>(6);
				s["arr"].T().Add<MP::Float2>("a");
				s["arr"].T().Add<MP::Float3>("b");
				MP::Buffer b = MP::Buffer(std::move(s));
				auto act = b.GetSizeInBytes();
				assert(act == 16u * 2u * 6u);
			}
			//size test array of primitive that needs padding
			{
				MP::RawLayout s;
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Float3>(6);
				MP::Buffer b = MP::Buffer(std::move(s));

				auto act = b.GetSizeInBytes();
				assert(act == 16u * 6u);
			}
			// Testing cookedLayout

			{
				MP::RawLayout s;
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Float3>(6);
				auto cooked = MP::LayoutCodex::Resolve(std::move(s));

				//raw is cleared after donating
				s.Add<MP::Float>("arr");
				//failes to compile, cooked returns const&
				//cooked["arr"].Add<MP::Float>("buttman")
				auto b1 = MP::Buffer(cooked);
				b1["arr"][0] = DirectX::XMFLOAT3{ 69.0f,69.0f,60.69f };
				auto b2 = MP::Buffer(cooked);
				b2["arr"][0] = DirectX::XMFLOAT3{ 690.0f,0.0f,0.0f };
				assert(static_cast<DirectX::XMFLOAT3>(b1["arr"][0]).x == 69.0f);
				assert(static_cast<DirectX::XMFLOAT3>(b2["arr"][0]).x == 690.0f);
			}

		}
	}
}