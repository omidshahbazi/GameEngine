// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\PlatformNetwork.h>
#include <Common\BitwiseUtils.h>
#include <winsock2.h>

#pragma comment(lib, "wsock32.lib")

namespace Engine
{
	namespace Platform
	{
		int GetAddressFamiliy(PlatformNetwork::AddressFamilies Family)
		{
			switch (Family)
			{
			case PlatformNetwork::AddressFamilies::InterNetwork:
				return AF_INET;
			}

			return AF_UNSPEC;
		}

		int GetType(PlatformNetwork::Types Type)
		{
			switch (Type)
			{
			case PlatformNetwork::Types::Stream:
				return SOCK_STREAM;

			case PlatformNetwork::Types::Datagram:
				return SOCK_DGRAM;

			case PlatformNetwork::Types::RawProtocol:
				return SOCK_RAW;

			case PlatformNetwork::Types::ReliablyDeliverdMessage:
				return SOCK_RDM;

			case PlatformNetwork::Types::SequenedPacketStream:
				return SOCK_SEQPACKET;
			}

			return 0;
		}

		int GetIPProtocol(PlatformNetwork::IPProtocols IPProtocol)
		{
			switch (IPProtocol)
			{
			case PlatformNetwork::IPProtocols::UDP:
				return IPPROTO_UDP;
			}

			return IPPROTO_MAX;
		}

		int GetSendFlags(PlatformNetwork::SendModes Mode)
		{
			int flags = 0;

			if (BitwiseUtils::IsEnabled(Mode, PlatformNetwork::SendModes::DontRoute))
				flags |= MSG_DONTROUTE;

			if (BitwiseUtils::IsEnabled(Mode, PlatformNetwork::SendModes::OutOfBand))
				flags |= MSG_OOB;

			return flags;
		}

		sockaddr_in GetAddressInfo(PlatformNetwork::AddressFamilies AddressFamily, uint16 Port)
		{
			sockaddr_in address;
			address.sin_family = GetAddressFamiliy(AddressFamily);
			address.sin_addr.S_un.S_addr = INADDR_ANY;
			address.sin_port = Port;
			return address;
		}

