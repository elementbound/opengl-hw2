#ifndef _H_TYPES_H_
#define _H_TYPES_H_

#include <glm/glm.hpp>
#include <map>
#include <string>

struct transform_t {
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	glm::mat4 calculateWorld() const;
	glm::mat4 calculateView() const;

	glm::vec3 forward() const;
};

typedef std::map<std::string, glm::vec4> material_t;

struct renderable_t {
	transform_t	transform;
	material_t	material;
};

enum {
	renderPhase_Opaque, 
	renderPhase_Liquid, 
	renderPhase_Transparent
} renderPhase_t;

#endif