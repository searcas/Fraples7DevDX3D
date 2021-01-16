#include "TestingQA.h"
#include "RendererAPI/LayoutCodex.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "Core/MetaProgramming/Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "RendererAPI/Mesh/Mesh.h"
#include "RendererAPI/Mesh/Material.h"
#include "Core/Math/FraplesXM.h"
#include "GraphicAPI/Graphics.h"
#include "Platform/Windows/Window.h"
#include "RendererAPI/RenderTarget.h"	
#include "Core/Surface.h"
#include <algorithm>
#include <array>
namespace FraplesDev
{
	namespace QA
	{
		void TestMaterialSystemLoading(Graphics& gfx)
		{
			std::string path = "Models\\brick_wall\\brick_wall.obj";
			Assimp::Importer imp;
			const auto pScene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
			Material mat{ gfx,*pScene->mMaterials[1],path };
			Mesh mesh{ gfx,mat,*pScene->mMeshes[0] };
		}
		void TestScaleMatrixTranslation()
		{
			auto tlMat = DirectX::XMMatrixTranslation(20.0f, 30.0f, 40.0f);
			tlMat = ScaleTranslation(tlMat, 0.1f);
			DirectX::XMFLOAT4X4 f4;
			DirectX::XMStoreFloat4x4(&f4, tlMat);
			auto etl = ExtractTranslation(f4);
			assert(etl.x == 2.0f && etl.y == 3.0f && etl.z == 4.0f);
		}
		void D3DTestScratchPad(Window& wn)
		{
			const auto RenderWithVS = [&gfx = wn.GetGFX()](const std::string& vsName)
			{
				const auto bitop = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				const auto layout = MP::VertexLayout{}.Append(MP::ElementType::Position2D).Append(MP::ElementType::Float3Color);

				MP::VertexBuffer vb(layout, 3);
				vb[0].Attr<MP::ElementType::Position2D>() = DirectX::XMFLOAT2{ 0.0f,0.5f };
				vb[0].Attr<MP::ElementType::Float3Color>() = DirectX::XMFLOAT3{ 1.0f,0.0f,0.0f, };
				vb[1].Attr<MP::ElementType::Position2D>() = DirectX::XMFLOAT2{ 0.5, -0.5f };
				vb[1].Attr<MP::ElementType::Float3Color>() = DirectX::XMFLOAT3{ 0.0f,1.0f,0.0f };
				vb[2].Attr<MP::ElementType::Position2D>() = DirectX::XMFLOAT2{ -0.5,-0.5 };
				vb[2].Attr < MP::ElementType::Float3Color>() = DirectX::XMFLOAT3{ 0.0f,0.0f,1.0f };
				const auto bivb = VertexBuffer::Resolve(gfx, "##?", vb);

				const std::vector<unsigned short> idx = { 0, 1, 2 };

				const auto biidx = IndexBuffer::Resolve(gfx, "##?", idx);
				const auto bips = PixelShader::Resolve(gfx, "Test_PS.cso");
				const auto bivs = VertexShader::Resolve(gfx, vsName);
				const auto bilay = InputLayout::Resolve(gfx, layout, *bivs);

				auto rt = ShaderInputRenderTarget{ gfx,1280,720,0 };
				biidx->Bind(gfx);
				bivb->Bind(gfx);
				bitop->Bind(gfx);
				bips->Bind(gfx);
				bivs->Bind(gfx);
				bilay->Bind(gfx);
				rt.Clear(gfx, { 0.0f,0.0f,0.0f,1.0f });
				rt.BindAsBuffer(gfx);
				gfx.RenderIndexed(biidx->GetCount());
				gfx.GetTarget()->BindAsBuffer(gfx);
				rt.ToSurface(gfx).Save("Test_" + vsName + ".png");
			};
			RenderWithVS("Test1_VS.cso");
			RenderWithVS("Test2_VS.cso");

		}
		void TestDynamicMeshLoading()
		{
			using namespace MP;
			Assimp::Importer imp;

			const auto pScene = imp.ReadFile("Models\\brick_wall\\brick_wall.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

			auto layout = VertexLayout{}.
				Append(ElementType::Position3D).
				Append(ElementType::Normal).
				Append(ElementType::Tangent).
				Append(ElementType::Bitangent).
				Append(ElementType::Texture2D);
			MP::VertexBuffer buf{ std::move(layout),*pScene->mMeshes[0] };
			for (auto i = 0ull, end = buf.Size(); i < end; i++)
			{
				const auto a = buf[i].Attr<ElementType::Position3D>();
				const auto b = buf[i].Attr<ElementType::Normal>();
				const auto c = buf[i].Attr<ElementType::Tangent>();
				const auto d = buf[i].Attr<ElementType::Bitangent>();
				const auto e = buf[i].Attr<ElementType::Texture2D>();
			}
		}

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
				// set if exists
				{
					assert(b["butts"s]["pubes"s].SetIfExists(DirectX::XMFLOAT3{ 1.0f,2.0f,3.0f }));
					auto& f3 = static_cast<const DirectX::XMFLOAT3&>(b["butts"s]["pubes"s]);
					assert(f3.x == 1.0f && f3.y == 2.0f && f3.z == 3.0f);
					assert(!b["butts"s]["phubar"s].SetIfExists(DirectX::XMFLOAT3{ 2.0f,2.0f,7.0f }));
				}
				const auto& cb = b;
				{
					DirectX::XMFLOAT4X4 act = cb["arr"][2]["meta"s][5][3];
					assert(act._11 == 1.0f);
				}
				//this doesn't compile buffer is const 
				//cb["arr"][2]["booly"s] = true;
				//static_cast<bool&>(cb["arr"][2]["booly"]) =true;

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
			{
				MP::RawLayout s;
				s.Add<MP::Array>("arr");
				s["arr"].Set<MP::Float>(16);
				auto b = MP::Buffer(std::move(s));

				auto act = b.GetSizeInBytes();
				assert(act == 256u);
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
			//specific testing scenario
			{
				MP::RawLayout pscLayout;
				pscLayout.Add<MP::Float3>("materialColor");
				pscLayout.Add<MP::Float3>("specularColor");
				pscLayout.Add<MP::Float>("specularWeight");
				pscLayout.Add<MP::Float>("specularGloss");
				auto cooked = MP::LayoutCodex::Resolve(std::move(pscLayout));
				assert(cooked.GetSizeInBytes() == 48u);

			}
			// array non-packing
			{	MP::RawLayout pscLayout;
			pscLayout.Add<MP::Array>("arr");
			pscLayout["arr"].Set<MP::Float>(10);
			auto cooked = MP::LayoutCodex::Resolve(std::move(pscLayout));
			assert(cooked.GetSizeInBytes() == 160u);
			}
			//array of struct w/ padding
			{
				MP::RawLayout pscLayout;
				pscLayout.Add<MP::Array>("arr");
				pscLayout["arr"].Set<MP::Struct>(10);
				pscLayout["arr"].T().Add<MP::Float3>("x");
				pscLayout["arr"].T().Add<MP::Float2>("y");

				auto cooked = MP::LayoutCodex::Resolve(std::move(pscLayout));

				assert(cooked.GetSizeInBytes() == 320u);
			}
			// testing pointer stuff
			{
				MP::RawLayout s;
				s.Add<MP::Struct>("butts"s);
				s["butts"s].Add<MP::Float3>("pubes"s);
				s["butts"s].Add<MP::Float>("dank"s);

				auto b = MP::Buffer(std::move(s));
				const auto exp = 696969.6969f;
				b["butts"s]["dank"s] = 696969.6969f;
				assert((float&)b["butts"s]["dank"s] == exp);
				assert(*(float*)&b["butts"s]["dank"s] == exp);
				const auto exp2 = 42.424242f;
				*(float*)&b["butts"s]["dank"s] = exp2;
				assert((float&)b["butts"s]["dank"s] == exp2);
			}
			// specific testing scenario (packing error)
			{
				MP::RawLayout lay;
				lay.Add<MP::Bool>("normalMapEnabled");
				lay.Add<MP::Bool>("specularMapEnabled");
				lay.Add<MP::Bool>("hasGlossMap");
				lay.Add<MP::Float>("specularPower");
				lay.Add<MP::Float3>("specularColor");
				lay.Add<MP::Float>("specularMapWeight");

				auto buf = MP::Buffer(std::move(lay));
				assert(buf.GetSizeInBytes() == 32u);
			}
			// specific testing scenario (array packing issues )
			{
				const int maxRadius = 7;
				const int nCoef = maxRadius * 2 + 1;
				MP::RawLayout l;
				l.Add<MP::Integer>("nTaps");
				l.Add < MP::Array>("coefficients");
				l["coefficients"].Set<MP::Float>(nCoef);
				MP::Buffer buf{ std::move(l) };
				// assert proper amount of memory allocated
				assert(buf.GetSizeInBytes() == (nCoef + 1) * 4 * 4);

				// assert array empty
				{
					const char* begin = reinterpret_cast<char*>((int*)&buf["nTaps"]);
					assert(std::all_of(begin, begin + buf.GetSizeInBytes(), [](char c) {return c == 0; }));
				}
				// assert sparse float storage
				{
					for (int i = 0; i < nCoef; i++)
					{
						buf["coefficients"][i] = 6.9f;
					}
					const auto begin = reinterpret_cast<std::array<float, 4>*>((float*)&buf["coefficients"][0]);
					const auto end = begin + nCoef;
					assert(std::all_of(begin, end, [](const auto& arr)
						{
							return arr[0] == 6.9f && arr[1] == 0.0f && arr[2] == 0.0f && arr[3] == 0.0f;
						}));
				}
			}
		}
	}
}