		PlatformNetwork::Errors GetError(int ErrorCode)
		{
			switch (ErrorCode)
			{
			case WSABASEERR: return PlatformNetwork::Errors::BaseError;
			case WSAEINTR: return PlatformNetwork::Errors::Interrupted;
			case WSAEBADF: return PlatformNetwork::Errors::BadFile;
			case WSAEACCES: return PlatformNetwork::Errors::AccessDenied;
			case WSAEFAULT: return PlatformNetwork::Errors::InvalidPointer;
			case WSAEINVAL: return PlatformNetwork::Errors::InvalidArguments;
			case WSAEMFILE: return PlatformNetwork::Errors::TooManySockets;
			case WSAEWOULDBLOCK: return PlatformNetwork::Errors::WouldBlock;
			case WSAEINPROGRESS: return PlatformNetwork::Errors::BlockingInProgress;
			case WSAEALREADY: return PlatformNetwork::Errors::NonBlockingInProgress;
			case WSAENOTSOCK: return PlatformNetwork::Errors::NoSocket;
			case WSAEDESTADDRREQ: return PlatformNetwork::Errors::DestinationAddressRequired;
			case WSAEMSGSIZE: return PlatformNetwork::Errors::LargePacketSize;
			case WSAEPROTOTYPE: return PlatformNetwork::Errors::MismatchProtocolType;
			case WSAENOPROTOOPT: return PlatformNetwork::Errors::NoProtocolOperation;
			case WSAEPROTONOSUPPORT: return PlatformNetwork::Errors::ProtocolNotSupported;
			case WSAESOCKTNOSUPPORT: return PlatformNetwork::Errors::SocketNotSupported;
			case WSAEOPNOTSUPP: return PlatformNetwork::Errors::OperationNotSupported;
			case WSAEPFNOSUPPORT: return PlatformNetwork::Errors::ProtocolFamilyNotSupported;
			case WSAEAFNOSUPPORT: return PlatformNetwork::Errors::AddressFamilyNotSupported;
			case WSAEADDRINUSE: return PlatformNetwork::Errors::AddressInUse;
			case WSAEADDRNOTAVAIL: return PlatformNetwork::Errors::AddressNotValid;
			case WSAENETDOWN: return PlatformNetwork::Errors::NetworkDown;
			case WSAENETUNREACH: return PlatformNetwork::Errors::NetworkUnreachable;
			case WSAENETRESET: return PlatformNetwork::Errors::NetworkReset;
			case WSAECONNABORTED: return PlatformNetwork::Errors::ConnectionAbort;
			case WSAECONNRESET: return PlatformNetwork::Errors::ConnectionReset;
			case WSAENOBUFS: return PlatformNetwork::Errors::NoBuffer;
			case WSAEISCONN: return PlatformNetwork::Errors::IsConnected;
			case WSAENOTCONN: return PlatformNetwork::Errors::NotConnected;
			case WSAESHUTDOWN: return PlatformNetwork::Errors::Shutdown;
			case WSAETOOMANYREFS: return PlatformNetwork::Errors::TooManyReferences;
			case WSAETIMEDOUT: return PlatformNetwork::Errors::Timeout;
			case WSAECONNREFUSED: return PlatformNetwork::Errors::ConnectionRefused;
			case WSAELOOP: return PlatformNetwork::Errors::Loop;
			case WSAENAMETOOLONG: return PlatformNetwork::Errors::NameTooLong;
			case WSAEHOSTDOWN: return PlatformNetwork::Errors::HostDown;
			case WSAEHOSTUNREACH: return PlatformNetwork::Errors::HostUnreachable;
			case WSAENOTEMPTY: return PlatformNetwork::Errors::NotEmpty;
			case WSAEPROCLIM: return PlatformNetwork::Errors::ProcessLimit;
			case WSAEUSERS: return PlatformNetwork::Errors::OutOfUsers;
			case WSAEDQUOT: return PlatformNetwork::Errors::OutOfDisk;
			case WSAESTALE: return PlatformNetwork::Errors::HandleNotExists;
			case WSAEREMOTE: return PlatformNetwork::Errors::ItemNotExists;
			case WSASYSNOTREADY: return PlatformNetwork::Errors::SystemNotReady;
			case WSAVERNOTSUPPORTED: return PlatformNetwork::Errors::VersionNotSupported;
			case WSANOTINITIALISED: return PlatformNetwork::Errors::NotInitialized;
			case WSAEDISCON: return PlatformNetwork::Errors::Disconnected;
			case WSAEINVALIDPROCTABLE: return PlatformNetwork::Errors::InvalidProcedureTable;
			case WSAEINVALIDPROVIDER: return PlatformNetwork::Errors::InvalidProvider;
			case WSAEPROVIDERFAILEDINIT: return PlatformNetwork::Errors::ProviderFailedToInitialize;
			case WSASYSCALLFAILURE: return PlatformNetwork::Errors::SystemCallFailure;
			case WSASERVICE_NOT_FOUND: return PlatformNetwork::Errors::ServiceNotFound;
			case WSATYPE_NOT_FOUND: return PlatformNetwork::Errors::TypeNotFound;
			case WSA_E_NO_MORE: return PlatformNetwork::Errors::NoMoreResult2;
			case WSA_E_CANCELLED: return PlatformNetwork::Errors::Canceled2;
			case WSAEREFUSED: return PlatformNetwork::Errors::Refused;
			case WSAHOST_NOT_FOUND: return PlatformNetwork::Errors::HostNotFound;
			case WSATRY_AGAIN: return PlatformNetwork::Errors::TryAgain;
			case WSANO_RECOVERY: return PlatformNetwork::Errors::NoRecoverable;
			case WSANO_DATA: return PlatformNetwork::Errors::NoData;
			case WSA_QOS_RECEIVERS: return PlatformNetwork::Errors::QOSReceivers;
			case WSA_QOS_NO_SENDERS: return PlatformNetwork::Errors::QOSNoSenders;
			case WSA_QOS_NO_RECEIVERS: return PlatformNetwork::Errors::QOSNoReceiver;
			case WSA_QOS_REQUEST_CONFIRMED: return PlatformNetwork::Errors::QOSRequestConfirmed;
			case WSA_QOS_ADMISSION_FAILURE: return PlatformNetwork::Errors::AdmissionFailure;
			case WSA_QOS_POLICY_FAILURE: return PlatformNetwork::Errors::QOSPolicyFailure;
			case WSA_QOS_BAD_STYLE: return PlatformNetwork::Errors::QOSBadStyle;
			case WSA_QOS_BAD_OBJECT: return PlatformNetwork::Errors::QOSBadObject;
			case WSA_QOS_TRAFFIC_CTRL_ERROR: return PlatformNetwork::Errors::QOSTrafficControlError;
			case WSA_QOS_GENERIC_ERROR: return PlatformNetwork::Errors::QOSGenericError;
			case WSA_QOS_ESERVICETYPE: return PlatformNetwork::Errors::QOSServiceType;
			case WSA_QOS_EFLOWSPEC: return PlatformNetwork::Errors::QOSFlowSpecific;
			case WSA_QOS_EPROVSPECBUF: return PlatformNetwork::Errors::QOSProviderSpecific;
			case WSA_QOS_EFILTERSTYLE: return PlatformNetwork::Errors::QOSFilterStyle;
			case WSA_QOS_EFILTERTYPE: return PlatformNetwork::Errors::QOSFilterType;
			case WSA_QOS_EFILTERCOUNT: return PlatformNetwork::Errors::QOSFilterCount;
			case WSA_QOS_EOBJLENGTH: return PlatformNetwork::Errors::QOSObjectLength;
			case WSA_QOS_EFLOWCOUNT: return PlatformNetwork::Errors::QOSLowCount;
			case WSA_QOS_EUNKOWNPSOBJ: return PlatformNetwork::Errors::QOSUnknownProviderSpecific;
			case WSA_QOS_EPOLICYOBJ: return PlatformNetwork::Errors::QOSPolicyObject;
			case WSA_QOS_EFLOWDESC: return PlatformNetwork::Errors::QOSFlowDescriptor;
			case WSA_QOS_EPSFLOWSPEC: return PlatformNetwork::Errors::QOSInconsistentFlowSpecific;
			case WSA_QOS_EPSFILTERSPEC: return PlatformNetwork::Errors::QOSInconsistentFilterSpecific;
			case WSA_QOS_ESDMODEOBJ: return PlatformNetwork::Errors::QOSShapeDiscardMode;
			case WSA_QOS_ESHAPERATEOBJ: return PlatformNetwork::Errors::QOSShapingRateObject;
			case WSA_QOS_RESERVED_PETYPE: return PlatformNetwork::Errors::QOSReceivedProviderType;
			case WSA_SECURE_HOST_NOT_FOUND: return PlatformNetwork::Errors::QOSSecureHostNotFound;
			case WSA_IPSEC_NAME_POLICY_ERROR: return PlatformNetwork::Errors::IPSecPolicy;
			}

			return PlatformNetwork::Errors::NoError;
		}

