// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBuffer.h>
#include <RenderDevice\ICommandBuffer.h>

namespace Engine
{
	namespace RenderSystem
	{
		CommandBuffer::CommandBuffer(ICommandBuffer* Buffer) :
			m_Buffer(Buffer)
		{
		}

		CommandBuffer::~CommandBuffer(void)
		{
		}

		void CommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
		{
			m_Buffer->SetViewport(Position, Size);
		}

		void CommandBuffer::SetRenderTarget(RenderTarget* RenderTarget)
		{
			m_Buffer->BindRenderTarget
		}

		void CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
		{
			m_Buffer->Clear(Flags, Color);
		}

		void CommandBuffer::DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material)
		{
		}

		void CommandBuffer::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material)
		{
		}

		void CommandBuffer::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
		}

		void CommandBuffer::BeginEvent(const String& Label)
		{
			BeginEvent(Label.ChangeType<char16>());
		}

		void CommandBuffer::BeginEvent(const WString& Label)
		{
			m_Buffer->BeginEvent(Label.GetValue());
		}

		void CommandBuffer::EndEvent(void)
		{
			m_Buffer->EndEvent();
		}

		void CommandBuffer::SetMarker(const String& Label)
		{
			SetMarker(Label.ChangeType<char16>());
		}

		void CommandBuffer::SetMarker(const WString& Label)
		{
			m_Buffer->SetMarker(Label.GetValue());
		}
	}
}