// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <RenderSystem\Pass.h>

namespace Engine
{
	namespace RenderSystem
	{
		class RENDERSYSTEM_API Material
		{
		public:
			typedef Vector<Pass> PassList;

		public:
			Material(void);

			Material(const Material& Other);

			INLINE void AddPass(const Pass& Pass)
			{
				m_Passes.Add(Pass);
			}

			INLINE void AddPass(Pass&& Pass)
			{
				m_Passes.Add({});

				Engine::RenderSystem::Pass& newPass = m_Passes[m_Passes.GetSize() - 1];
				newPass = Pass;
			}

			INLINE void RemovePass(uint8 Index)
			{
				m_Passes.RemoveAt(Index);
			}

			INLINE PassList& GetPasses(void)
			{
				return m_Passes;
			}

			INLINE const PassList& GetPasses(void) const
			{
				return m_Passes;
			}

			INLINE Material& operator=(const Material& Other)
			{
				m_Passes = Other.m_Passes;

				return *this;
			}

			void SetQueue(RenderQueues Queue);

			bool SetBuffer(Pass::ConstantHash Hash, const byte* Data, uint16 Size);
			INLINE bool SetBuffer(const String& Name, const byte* Data, uint16 Size)
			{
				return SetBuffer(GetHash(Name), Data, Size);
			}

			template<typename T, int Size = sizeof(T)>
			INLINE void SetBuffer(Pass::ConstantHash Hash, const T* Data)
			{
				SetBuffer(Hash, ReinterpretCast(const byte*, Data), Size);
			}

			template<typename T, int Size = sizeof(T)>
			INLINE void SetBuffer(const String& Name, const T* Data)
			{
				SetBuffer(Name, ReinterpretCast(const byte*, Data), Size);
			}

			bool SetTexture(Pass::ConstantHash Hash, const TextureResource* Value);
			bool SetTexture(const String& Name, const TextureResource* Value)
			{
				return SetTexture(GetHash(Name), Value);
			}

			bool SetSprite(Pass::ConstantHash Hash, const SpriteResource* Value);
			bool SetSprite(const String& Name, const SpriteResource* Value)
			{
				return SetSprite(GetHash(Name), Value);
			}

			void SetRenderState(const RenderState& State);

			static Pass::ConstantHash GetHash(const String& Name)
			{
				return Pass::GetHash(Name);
			}

		private:
			PassList m_Passes;
		};
	}
}

#endif