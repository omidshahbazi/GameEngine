// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef CONNECTION_PROTOCOL_H
#define CONNECTION_PROTOCOL_H

namespace Engine
{
	using namespace Common;

	namespace Networking
	{
		static const byte PACKET_TYPE_HAND_SHAKE_REQUEST[]{ 1 };
		static const byte PACKET_TYPE_HAND_SHAKE_RESPONSE[]{ 2 };

		static const byte PACKET_TYPE_MESSAGE[]{ 3 };

		static const byte PACKET_TYPE_SIZE = sizeof(PACKET_TYPE_HAND_SHAKE_REQUEST);
	}
}

#endif