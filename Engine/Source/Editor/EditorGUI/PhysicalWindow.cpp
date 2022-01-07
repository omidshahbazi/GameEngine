// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\PhysicalWindow.h>
#include <RenderSystem\RenderManager.h>

namespace Engine
{
	using namespace RenderSystem;

	namespace EditorGUI
	{
		PhysicalWindow::PhysicalWindow(void) :
			RenderWindow("PhysicalWindow"),
			m_RenderContext(nullptr),
			m_ShouldUpdateSizeFromRenderableWindow(true),
			m_ShouldUpdateSizeFromRenderWindow(true)
		{
			m_RenderContext = RenderManager::GetInstance()->GetDevice()->CreateContext(this);

			RenderWindow::OnSizeChangedEvent += EventListener_OnSizeChangedHandler;
			RenderWindow::OnKeyDownEvent += EventListener_OnKeyDownHandler;
			RenderWindow::OnKeyUpEvent += EventListener_OnKeyUpHandler;
			RenderWindow::OnKeyPressedEvent += EventListener_OnKeyPressedHandler;
			RenderWindow::OnMouseDownEvent += EventListener_OnMouseDownHandler;
			RenderWindow::OnMouseUpEvent += EventListener_OnMouseUpHandler;
			RenderWindow::OnMouseClickEvent += EventListener_OnMouseClickHandler;
			RenderWindow::OnMouseWheelEvent += EventListener_OnMouseWheelHandler;
			RenderWindow::OnMouseMoveEvent += EventListener_OnMouseMoveHandler;
			RenderWindow::OnMouseLeaveEvent += EventListener_OnMouseLeaveHandler;

			RenderWindow::SetShowFrame(false);
		}

		PhysicalWindow::~PhysicalWindow(void)
		{
			RenderWindow::OnSizeChangedEvent -= EventListener_OnSizeChangedHandler;
			RenderWindow::OnKeyDownEvent -= EventListener_OnKeyDownHandler;
			RenderWindow::OnKeyUpEvent -= EventListener_OnKeyUpHandler;
			RenderWindow::OnKeyPressedEvent -= EventListener_OnKeyPressedHandler;
			RenderWindow::OnMouseDownEvent -= EventListener_OnMouseDownHandler;
			RenderWindow::OnMouseUpEvent -= EventListener_OnMouseUpHandler;
			RenderWindow::OnMouseClickEvent -= EventListener_OnMouseClickHandler;
			RenderWindow::OnMouseWheelEvent -= EventListener_OnMouseWheelHandler;
			RenderWindow::OnMouseMoveEvent -= EventListener_OnMouseMoveHandler;
			RenderWindow::OnMouseLeaveEvent -= EventListener_OnMouseLeaveHandler;

			if (m_RenderContext != nullptr)
				RenderManager::GetInstance()->GetDevice()->DestroyContext(m_RenderContext);
		}

		void PhysicalWindow::RenderAll(EditorRenderDeviceBase* Device) const
		{
			if (m_RenderContext == nullptr)
				return;

			RenderManager::GetInstance()->GetDevice()->SetContext(m_RenderContext);

			RenderManager::GetInstance()->GetDevice()->SetViewport(Vector2I::Zero, RenderWindow::GetClientSize());

			Device->SetProjectionSize(RenderWindow::GetClientSize());

			RenderableWindow::RenderAll(Device);
		}

		void PhysicalWindow::OnSizeChanged(void)
		{
			RenderableWindow::OnSizeChanged();

			if (!m_ShouldUpdateSizeFromRenderableWindow)
				return;

			m_ShouldUpdateSizeFromRenderWindow = false;

			RenderWindow::SetSize(RenderableWindow::GetSize() + (RenderWindow::GetSize() - RenderWindow::GetClientSize()));

			UpdateTitleSize();

			m_ShouldUpdateSizeFromRenderWindow = true;
		}

		void PhysicalWindow::OnClosing(void)
		{
			RenderableWindow::OnClosing();

			RenderManager::GetInstance()->GetDevice()->DestroyContext(m_RenderContext);
			m_RenderContext = nullptr;

			RenderWindow::Close();
		}

		void PhysicalWindow::OnMaximize(void)
		{
			RenderableWindow::OnMaximize();

			RenderWindow::SetState(RenderWindow::States::Maximized);
		}

		void PhysicalWindow::OnRestore(void)
		{
			RenderableWindow::OnRestore();

			RenderWindow::SetState(RenderWindow::States::Noraml);
		}

		void PhysicalWindow::OnMinimize(void)
		{
			RenderableWindow::OnMinimize();

			RenderWindow::SetState(RenderWindow::States::Minimized);
		}

		void PhysicalWindow::UpdateSizeFromRenderWindow(void)
		{
			if (!m_ShouldUpdateSizeFromRenderWindow)
				return;

			m_ShouldUpdateSizeFromRenderableWindow = false;

			auto& size = RenderWindow::GetClientSize();

			RenderableWindow::SetSize(size);

			UpdateTitleSize();

			m_ShouldUpdateSizeFromRenderableWindow = true;
		}

		void PhysicalWindow::UpdateTitleSize(void)
		{
			RenderWindow::SetTitleBarSize({ RenderableWindow::GetSize().X - 100, 25 });
		}
	}
}