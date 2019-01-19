// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\Parser\OBJParser.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			namespace Parser
			{
				float32 ReadFloat(uint64 &Index, const char8 *Data)
				{
					String value;

					char8 ch;
					while ((ch = Data[Index++]) == '.' || ch == '-' || CharacterUtility::IsDigit(ch))
						value += ch;

					if (ch != ' ' && ch != '\n' && ch != '\r')
						--Index;

					return StringUtility::ToFloat32(value, -1);
				}

				bool ReadIndex(uint64 &Index, const char8 *Data, Vector3F &Value)
				{
					Value.X = ReadFloat(Index, Data);

					if (Value.X == -1)
						return false;

					if (Data[Index] == '/')
					{
						++Index;
						Value.Y = ReadFloat(Index, Data);

						if (Data[Index] == '/')
						{
							++Index;
							Value.Z = ReadFloat(Index, Data);
						}
					}

					return true;
				}

				void OBJParser::Parse(const byte *Data, MeshInfo &MeshInfo)
				{
					const char8 *data = ReinterpretCast(const char8*, Data);
					uint32 fileSize = CharacterUtility::GetLength(data);

					SubMeshInfo subMeshInfo;
					subMeshInfo.Layout = SubMeshInfo::VertexLayouts::Position;

					uint64 index = 0;
					uint32 vertexIndex = 0;
					uint8 stage = 0;
					while (index != fileSize)
					{
						String type;

						char8 ch;
						while ((ch = data[index++]) != ' ')
							type += ch;

						if (type == "v")
						{
							float32 x = ReadFloat(index, data);
							float32 y = ReadFloat(index, data);
							float32 z = ReadFloat(index, data);

							subMeshInfo.Vertices.Add({ Vector3F(x, y, z), Vector3F(), Vector2F() });

						}
						else if (type == "vt")
						{
							if (stage == 0)
							{
								++stage;

								vertexIndex = 0;
								subMeshInfo.Layout |= SubMeshInfo::VertexLayouts::UV;
							}

							float32 u = ReadFloat(index, data);
							float32 v = ReadFloat(index, data);

							if (vertexIndex < subMeshInfo.Vertices.GetSize())
								subMeshInfo.Vertices[vertexIndex++].UV = { u, v };
						}
						else if (type == "vn")
						{
							if (stage == 1)
							{
								++stage;

								vertexIndex = 0;
								subMeshInfo.Layout |= SubMeshInfo::VertexLayouts::Normal;
							}

							float32 x = ReadFloat(index, data);
							float32 y = ReadFloat(index, data);
							float32 z = ReadFloat(index, data);

							if (vertexIndex < subMeshInfo.Vertices.GetSize())
								subMeshInfo.Vertices[vertexIndex++].Normal = { x, y, z };
						}
						else if (type == "f")
						{
							Vector3F v1;
							ReadIndex(index, data, v1);
							Vector3F v2;
							ReadIndex(index, data, v2);
							Vector3F v3;
							ReadIndex(index, data, v3);

							subMeshInfo.Indices.Add(v1.X - 1);
							subMeshInfo.Indices.Add(v2.X - 1);
							subMeshInfo.Indices.Add(v3.X - 1);

							Vector3F v4;
							if (ReadIndex(index, data, v4))
							{
								subMeshInfo.Indices.Add(v1.X - 1);
								subMeshInfo.Indices.Add(v3.X - 1);
								subMeshInfo.Indices.Add(v4.X - 1);
							}
						}
						else
						{
							while ((ch = data[index++]) != '\n' && ch != '\r' && index < fileSize);
						}
					}

					MeshInfo.SubMeshes.Add(subMeshInfo);
				}
			}
		}
	}
}