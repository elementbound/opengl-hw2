#ifndef _H_APP_H_
#define _H_APP_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <map>
#include <set>

#include "glwrap/window.h"
#include "glwrap/resizable_window.h"
#include "glwrap/shader.h"
#include "glwrap/mesh.h"
#include "glwrap/texture.h"

#include "types.h"

class app_Scene : public resizable_window {
	private: 
		//Resources
		std::map<std::string, separate_mesh*> 	m_Meshes;
		std::map<std::string, texture2d*> 		m_Textures;
		std::map<std::string, shader_program*> 	m_Shaders;

		std::set<std::pair<renderPhase_t, renderable_t>> m_Renderables;

		transform_t m_Camera;
		glm::mat4	m_CameraProjection;
		float		m_CameraFOV = glm::radians(60.0f);

		//

		bool load_mesh(const char* name, const char* file);
		bool load_texture(const char* name, const char* file);
		bool load_shader(const char* name, const char* vertFile, const char* fragFile);

		//

		bool glew_init();
		bool gl_init();
		bool load_resources();
		bool init_scene();

		void update();

		void free_resources();

	protected: 
		void on_open();
		void on_resize(int w, int h);
		void on_refresh();
		
		void on_key(int key, int scancode, int action, int mods);

		void on_close();
};

#endif