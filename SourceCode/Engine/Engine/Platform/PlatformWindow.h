// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformOS.h>
#include <functional>

#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformWindow
		{
		public:
			enum class Style
			{
				Overlapped = 0x00000000L,
				Popup = 0x80000000L,
				Child = 0x40000000L,
				Minimize = 0x20000000L,
				Visible = 0x10000000L,
				Disabled = 0x08000000L,
				ClipSiblings = 0x04000000L,
				ClipChildren = 0x02000000L,
				Maximize = 0x01000000L,
				Caption = 0x00C00000L,
				Border = 0x00800000L,
				DialogFrame = 0x00400000L,
				VerticalScroll = 0x00200000L,
				HorizontalScroll = 0x00100000L,
				SystemMenu = 0x00080000L,
				ThickFrame = 0x00040000L,
				Group = 0x00020000L,
				TabStop = 0x00010000L,
				MinimizeBox = 0x00020000L,
				MaximizeBox = 0x00010000L,

				OverlappedWindow = Overlapped | Caption | SystemMenu | ThickFrame | MinimizeBox | MaximizeBox,
				PopupWindow = Popup | Border | SystemMenu
			};

		public:
			typedef size_t * Handle;
			typedef std::function<int32(Handle, uint32, uint32*, uint32*)> Procedure;

		public:
			static Handle Create(PlatformOS::Handle Handle, cstr Name, Style Style, Procedure Procedure);

			static void SetPosition(Handle Handle, uint32 X, uint32 Y);
			static void SetSize(Handle Handle, uint32 Width, uint32 Height);
			static void Show(Handle Handle, bool Show);
			static void Invalidate(Handle Handle);
			static void SetTopMost(Handle Handle, bool TopMost);

			static int32 DefaultProcedure(Handle Handle, uint32 Message, uint32 *WParam, uint32 *LParam);

			static int32 Update(Handle Handle);
		};
	}
}

#endif