// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <utility>

#ifndef PLATFORM_NETWORK_H
#define PLATFORM_NETWORK_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformNetwork
		{
		public:
			enum class AddressFamilies
			{
				InterNetwork
			};

			enum class Types
			{
				Stream,
				Datagram,
				RawProtocol,
				ReliablyDeliverdMessage,
				SequenedPacketStream
			};

			enum class IPProtocols
			{
				UDP
			};

			typedef uint32 Handle;

			static bool Initialize(void);
			static bool Shotdown(void);

			static Handle Create(AddressFamilies AddressFamily, Types Type, IPProtocols IPProtocol);
			static void Close(Handle Handle);

			static bool Bind(Handle Handle, AddressFamilies AddressFamily, uint16 Port);

			static uint32 GetAddress(uint8 A, uint8 B, uint8 C, uint8 D)
			{
				return ((A << 24) | (B << 16) | (C << 8) | D);
			}
		};
	}
}

#endif