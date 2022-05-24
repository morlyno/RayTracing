#pragma once

#include "Material.h"

#include <glm/glm.hpp>
#include <memory>

namespace Component {

	struct SphereComponent
	{
		glm::vec3 Center;
		float Radius;
	};

	struct MaterialComponent
	{
		std::shared_ptr<Material> Material;
	};

}
