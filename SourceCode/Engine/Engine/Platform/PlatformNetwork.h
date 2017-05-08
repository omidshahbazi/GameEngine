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
		//
		// http://gafferongames.com/networking-for-game-programmers/sending-and-receiving-packets/
		//
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

			enum class Errors
			{
				NoError,
				BaseError,
				Interrupted,
				BadFile,
				AccessDenied,
				InvalidPointer,
				InvalidArguments,
				TooManySockets,
				WouldBlock,
				BlockingInProgress,
				NonBlockingInProgress,
				NoSocket,
				DestinationAddressRequired,
				LargePacketSize,
				MismatchProtocolType,
				NoProtocolOperation,
				ProtocolNotSupported,
				SocketNotSupported,
				OperationNotSupported,
				ProtocolFamilyNotSupported,
				AddressFamilyNotSupported,
				AddressInUse,
				AddressNotValid,
				NetworkDown,
				NetworkUnreachable,
				NetworkReset,
				ConnectionAbort,
				ConnectionReset,
				NoBuffer,
				IsConnected,
				NotConnected,
				Shutdown,
				TooManyReferences,
				Timeout,
				ConnectionRefused,
				Loop,
				NameTooLong,
				HostDown,
				HostUnreachable,
				NotEmpty,
				ProcessLimit,
				OutOfUsers,
				OutOfDisk,
				HandleNotExists,
				ItemNotExists,
				SystemNotReady,
				VersionNotSupported,
				NotInitialized,
				Disconnected,
				NoMoreResult1,
				Canceled,
				InvalidProcedureTable,
				InvalidProvider,
				ProviderFailedToInitialize,
				SystemCallFailure,
				ServiceNotFound,
				TypeNotFound,
				NoMoreResult2,

				//
				// MessageId: WSA_E_CANCELLED
				//
				// MessageText:
				//
				// A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
				//
				WSA_E_CANCELLED,

				//
				// MessageId: WSAEREFUSED
				//
				// MessageText:
				//
				// A database query failed because it was actively refused.
				//
				WSAEREFUSED,

				//
				// MessageId: WSAHOST_NOT_FOUND
				//
				// MessageText:
				//
				// No such host is known.
				//
				WSAHOST_NOT_FOUND,

				//
				// MessageId: WSATRY_AGAIN
				//
				// MessageText:
				//
				// This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server.
				//
				WSATRY_AGAIN,

				//
				// MessageId: WSANO_RECOVERY
				//
				// MessageText:
				//
				// A non-recoverable error occurred during a database lookup.
				//
				WSANO_RECOVERY,

				//
				// MessageId: WSANO_DATA
				//
				// MessageText:
				//
				// The requested name is valid, but no data of the requested type was found.
				//
				WSANO_DATA,

				//
				// MessageId: WSA_QOS_RECEIVERS
				//
				// MessageText:
				//
				// At least one reserve has arrived.
				//
				WSA_QOS_RECEIVERS,

				//
				// MessageId: WSA_QOS_SENDERS
				//
				// MessageText:
				//
				// At least one path has arrived.
				//
				WSA_QOS_SENDERS,

				//
				// MessageId: WSA_QOS_NO_SENDERS
				//
				// MessageText:
				//
				// There are no senders.
				//
				WSA_QOS_NO_SENDERS,

				//
				// MessageId: WSA_QOS_NO_RECEIVERS
				//
				// MessageText:
				//
				// There are no receivers.
				//
				WSA_QOS_NO_RECEIVERS,

				//
				// MessageId: WSA_QOS_REQUEST_CONFIRMED
				//
				// MessageText:
				//
				// Reserve has been confirmed.
				//
				WSA_QOS_REQUEST_CONFIRMED,

				//
				// MessageId: WSA_QOS_ADMISSION_FAILURE
				//
				// MessageText:
				//
				// Error due to lack of resources.
				//
				WSA_QOS_ADMISSION_FAILURE,

				//
				// MessageId: WSA_QOS_POLICY_FAILURE
				//
				// MessageText:
				//
				// Rejected for administrative reasons - bad credentials.
				//
				WSA_QOS_POLICY_FAILURE,

				//
				// MessageId: WSA_QOS_BAD_STYLE
				//
				// MessageText:
				//
				// Unknown or conflicting style.
				//
				WSA_QOS_BAD_STYLE,

				//
				// MessageId: WSA_QOS_BAD_OBJECT
				//
				// MessageText:
				//
				// Problem with some part of the filterspec or providerspecific buffer in general.
				//
				WSA_QOS_BAD_OBJECT,

				//
				// MessageId: WSA_QOS_TRAFFIC_CTRL_ERROR
				//
				// MessageText:
				//
				// Problem with some part of the flowspec.
				//
				WSA_QOS_TRAFFIC_CTRL_ERROR,

				//
				// MessageId: WSA_QOS_GENERIC_ERROR
				//
				// MessageText:
				//
				// General QOS error.
				//
				WSA_QOS_GENERIC_ERROR,

				//
				// MessageId: WSA_QOS_ESERVICETYPE
				//
				// MessageText:
				//
				// An invalid or unrecognized service type was found in the flowspec.
				//
				WSA_QOS_ESERVICETYPE,

				//
				// MessageId: WSA_QOS_EFLOWSPEC
				//
				// MessageText:
				//
				// An invalid or inconsistent flowspec was found in the QOS structure.
				//
				WSA_QOS_EFLOWSPEC,

				//
				// MessageId: WSA_QOS_EPROVSPECBUF
				//
				// MessageText:
				//
				// Invalid QOS provider-specific buffer.
				//
				WSA_QOS_EPROVSPECBUF,

				//
				// MessageId: WSA_QOS_EFILTERSTYLE
				//
				// MessageText:
				//
				// An invalid QOS filter style was used.
				//
				WSA_QOS_EFILTERSTYLE,

				//
				// MessageId: WSA_QOS_EFILTERTYPE
				//
				// MessageText:
				//
				// An invalid QOS filter type was used.
				//
				WSA_QOS_EFILTERTYPE,

				//
				// MessageId: WSA_QOS_EFILTERCOUNT
				//
				// MessageText:
				//
				// An incorrect number of QOS FILTERSPECs were specified in the FLOWDESCRIPTOR.
				//
				WSA_QOS_EFILTERCOUNT,

				//
				// MessageId: WSA_QOS_EOBJLENGTH
				//
				// MessageText:
				//
				// An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer.
				//
				WSA_QOS_EOBJLENGTH,

				//
				// MessageId: WSA_QOS_EFLOWCOUNT
				//
				// MessageText:
				//
				// An incorrect number of flow descriptors was specified in the QOS structure.
				//
				WSA_QOS_EFLOWCOUNT,

				//
				// MessageId: WSA_QOS_EUNKOWNPSOBJ
				//
				// MessageText:
				//
				// An unrecognized object was found in the QOS provider-specific buffer.
				//
				WSA_QOS_EUNKOWNPSOBJ,

				//
				// MessageId: WSA_QOS_EPOLICYOBJ
				//
				// MessageText:
				//
				// An invalid policy object was found in the QOS provider-specific buffer.
				//
				WSA_QOS_EPOLICYOBJ,

				//
				// MessageId: WSA_QOS_EFLOWDESC
				//
				// MessageText:
				//
				// An invalid QOS flow descriptor was found in the flow descriptor list.
				//
				WSA_QOS_EFLOWDESC,

				//
				// MessageId: WSA_QOS_EPSFLOWSPEC
				//
				// MessageText:
				//
				// An invalid or inconsistent flowspec was found in the QOS provider specific buffer.
				//
				WSA_QOS_EPSFLOWSPEC,

				//
				// MessageId: WSA_QOS_EPSFILTERSPEC
				//
				// MessageText:
				//
				// An invalid FILTERSPEC was found in the QOS provider-specific buffer.
				//
				WSA_QOS_EPSFILTERSPEC,

				//
				// MessageId: WSA_QOS_ESDMODEOBJ
				//
				// MessageText:
				//
				// An invalid shape discard mode object was found in the QOS provider specific buffer.
				//
				WSA_QOS_ESDMODEOBJ,

				//
				// MessageId: WSA_QOS_ESHAPERATEOBJ
				//
				// MessageText:
				//
				// An invalid shaping rate object was found in the QOS provider-specific buffer.
				//
				WSA_QOS_ESHAPERATEOBJ,

				//
				// MessageId: WSA_QOS_RESERVED_PETYPE
				//
				// MessageText:
				//
				// A reserved policy element was found in the QOS provider-specific buffer.
				//
				WSA_QOS_RESERVED_PETYPE,

				//
				// MessageId: WSA_SECURE_HOST_NOT_FOUND
				//
				// MessageText:
				//
				// No such host is known securely.
				//
				WSA_SECURE_HOST_NOT_FOUND,

				//
				// MessageId: WSA_IPSEC_NAME_POLICY_ERROR
				//
				// MessageText:
				//
				// Name based IPSEC policy could not be added.
				//
				WSA_IPSEC_NAME_POLICY_ERROR,
			};

			typedef uint32 Handle;

			static Errors Initialize(void);
			static Errors Shotdown(void);

			static Handle Create(AddressFamilies AddressFamily, Types Type, IPProtocols IPProtocol);
			static Errors Close(Handle Handle);

			static Errors Bind(Handle Handle, AddressFamilies AddressFamily, uint16 Port);

			static Errors SetNonBlocking(Handle Handle, bool Enabled);

			static Errors Send(Handle Handle, const byte *Buffer, uint32 Length, AddressFamilies AddressFamily, uint16 Port);

			static uint32 GetAddress(uint8 A, uint8 B, uint8 C, uint8 D)
			{
				return ((A << 24) | (B << 16) | (C << 8) | D);
			}
		};
	}
}

#endif