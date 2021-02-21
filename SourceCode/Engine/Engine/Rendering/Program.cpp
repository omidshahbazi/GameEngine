// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		Program::Program(ThreadedDevice* Device, Handle Handle) :
			NativeType(Device, Handle)
		{
			QueryActiveConstants();
		}

		void Program::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Program, GetName().GetValue());
		}

		bool Program::SetFloat32(ConstantHash Hash, float32 Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetColor(ConstantHash Hash, const ColorUI8& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetVector2(ConstantHash Hash, const Vector2F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetVector3(ConstantHash Hash, const Vector3F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetVector4(ConstantHash Hash, const Vector4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetMatrix4(ConstantHash Hash, const Matrix4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Program::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Program::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		bool Program::SetFloat32(const String& Name, float32 Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetColor(const String& Name, const ColorUI8& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetVector2(const String& Name, const Vector2F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetVector3(const String& Name, const Vector3F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetVector4(const String& Name, const Vector4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Program::SetTexture(const String& Name, const TextureResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Program::SetSprite(const String& Name, const SpriteResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		Program::ConstantHash Program::GetConstantHash(const String& Name)
		{
			ConstantHash hash = GetHash(Name);

			if (m_ConstantsData.Contains(hash))
				return hash;

			return 0;
		}

		void Program::SetConstantsValue(const ConstantInfoMap& Constants)
		{
			for (auto& info : Constants)
			{
				auto& constant = info.GetSecond();

				if (!m_ConstantsData.Contains(constant.Hash))
					continue;

				m_ConstantsData[constant.Hash].Value = constant.Value;
			}
		}

		void Program::ApplyConstantsValue(IDevice* Device)
		{
			for (auto& item : m_ConstantsData)
			{
				auto& constant = item.GetSecond();

				SetConstantValueOnDevice(Device, constant.Handle, constant.Type, constant.Value);
			}
		}

		bool Program::SetConstantValue(Program::ConstantHash Hash, const AnyDataType& Value)
		{
			if (!m_ConstantsData.Contains(Hash))
				return false;

			m_ConstantsData[Hash].Value = Value;

			return true;
		}

		void Program::QueryActiveConstants(void)
		{
			ConstantDataList list;
			GetDevice()->QueryProgramActiveConstants(GetHandle(), list).Wait();

			for (auto& constant : list)
				m_ConstantsData[constant.Hash] = constant;
		}

		bool Program::SetConstantValueOnDevice(IDevice* Device, Program::ConstantHandle Handle, ProgramDataTypes Type, const AnyDataType& Value)
		{
			switch (Type)
			{
			case ProgramDataTypes::Float:
			{
				Device->SetProgramFloat32(Handle, Value.Get<float32>());
			} break;

			case ProgramDataTypes::Float2:
			{
				Device->SetProgramVector2(Handle, Value.Get<Vector2F>());
			} break;

			case ProgramDataTypes::Float3:
			{
				Device->SetProgramVector3(Handle, Value.Get<Vector3F>());
			} break;

			case ProgramDataTypes::Float4:
			{
				if (Value.GetValueType() == ValueTypes::ColorUI8)
					Device->SetProgramColor(Handle, Value.GetAsColorUI8());
				else
					Device->SetProgramVector4(Handle, Value.Get<Vector4F>());
			} break;

			case ProgramDataTypes::Matrix4:
			{
				Device->SetProgramMatrix4(Handle, Value.Get<Matrix4F>());
			} break;

			case ProgramDataTypes::Texture2D:
			{
				auto val = Value.Get<TextureResource*>();
				Texture::Types type = Texture::Types::TwoD;
				Texture::Handle texHandle = 0;
				if (val != nullptr && !val->IsNull())
				{
					Texture* tex = val->GetPointer();

					texHandle = tex->GetHandle();
					type = tex->GetType();
				}

				Device->SetProgramTexture(Handle, type, texHandle);
			} break;

			default:
				return false;
			}

			return true;
		}
	}
}