// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\PhysicalWindow.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Rendering;

	namespace EditorGUI
	{
		PhysicalWindow::PhysicalWindow(void) :
			m_RenderWindow("PhysicalWindow"),
			m_RenderContext(nullptr),
			m_RenderWindowListener(this),
			m_ShouldUpdateSizeFromRenderableWindow(true),
			m_ShouldUpdateSizeFromRenderWindow(true)
		{
			m_RenderContext = RenderingManager::GetInstance()->GetActiveDevice()->CreateContext(&m_RenderWindow);

			m_RenderWindow.AddListener(&m_RenderWindowListener);
			m_RenderWindow.SetShowFrame(false);
		}

		void PhysicalWindow::RenderAll(EditorRenderDeviceBase* Device)
		{
			if (m_RenderContext == nullptr)
				return;

			RenderingManager::GetInstance()->GetActiveDevice()->SetContext(m_RenderContext);

			Device->SetProjectionSize(m_RenderWindow.GetClientSize());

			RenderableWindow::RenderAll(Device);
		}

		void PhysicalWindow::OnPositionChanged(void)
		{
			RenderableWindow::OnPositionChanged();

		}

		void PhysicalWindow::OnSizeChanged(void)
		{
			RenderableWindow::OnSizeChanged();

			if (!m_ShouldUpdateSizeFromRenderableWindow)
				return;

			m_ShouldUpdateSizeFromRenderWindow = false;

			m_RenderWindow.SetSize(GetSize() + (m_RenderWindow.GetSize() - m_RenderWindow.GetClientSize()));

			UpdateTitleSize();

			m_ShouldUpdateSizeFromRenderWindow = true;
		}

		void PhysicalWindow::OnClosing(void)
		{
			RenderableWindow::OnClosing();

			RenderingManager::GetInstance()->GetActiveDevice()->DestroyContext(m_RenderContext);
			m_RenderContext = nullptr;

			m_RenderWindow.Close();
		}

		void PhysicalWindow::OnMaximizeRestore(void)
		{
			RenderableWindow::OnMaximizeRestore();

			if (m_RenderWindow.GetState() == RenderWindow::States::Noraml)
				m_RenderWindow.SetState(RenderWindow::States::Maximized);
			else
				m_RenderWindow.SetState(RenderWindow::States::Noraml);
		}

		void PhysicalWindow::OnMinimize(void)
		{
			RenderableWindow::OnMinimize();

			m_RenderWindow.SetState(RenderWindow::States::Minimized);
		}

		void PhysicalWindow::UpdateSizeFromRenderWindow(void)
		{
			if (!m_ShouldUpdateSizeFromRenderWindow)
				return;

			m_ShouldUpdateSizeFromRenderableWindow = false;

			auto& size = m_RenderWindow.GetClientSize();

			SetSize(size);

			UpdateTitleSize();

			m_ShouldUpdateSizeFromRenderableWindow = true;
		}

		void PhysicalWindow::UpdateTitleSize(void)
		{
			m_RenderWindow.SetTitleBarSize({ GetSize().X - 100, 25 });
		}
	}
}