		PlatformNetwork::Errors PlatformNetwork::Initialize(void)
		{
			WSADATA data;
			return  GetError(WSAStartup(MAKEWORD(2, 2), &data));
		}

		PlatformNetwork::Errors PlatformNetwork::Shotdown(void)
		{
			return  GetError(WSACleanup());
		}

		PlatformNetwork::Handle PlatformNetwork::Create(AddressFamilies AddressFamily, Types Type, IPProtocols IPProtocol)
		{
			return (Handle)socket(GetAddressFamiliy(AddressFamily), GetType(Type), GetIPProtocol(IPProtocol));
		}

		PlatformNetwork::Errors PlatformNetwork::Close(Handle Handle)
		{
			return GetError(closesocket(Handle));
		}

		PlatformNetwork::Errors PlatformNetwork::Bind(Handle Handle, AddressFamilies AddressFamily, uint16 Port)
		{
			sockaddr_in &&address = GetAddressInfo(AddressFamily, Port);

			return  GetError(bind(Handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)));
		}

		PlatformNetwork::Errors PlatformNetwork::SetNonBlocking(Handle Handle, bool Enabled)
		{
			DWORD enabled = (Enabled ? 1 : 0);

			return GetError(ioctlsocket(Handle, FIONBIO, &enabled));
		}

		PlatformNetwork::Errors PlatformNetwork::Send(Handle Handle, const byte *Buffer, uint32 Length, AddressFamilies AddressFamily, uint16 Port, SendModes Mode)
		{
			sockaddr_in address = GetAddressInfo(AddressFamily, Port);

			if (sendto(Handle, reinterpret_cast<const char*>(Buffer), Length, GetSendFlags(Mode), reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) != NO_ERROR)
				return GetError(WSAGetLastError());

			return Errors::NoError;
		}
	}
}