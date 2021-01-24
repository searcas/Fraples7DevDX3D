#pragma once


namespace FraplesDev
{
	class Graphics;
	class Window;
	namespace QA
	{
		void TestDynamicMeshLoading();
		void TestMaterialSystemLoading(class Graphics& gfx);
		void TestDynamicConstant();
		void TestScaleMatrixTranslation();
		void D3DTestScratchPad(class Window& wn);
		void TestNumpy();
	}
}