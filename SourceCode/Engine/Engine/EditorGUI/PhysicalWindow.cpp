// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\PhysicalWindow.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Rendering;

	namespace EditorGUI
	{
		PhysicalWindow::PhysicalWindow(void) :
			RenderWindow("PhysicalWindow"),
			m_RenderContext(nullptr),
			m_RenderWindowListener(this),
			m_ShouldUpdateSizeFromRenderableWindow(true),
			m_ShouldUpdateSizeFromRenderWindow(true)
		{
			m_RenderContext = RenderingManager::GetInstance()->GetActiveDevice()->CreateContext(this);

			RenderWindow::AddListener(&m_RenderWindowListener);
			RenderWindow::SetShowFrame(false);
		}

		void PhysicalWindow::RenderAll(EditorRenderDeviceBase* Device)
		{
			if (m_RenderContext == nullptr)
				return;

			RenderingManager::GetInstance()->GetActiveDevice()->SetContext(m_RenderContext);

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

			RenderingManager::GetInstance()->GetActiveDevice()->DestroyContext(m_RenderContext);
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