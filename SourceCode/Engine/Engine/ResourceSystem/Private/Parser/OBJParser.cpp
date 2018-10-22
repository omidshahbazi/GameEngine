// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\Parser\OBJParser.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace Platform;

	namespace ResourceSystem
	{
		namespace Private
		{
			namespace Parser
			{
				float ReadFloat(uint64 &Index, char8 *Data)
				{
					String value;

					char8 ch;
					while ((ch = Data[Index++]) != ' ' && ch != '\n' && ch != '\r' && ch != '\0')
						value += ch;

					return atof(value.GetValue());
				}

				void OBJParser::Parse(const WString &Path)
				{
					PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

					if (handle == 0)
						return;

					uint64 fileSize = PlatformFile::Size(handle);
					char8 *data = ResourceSystemAllocators::Allocate<char8>(fileSize + 1);
					fileSize =PlatformFile::Read(handle, data, fileSize);
					data[fileSize] = '\0';

					uint64 index = 0;
					while (index != fileSize)
					{
						String type;

						char8 ch;
						while ((ch = data[index++]) != ' ')
							type += ch;

						if (type == "v")
						{
							float x = ReadFloat(index, data);
							float y = ReadFloat(index, data);
							float z = ReadFloat(index, data);
						}
						else if (type == "vt")
						{
							float u = ReadFloat(index, data);
							float v = ReadFloat(index, data);
						}
						else if (type == "vn")
						{
							float x = ReadFloat(index, data);
							float y = ReadFloat(index, data);
							float z = ReadFloat(index, data);
						}
						else if (type == "f")
						{
							float x = ReadFloat(index, data);
							float y = ReadFloat(index, data);
							float z = ReadFloat(index, data);
						}
						else
						{
							while ((ch = data[index++]) != '\n' && ch != '\r' && ch != '\0');
						}
					}
				}
			}
		}
	}
}