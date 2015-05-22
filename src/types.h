#ifndef _H_TYPES_H_
#define _H_TYPES_H_

#include <glm/glm.hpp>
#include <map>
#include <string>

#include "glwrap/texture.h"

struct transform_t {
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	glm::mat4 calculateWorld() const;
	glm::mat4 calculateView() const;

	glm::vec3 forward() const;
};

struct material_t {
	std::map<std::string, glm::vec4> attributes;

	texture2d* diffuseTexture;
	texture2d* specularTexture;
	texture2d* aoTexture;
};

struct renderable_t {
	transform_t	transform;
	material_t	material;
};

bool operator<(const renderable_t& a, const renderable_t& b);

enum renderPhase_t {
	renderPhase_Opaque, 
	renderPhase_Liquid, 
	renderPhase_Transparent
};

#endif