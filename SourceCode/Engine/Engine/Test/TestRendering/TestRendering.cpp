
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Profiling.h>
#include <Containers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <Utility\HighResolutionTime.h>
#include <Utility\Window.h>
#include <Utility\FileSystem.h>
#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <iostream>


using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Profiler;
using namespace Engine::Containers;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

float32 Area(const FT_Outline &Outline)
{
	float32 A = 0.0F;

	for (int32 p = Outline.n_points - 1, q = 0; q < Outline.n_points; p = q++)
	{
		const FT_Vector &pval = Outline.points[p];
		const FT_Vector &qval = Outline.points[q];

		A += (pval.x * qval.y) - (qval.x * pval.y);
	}

	return (A * 0.5f);
}

bool InsideTriangle(const FT_Vector &A, const FT_Vector &B, const FT_Vector &C, const FT_Vector &P)
{
	float32 ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
	float32 cCROSSap, bCROSScp, aCROSSbp;

	ax = C.x - B.x; ay = C.y - B.y;
	bx = A.x - C.x; by = A.y - C.y;
	cx = B.x - A.x; cy = B.y - A.y;
	apx = P.x - A.x; apy = P.y - A.y;
	bpx = P.x - B.x; bpy = P.y - B.y;
	cpx = P.x - C.x; cpy = P.y - C.y;

	aCROSSbp = ax * bpy - ay * bpx;
	cCROSSap = cx * apy - cy * apx;
	bCROSScp = bx * cpy - by * cpx;

	return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
}

bool Snip(const FT_Outline &Outline, int32 u, int32 v, int32 w, int32 n, const Vector<int32> &V)
{
	int32 p;

	const FT_Vector &A = Outline.points[V[u]];
	const FT_Vector &B = Outline.points[V[v]];
	const FT_Vector &C = Outline.points[V[w]];

	if (0 > (((B.x - A.x) * (C.y - A.y)) - ((B.y - A.y) * (C.x - A.x))))
		return false;

	for (p = 0; p < n; p++) 
	{
		if ((p == u) || (p == v) || (p == w))
			continue;
		const FT_Vector &P = Outline.points[V[p]];
		if (InsideTriangle(A, B, C, P))
			return false;
	}

	return true;
}

void MakeMeshFromOutline(const FT_Outline &Outline, MeshInfo &MeshInfo)
{
	SubMeshInfo subMeshInfo;
	subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position;

	for (int32 i = 0; i < Outline.n_points; ++i)
	{
		const FT_Vector &point = Outline.points[i];

		subMeshInfo.Vertices.Add({ Vector3F(point.x, point.y, 0), Vector2F(0, 0) });
	}

	int32 n = subMeshInfo.Vertices.GetSize();
	if (n < 3)
		return;

	Vector<int32> V(n);
	if (Area(Outline) > 0)
	{
		for (int32 v = 0; v < n; v++)
			V.Add(v);
	}
	else
	{
		for (int32 v = 0; v < n; v++)
			V.Add((n - 1) - v);
	}

	int32 nv = n;
	int32 count = 2 * nv;
	for (int32 v = nv - 1; nv > 2; )
	{
		if ((count--) <= 0)
			return;

		int32 u = v;
		if (nv <= u)
			u = 0;
		v = u + 1;
		if (nv <= v)
			v = 0;
		int32 w = v + 1;
		if (nv <= w)
			w = 0;

		if (Snip(Outline, u, v, w, nv, V))
		{
			int32 a, b, c, s, t;
			a = V[u];
			b = V[v];
			c = V[w];
			subMeshInfo.Indices.Insert(0, a);
			subMeshInfo.Indices.Insert(0, b);
			subMeshInfo.Indices.Insert(0, c);
			for (s = v, t = v + 1; t < nv; s++, t++)
				V[s] = V[t];
			nv--;
			count = 2 * nv;
		}
	}

	MeshInfo.SubMeshes.Add(subMeshInfo);
}

void main()
{


	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "D:/Projects/GameEngineAssets/Assets/calibri.ttf", 0, &face))
	{
		return;
	}

	const float DPI = 300;
	if (FT_Set_Char_Size(face, 0, 0, DPI, DPI))
	{
		return;
	}

	if (FT_Set_Pixel_Sizes(face, 0, 30))
	{
		return;
	}



	//if (FT_Load_Char(face, 'd', FT_LOAD_RENDER))
	//	return;
	//FT_Glyph glyph;
	//if (FT_Get_Glyph(face->glyph, &glyph))
	//	return;
	//if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, nullptr, true))
	//	return;

	//char str[] = { 'D' ,'e' ,'l' ,'a' ,'r' ,'a' ,'m' };

	//for (int k = 0; k < sizeof(str); ++k)
	//{
	//	if (FT_Load_Char(face, str[k], FT_LOAD_RENDER))
	//		return;

	//	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))
	//		return;

	//	for (int y = 0; y < face->glyph->bitmap.rows; ++y)
	//	{
	//		for (int x = 0; x < face->glyph->bitmap.width; ++x)
	//		{
	//			uint8 value = 0;

	//			value |= face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.pitch)];

	//			std::cout << (value < 128 ? ' ' : '*');
	//		}

	//		std::cout << std::endl;
	//	}

	//	std::cout << std::endl;
	//}

	char ch = 'S';

	if (FT_Load_Char(face, ch, FT_LOAD_RENDER))
		return;

	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))
		return;

	MeshInfo meshInfo;

	MakeMeshFromOutline(face->glyph->outline, meshInfo);

	FileSystem::SetWorkingPath(L"D:\\Projects\\GameEngineAssets");

	RealtimeProfiler::Create(RootAllocator::GetInstance());
	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());
	DeviceInterface *device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);
	SceneManager *sceneMgr = SceneManager::Create(RootAllocator::GetInstance());

	Window window("Test Rendering");
	window.Initialize();
	window.SetSize({ WIDTH, HEIGHT });
	window.SetTitle("Test Rendering");

	device->SetWindow(&window);

	device->Initialize();

	ResourceManager *resources = ResourceManager::Create(RootAllocator::GetInstance());
	ProgramResource shader = resources->Load<Program>("TextShader.shader");

	Mesh *mesh = device->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw);

	Material characterMat;
	characterMat.SetQueue(RenderQueues::HUD);
	Pass pass(*shader);
	auto st = pass.GetRenderState();
	st.CullMode = IDevice::CullModes::None;
	st.DepthTestFunction = IDevice::TestFunctions::Never;
	pass.SetRenderState(st);
	characterMat.AddPass(pass);

	float32 fps = 0;
	uint32 frameCount = 0;
	uint64 nextCheckTime = HighResolutionTime::GetTime().GetMilliseconds() + 1000;

	Matrix4F projection;
	projection.MakeOrthographicProjectionMatrix(WIDTH, HEIGHT, 0.1, 1000);

	while (!window.ShouldClose())
	{
		PlatformWindow::PollEvents();

		Matrix4F idMat;
		idMat.MakeIdentity();
		idMat.SetScale(0.1F, 0.1F, 0.1F);
		idMat.SetPosition(-300, 0, -100);

		idMat = projection * idMat;

		device->BeginRender();

		device->DrawMesh(mesh, idMat, &characterMat);

		device->EndRender();

		uint64 time = HighResolutionTime::GetTime().GetMilliseconds();

		++frameCount;

		if (time >= nextCheckTime)
		{
			fps = frameCount;
			frameCount = 0;
			nextCheckTime = time + 1000;



			std::cout << fps << std::endl;
		}
	}
}