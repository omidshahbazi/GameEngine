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
			RenderingManager::GetInstance()->GetActiveDevice()->SetContext(m_RenderContext);

			Device->SetProjectionSize(m_RenderWindow.GetClientSize());

			RenderableWindow::RenderAll(Device);
		}

		void PhysicalWindow::OnPositionChanged(void)
		{
			
		}

		void PhysicalWindow::OnSizeChanged(void)
		{
			RenderableWindow::OnSizeChanged();

			if (!m_ShouldUpdateSizeFromRenderableWindow)
				return;

			m_ShouldUpdateSizeFromRenderWindow = false;

			auto& size = GetSize();

			m_RenderWindow.SetSize(size);
			m_RenderWindow.SetTitleBarSize({ size.X - 100, 25 });

			m_ShouldUpdateSizeFromRenderWindow = true;
		}

		void PhysicalWindow::UpdateSizeFromRenderWindow(void)
		{
			if (!m_ShouldUpdateSizeFromRenderWindow)
				return;

			m_ShouldUpdateSizeFromRenderableWindow = false;

			auto& size = m_RenderWindow.GetClientSize();

			SetSize(size);

			m_ShouldUpdateSizeFromRenderableWindow = true;
		}
	}
}