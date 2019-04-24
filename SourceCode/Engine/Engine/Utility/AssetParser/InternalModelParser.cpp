// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\AssetParser\InternalModelParser.h>
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
			void ParseSubMeshInfo(const ByteBuffer &Buffer, SubMeshInfo &SubMeshInfo, uint64 &Index)
			{
				SubMeshInfo.Layout = (Mesh::SubMesh::VertexLayouts)Buffer.ReadValue<int32>(Index);
				Index += sizeof(int32);

				uint32 vertexCount = Buffer.ReadValue<uint32>(Index);

				SubMeshInfo.Vertices.Recap(vertexCount);

				Index += sizeof(uint32);
				for (uint32 j = 0; j < vertexCount; ++j)
				{
					Vector3F pos;
					pos.X = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);
					pos.Y = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);
					pos.Z = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);

					Vector3F norm;
					norm.X = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);
					norm.Y = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);
					norm.Z = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);

					Vector2F uv;
					uv.X = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);
					uv.Y = Buffer.ReadValue<float32>(Index);
					Index += sizeof(float32);

					SubMeshInfo.Vertices.Add({ pos, norm, uv });
				}

				uint32 idxCount = Buffer.ReadValue<uint32>(Index);

				SubMeshInfo.Indices.Recap(idxCount);

				Index += sizeof(uint32);
				for (uint32 j = 0; j < idxCount; ++j)
				{
					uint32 idx = Buffer.ReadValue<uint32>(Index);
					Index += sizeof(uint32);

					SubMeshInfo.Indices.Add(idx);
				}
			}

			void InternalModelParser::Parse(const ByteBuffer &Buffer, MeshInfo &MeshInfo)
			{
				uint64 index = 0;
				uint32 subMeshCount = Buffer.ReadValue<uint32>(index);
				index += sizeof(uint32);

				for (uint32 i = 0; i < subMeshCount; ++i)
				{
					SubMeshInfo subMeshInfo;

					ParseSubMeshInfo(Buffer, subMeshInfo, index);

					MeshInfo.SubMeshes.Add(subMeshInfo);
				}
			}

			void InternalModelParser::Parse(const ByteBuffer &Buffer, SubMeshInfo &SubMeshInfo)
			{
				uint64 index = 0;

				uint32 subMeshCount = Buffer.ReadValue<uint32>(index);
				index += sizeof(uint32);

				ParseSubMeshInfo(Buffer, SubMeshInfo, index);
			}

			void InternalModelParser::Dump(ByteBuffer &Buffer, MeshInfo & MeshInfo)
			{
				uint32 desiredSize = 0;
				for each (auto &subMesh in MeshInfo.SubMeshes)
				{
					desiredSize += sizeof(int32);

					desiredSize += sizeof(int32);
					desiredSize += subMesh.Vertices.GetSize() * sizeof(Vertex);

					desiredSize += sizeof(int32);
					desiredSize += subMesh.Indices.GetSize() * sizeof(uint32);
				}

				Buffer.Recap(Buffer.GetSize() + desiredSize);

				Buffer.Append(MeshInfo.SubMeshes.GetSize());
				for each (auto &subMesh in MeshInfo.SubMeshes)
				{
					Buffer.Append((int32)subMesh.Layout);

					Buffer.Append(subMesh.Vertices.GetSize());
					for each (auto &vertex in subMesh.Vertices)
					{
						Buffer.Append(vertex.Position.X);
						Buffer.Append(vertex.Position.Y);
						Buffer.Append(vertex.Position.Z);

						Buffer.Append(vertex.Normal.X);
						Buffer.Append(vertex.Normal.Y);
						Buffer.Append(vertex.Normal.Z);

						Buffer.Append(vertex.UV.X);
						Buffer.Append(vertex.UV.Y);
					}

					Buffer.Append(subMesh.Indices.GetSize());
					for each (auto &index in subMesh.Indices)
						Buffer.Append(index);
				}
			}
		}
	}
}