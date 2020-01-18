// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONNECTION_PROTOCOL_H
#define CONNECTION_PROTOCOL_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Networking
	{
		static const byte PACKET_TYPE_HAND_SHAKE_REQUEST[]{ 223 };
		static const byte PACKET_TYPE_HAND_SHAKE_RESPONSE[]{ 115 };

		static const byte PACKET_TYPE_MESSAGE[]{ 140 };

		static const byte PACKET_TYPE_SIZE = sizeof(PACKET_TYPE_HAND_SHAKE_REQUEST);
	}
}

#endif