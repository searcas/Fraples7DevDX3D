#pragma once



namespace FraplesDev
{
	class Graphics;
	class CommandList
	{
	public:
		CommandList(Graphics &gfx);
		void Begin();
		void Execute();
		void Finish();

	private:

	};
}