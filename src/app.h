#ifndef _H_APP_H_
#define _H_APP_H_

#include <GL/glew.h>

#include <string>
#include <map>
#include <set>

#include "types.h"

class app_Scene {
	private: 
		//Resources
		std::map<std::string, separate_mesh*> 	m_Meshes;
		std::map<std::string, texture2d*> 		m_Textures;
		std::map<std::string, shader_program*> 	m_Shaders;

		std::set<std::pair<renderPhase_t, renderable_t>> m_Renderables;

		transform_t m_Camera;
};

#endif