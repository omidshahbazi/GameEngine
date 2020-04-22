// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\AssetParser\OBJParser.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;
	using namespace Rendering;

	namespace Utility
	{
		namespace AssetParser
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

			void OBJParser::Parse(const ByteBuffer &Buffer, MeshInfo &MeshInfo)
			{
				SubMeshInfo *subMeshInfo = ReinterpretCast(SubMeshInfo*, AllocateMemory(MeshInfo.SubMeshes.GetAllocator(), sizeof(SubMeshInfo)));

				Construct(subMeshInfo, MeshInfo.SubMeshes.GetAllocator());

				Parse(Buffer, *subMeshInfo);

				MeshInfo.SubMeshes.Add(subMeshInfo);
			}

			void OBJParser::Parse(const ByteBuffer &Buffer, SubMeshInfo &SubMeshInfo)
			{
				const char8 *data = ReinterpretCast(const char8*, Buffer.GetBuffer());
				const uint64 size = Buffer.GetSize();

				SubMeshInfo.Type = SubMesh::PolygonTypes::Triangles;
				SubMeshInfo.Layout = SubMesh::VertexLayouts::Position;

				uint64 index = 0;
				uint32 vertexIndex = 0;
				uint8 stage = 0;
				while (index != size)
				{
					String type;

					char8 ch;

					while (index < size)
					{
						ch = data[index++];

						if (ch == ' ' ||
							ch == '\t' ||
							ch == '\n' ||
							ch == '\r' ||
							ch == '\0')
							break;

						type += ch;
					}

					if (type == "v")
					{
						float32 x = ReadFloat(index, data);
						float32 y = ReadFloat(index, data);
						float32 z = ReadFloat(index, data);

						SubMeshInfo.Vertices.Add({ Vector3F(x, y, z), Vector3F(), Vector2F() });
					}
					else if (type == "vt")
					{
						if (stage == 0)
						{
							++stage;

							vertexIndex = 0;
							SubMeshInfo.Layout |= SubMesh::VertexLayouts::UV;
						}

						float32 u = ReadFloat(index, data);
						float32 v = ReadFloat(index, data);

						if (vertexIndex < SubMeshInfo.Vertices.GetSize())
							SubMeshInfo.Vertices[vertexIndex++].UV = { u, v };
					}
					else if (type == "vn")
					{
						if (stage == 1)
						{
							++stage;

							vertexIndex = 0;
							SubMeshInfo.Layout |= SubMesh::VertexLayouts::Normal;
						}

						float32 x = ReadFloat(index, data);
						float32 y = ReadFloat(index, data);
						float32 z = ReadFloat(index, data);

						if (vertexIndex < SubMeshInfo.Vertices.GetSize())
							SubMeshInfo.Vertices[vertexIndex++].Normal = { x, y, z };
					}
					else if (type == "f")
					{
						Vector3F v1;
						ReadIndex(index, data, v1);
						Vector3F v2;
						ReadIndex(index, data, v2);
						Vector3F v3;
						ReadIndex(index, data, v3);

						SubMeshInfo.Indices.Add(v1.X - 1);
						SubMeshInfo.Indices.Add(v2.X - 1);
						SubMeshInfo.Indices.Add(v3.X - 1);

						Vector3F v4;
						if (ReadIndex(index, data, v4))
						{
							SubMeshInfo.Indices.Add(v1.X - 1);
							SubMeshInfo.Indices.Add(v3.X - 1);
							SubMeshInfo.Indices.Add(v4.X - 1);
						}
					}
				}
			}

			void OBJParser::Dump(ByteBuffer & Buffer, MeshInfo & MeshInfo)
			{
				for each (const auto &submesPtr in MeshInfo.SubMeshes)
				{
					auto &subMesh = *submesPtr;

					if (BitwiseUtils::IsEnabled(subMesh.Layout, SubMesh::VertexLayouts::Position))
						for each (const auto &vertex in subMesh.Vertices)
						{
							Buffer.Append('v');
							Buffer.Append(' ');
							Buffer.Append(vertex.Position.X);
							Buffer.Append(' ');
							Buffer.Append(vertex.Position.Y);
							Buffer.Append(' ');
							Buffer.Append(vertex.Position.Z);
							Buffer.Append('\n');
						}

					if (BitwiseUtils::IsEnabled(subMesh.Layout, SubMesh::VertexLayouts::Normal))
						for each (const auto &vertex in subMesh.Vertices)
						{
							Buffer.Append('v');
							Buffer.Append('n');
							Buffer.Append(' ');
							Buffer.Append(vertex.Normal.X);
							Buffer.Append(' ');
							Buffer.Append(vertex.Normal.Y);
							Buffer.Append(' ');
							Buffer.Append(vertex.Normal.Z);
							Buffer.Append('\n');
						}

					if (BitwiseUtils::IsEnabled(subMesh.Layout, SubMesh::VertexLayouts::UV))
						for each (const auto &vertex in subMesh.Vertices)
						{
							Buffer.Append('v');
							Buffer.Append('t');
							Buffer.Append(' ');
							Buffer.Append(vertex.UV.X);
							Buffer.Append(' ');
							Buffer.Append(vertex.UV.Y);
							Buffer.Append('\n');
						}

					for (int32 i = 0; i < subMesh.Indices.GetSize(); )
					{
						for (int32 j = 0; j < 3; ++j)
						{
							uint32 index = subMesh.Indices[i++];

							Buffer.Append('f');
							Buffer.Append(' ');
							Buffer.Append(index);
							Buffer.Append('/');
							Buffer.Append(index);
							Buffer.Append('/');
							Buffer.Append(index);
							Buffer.Append('\n');
						}
					}
				}
			}
		}
	}
}