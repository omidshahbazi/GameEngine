// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\RenderTargetPool.h>
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace DataUtility;

	namespace RenderSystem
	{
		SINGLETON_DEFINITION(RenderTargetPool);

		uint32 CalculateHash(const Vector2I& Dimension, Formats Format)
		{
			return Hash::CRC32(&Dimension, 1) + Hash::CRC32(&Format, 1);
		}

		RenderTargetPool::RenderTargetPool(void) :
			m_RenderTargets(RenderSystemAllocators::ContainersAllocator)
		{
		}

		RenderTargetPool::~RenderTargetPool(void)
		{
			for (auto item : m_RenderTargets)
				for (auto rt : item.GetSecond())
					m_Device->DestroyRenderTarget(rt);

			m_RenderTargets.Clear();
		}

		void RenderTargetPool::Initialize(DeviceInterface* Device)
		{
			m_Device = Device;
		}

		RenderTarget* RenderTargetPool::Get(const RenderTargetInfo* Info)
		{
			uint32 hash = 0;
			for (const auto& texture : Info->Textures)
				hash += CalculateHash(texture.Dimension, texture.Format);

			if (m_RenderTargets.Contains(hash))
			{
				auto& stack = m_RenderTargets[hash];

				RenderTarget* rt = nullptr;

				if (stack.GetSize() != 0)
					stack.Pop(&rt);
				else
					rt = m_Device->CreateRenderTarget(Info);

				return rt;
			}

			m_RenderTargets[hash] = RenderTargetStack(RenderSystemAllocators::ContainersAllocator);

			return Get(Info);
		}

		void RenderTargetPool::Back(RenderTarget* RenderTarget)
		{
			uint32 hash = 0;
			for (const auto texture : RenderTarget->GetTextures())
				hash += CalculateHash(texture->GetDimension(), texture->GetFormat());

			if (m_RenderTargets.Contains(hash))
			{
				auto& stack = m_RenderTargets[hash];

				stack.Push(RenderTarget);
			}
			else
				m_RenderTargets[hash] = RenderTargetStack(RenderSystemAllocators::ContainersAllocator);

			Back(RenderTarget);
		}
	}
}