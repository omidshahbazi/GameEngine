// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MeshAssetParser\MeshParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ResourceAssetParser
	{
		void ParseSubMeshInfo(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo)
		{
			SubMeshInfo.Type = (PolygonTypes)Buffer.ReadValue<int32>();

			SubMeshInfo.Layout = (VertexLayouts)Buffer.ReadValue<int32>();

			uint32 vertexCount = Buffer.ReadValue<uint32>();

			SubMeshInfo.Vertices.Recap(vertexCount);

			for (uint32 j = 0; j < vertexCount; ++j)
			{
				Vector3F pos;
				pos.X = Buffer.ReadValue<float32>();
				pos.Y = Buffer.ReadValue<float32>();
				pos.Z = Buffer.ReadValue<float32>();

				Vector3F norm;
				norm.X = Buffer.ReadValue<float32>();
				norm.Y = Buffer.ReadValue<float32>();
				norm.Z = Buffer.ReadValue<float32>();

				Vector2F uv;
				uv.X = Buffer.ReadValue<float32>();
				uv.Y = Buffer.ReadValue<float32>();

				SubMeshInfo.Vertices.Add({ pos, norm, uv });
			}

			uint32 idxCount = Buffer.ReadValue<uint32>();

			SubMeshInfo.Indices.Recap(idxCount);

			for (uint32 j = 0; j < idxCount; ++j)
			{
				uint32 idx = Buffer.ReadValue<uint32>();

				SubMeshInfo.Indices.Add(idx);
			}
		}

		void MeshParser::Parse(const ByteBuffer& Buffer, MeshInfo& MeshInfo)
		{
			uint32 subMeshCount = Buffer.ReadValue<uint32>();

			for (uint32 i = 0; i < subMeshCount; ++i)
			{
				SubMeshInfo* subMeshInfo = ReinterpretCast(SubMeshInfo*, AllocateMemory(MeshInfo.SubMeshes.GetAllocator(), sizeof(SubMeshInfo)));

				Construct(subMeshInfo, MeshInfo.SubMeshes.GetAllocator());

				ParseSubMeshInfo(Buffer, *subMeshInfo);

				MeshInfo.SubMeshes.Add(subMeshInfo);
			}
		}

		void MeshParser::Parse(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo)
		{
			uint32 subMeshCount = Buffer.ReadValue<uint32>();

			ParseSubMeshInfo(Buffer, SubMeshInfo);
		}

		uint64 MeshParser::GetDumpSize(const MeshInfo& MeshInfo)
		{
			uint32 size = sizeof(uint32);

			for (auto& subMeshPtr : MeshInfo.SubMeshes)
			{
				auto& subMesh = *subMeshPtr;

				size += sizeof(int32);
				size += sizeof(int32);

				size += sizeof(uint32);
				size += subMesh.Vertices.GetSize() * sizeof(Vertex);

				size += sizeof(uint32);
				size += subMesh.Indices.GetSize() * sizeof(uint32);
			}

			return size;
		}

		void MeshParser::Dump(ByteBuffer& Buffer, const MeshInfo& MeshInfo)
		{
			uint64 desiredSize = GetDumpSize(MeshInfo);
			Buffer.Recap(Buffer.GetSize() + desiredSize);

			Buffer.Append(MeshInfo.SubMeshes.GetSize());
			for (auto& subMeshPtr : MeshInfo.SubMeshes)
			{
				auto& subMesh = *subMeshPtr;

				Buffer.Append((int32)subMesh.Type);
				Buffer.Append((int32)subMesh.Layout);

				Buffer.Append(subMesh.Vertices.GetSize());
				for (auto& vertex : subMesh.Vertices)
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
				for (auto& index : subMesh.Indices)
					Buffer.Append(index);
			}
		}
	}
}