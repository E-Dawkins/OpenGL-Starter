#pragma once

namespace MathHelpers
{
	const glm::vec3 up = glm::vec3(0, 1, 0);
	const glm::vec3 right = glm::vec3(1, 0, 0);
	const glm::vec3 forward = glm::vec3(0, 0, 1);

	// Returns a vector that is flat against a given _axis, for instance _v against (0, 1, 0) will produce (_v.x, 0, _v.z)
	static inline glm::vec3 FlattenVector(const glm::vec3& _v, const glm::vec3& _axis)
	{
		return _v * (glm::vec3(1) - glm::normalize(_axis));
	}
}