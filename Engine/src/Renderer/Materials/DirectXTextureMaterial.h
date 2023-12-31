#pragma once

#include "DirectXMaterial.h"
#include "Renderer/Resource/Texture.h"
#include "Renderer/DirectXContext.h"

namespace Engine
{
	class DirectXTextureShader;

	class DirectXTextureMaterial : public DirectXMaterial
	{
	public:
		DirectXTextureMaterial(DirectXTextureShader* shader);
		DirectXTextureMaterial(DirectXTextureShader* shader, Texture* texture);

		void Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer) override;

	protected:
		Texture* m_Texture;
	};
}
