#pragma once

namespace lve
{
	class Triangle
	{
	public:
		Triangle(glm::vec2 topVertex, glm::vec2 rightVertex, glm::vec2 leftVertex) :
			mTopVertex(topVertex), mRightVertex(rightVertex), mLeftVertex(leftVertex)
		{
		}
		const glm::vec2 mTopVertex;
		const glm::vec2 mRightVertex;
		const glm::vec2 mLeftVertex;
	};
}