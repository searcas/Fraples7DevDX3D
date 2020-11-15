#include "Material.h"

namespace FraplesDev
{
	Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& modelPath) noexcept(!IS_DEBUG)
		:_mModelPath(modelPath.string())
	{
		const auto rootPath = modelPath.parent_path().string() + "\\";
		
		{
			aiString tempName;
			material.Get(AI_MATKEY_NAME, tempName);
			_mName = tempName.C_Str();
		}

		// phong technique

		{
			Technique phong{ "Phong" };
			Step step(0);
			std::string shaderCode = "Phong";
			aiString texFileName;

			// common pre

			_mVertexLayout.Append(MP::ElementType::Position3D);
			_mVertexLayout.Append(MP::ElementType::Normal);
			MP::RawLayout pscLayout;
			bool hasTexture = false;
			bool hasGlossAlpha = false;

//diffuse
			{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Diffuse";
				_mVertexLayout.Append(MP::ElementType::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				if (tex->HasAlhpa())
				{
					hasAlpha = true;
					shaderCode += "Mask";
				}
				step.AddContext(std::move(tex));
			}
			else
			{
				pscLayout.Add<MP::Float3>("materialColor");
			}
			step.AddContext(Rasterizer::Resolve(gfx, hasAlpha));
			}
			//specular
			{
				if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
				{
					hasTexture = true;
					shaderCode += "Specular";
					_mVertexLayout.Append(MP::ElementType::Texture2D);
					auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
					hasGlossAlpha = tex->HasAlhpa();
					step.AddContext(std::move(tex));
					pscLayout.Add<MP::Bool>("UseGlossAlpha");
					pscLayout.Add<MP::Bool>("useSpecularMap");
				}
				pscLayout.Add<MP::Float3>("specularColor");
				pscLayout.Add<MP::Float>("specularWeight");
				pscLayout.Add<MP::Float>("specularGloss");
			}
			// normal

			{
				if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
				{
					hasTexture = true;
					shaderCode += "Normal";
					_mVertexLayout.Append(MP::ElementType::Texture2D);
					_mVertexLayout.Append(MP::ElementType::Tangent);
					_mVertexLayout.Append(MP::ElementType::Bitangent);
					step.AddContext(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
					pscLayout.Add<MP::Bool>("useNormalMap");
					pscLayout.Add<MP::Float>("normalMapWeight");
				}
			}
			// common (post)
			{
				step.AddContext(std::make_shared<TransformCBuf>(gfx, 0u));
				step.AddContext(Blending::Resolve(gfx, false));
				auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
				auto pvsbyte = pvs->GetByteCode();

				step.AddContext(std::move(pvs));
				step.AddContext(PixelShader::Resolve(gfx, shaderCode + "_PS.cso"));

				step.AddContext(InputLayout::Resolve(gfx, _mVertexLayout, pvsbyte));
				if (hasTexture)
				{
					step.AddContext(Sampler::Resolve(gfx));
				}
				// PS material params (cbuf	)
				MP::Buffer buf{ std::move(pscLayout) };
				if (auto r = buf["materialColor"]; r.Exists())
				{
					aiColor3D color = { 0.45f,0.45,0.85f };
					material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
				buf["useSpecularMap"].SetIfExists(true);
				if (auto r = buf["specularColor"]; r.Exists())
				{
					aiColor3D color = { 0.18f,0.18f,0.18f };
					material.Get(AI_MATKEY_COLOR_SPECULAR, color);
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buf["specularWeight"].SetIfExists(1.0f);

				if (auto r = buf["specularGloss"]; r.Exists())
				{
					float gloss = 8.0f;
					material.Get(AI_MATKEY_SHININESS, gloss);
					r = gloss;
				}
				buf["useNormalMap"].SetIfExists(true);
				buf["normalMapWeight"].SetIfExists(1.0f);
				step.AddContext(std::make_unique<CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
				phong.AddStep(std::move(step));
				_mTechniques.push_back(std::move(phong));
			}
		}

		//outline technique
		{
			Technique outline("Outline",false);
			{
				Step mask(1);

				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				auto pvsbyte = pvs->GetByteCode();
				mask.AddContext(std::move(pvs));

				//TODO: better sub-layout generation tech for future consideration maybe
				mask.AddContext(InputLayout::Resolve(gfx, _mVertexLayout, pvsbyte));
				mask.AddContext(std::make_shared<TransformCBuf>(gfx));

				//TODO: might need to specify rasterizer when doubled-sided models start being used
				outline.AddStep(std::move(mask));
			}
			{
				Step draw(2);
				//these can be pass-constant (tricky due to layout issues)
				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				auto pvsbyte = pvs->GetByteCode();
				draw.AddContext(std::move(pvs));

				//this can be pass-constant
				draw.AddContext(PixelShader::Resolve(gfx, "Solid_PS.cso"));
				MP::RawLayout layout;
				layout.Add<MP::Float3>("materialColor");
				auto buf = MP::Buffer(std::move(layout));
				buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f};
				draw.AddContext(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));

				//TODO: better sub-layout generation tech for future consideration maybe
				draw.AddContext(InputLayout::Resolve(gfx, _mVertexLayout, pvsbyte));

				//quic and dirty.. nicer soulution maybe takes a lamba ..

				class TransformCbufScaling : public TransformCBuf
				{
				public:
					TransformCbufScaling(Graphics& gfx, float scale = 1.04f)
						:TransformCBuf(gfx), _mBuf(MakeLayout())
					{
						_mBuf["scale"] = scale;
					}
					void Accept(TechniqueProbe& probe)override
					{
						probe.VisitBuffer(_mBuf);
					}
					void Bind(Graphics& gfx)noexcept override
					{
						const float scale = _mBuf["scale"];
						const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
						auto xf = GetTransforms(gfx);
						xf.modelView = xf.modelView * scaleMatrix;
						xf.modelViewProj = xf.modelViewProj * scaleMatrix;
						UpdateBindImpl(gfx, xf);
					}
					std::unique_ptr<CloningContext>Clone()const noexcept override
					{
						return std::make_unique<TransformCbufScaling>(*this);
					}
					static MP::RawLayout MakeLayout()
					{
						MP::RawLayout layout;
						layout.Add<MP::Float>("scale");
						return layout;
					}
				private:
					MP::Buffer _mBuf;
				};
				draw.AddContext(std::make_shared<TransformCbufScaling>(gfx));

				//TODO: might need to specify resterizer when doubled-sided models start being used

				outline.AddStep(std::move(draw));
			}
			_mTechniques.push_back(std::move(outline));
		}
	}

	MP::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
	{
		return { _mVertexLayout,mesh };
	}
	std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
	{
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		return indices;
	}

	std::vector<Technique> Material::GetTechniques() const noexcept
	{
		return _mTechniques;
	}
	std::shared_ptr<VertexBuffer> Material::MakeVertexContext(Graphics& gfx, const aiMesh& mesh, float scale) const noexcept(!IS_DEBUG)
	{
		auto vtc = ExtractVertices(mesh);

		if (scale != 1.0f)
		{
			for (auto i = 0u; i < vtc.Size(); i++)
			{
				DirectX::XMFLOAT3& pos = vtc[i].Attr<MP::ElementType::Position3D>();
				pos.x *= scale;
				pos.y *= scale;
				pos.z *= scale;
			}
		}
		return VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtc));
	}
	std::shared_ptr<IndexBuffer> Material::MakeIndexContext(Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG)
	{
		return IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
	}
	std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
	{
		return _mModelPath + "%" + mesh.mName.C_Str();
	}
}