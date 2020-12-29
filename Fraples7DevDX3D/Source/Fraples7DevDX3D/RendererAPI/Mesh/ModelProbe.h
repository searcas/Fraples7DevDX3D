#pragma once

namespace FraplesDev
{
	class ModelProbe
	{
	public:
		virtual bool PushNode(class Node& node) = 0;
		virtual void PopNode(class Node& node) = 0;
		virtual ~ModelProbe()=default;
	private:

	};


}
