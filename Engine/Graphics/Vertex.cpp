#include "Vertex.h"

namespace Craft
{
	Vertex::Vertex(const Vector3& position, const Vector2 texCoord, const Vector3& normal)
		: position(position), texCoord(texCoord), normal(normal)
	{
	}
}