#include "types.h"

#include "glwrap/util.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 transform_t::calculateWorld() const {
	glm::mat4 m_Transform;
	m_Transform = glm::mat4();
	
	//Apply translation
	m_Transform = glm::translate(m_Transform, pos);
	
	//Apply rotation
	m_Transform = glm::rotate(m_Transform, rot.x, glm::vec3(1.0f,0.0f,0.0f));
	m_Transform = glm::rotate(m_Transform, rot.y, glm::vec3(0.0f,1.0f,0.0f));
	m_Transform = glm::rotate(m_Transform, rot.z, glm::vec3(0.0f,0.0f,1.0f));

	//Apply scale
	m_Transform = glm::scale(m_Transform, scale);

	return m_Transform;
}

glm::mat4 transform_t::calculateView() const {
	return glm::lookAt(pos, forward(), glm::vec3(0.0, 0.0, 1.0));
}

glm::vec3 transform_t::forward() const {
	return dirvec(rot.z, rot.x);
}

bool operator<(const renderable_t& a, const renderable_t& b) {
	return &a < &b;
}