#include "app.h"

#include "glwrap/util.h"
#include "glwrap/meshutil.h"
#include "glwrap/texture_util.h"

#include <iostream>
#include <fstream>

#include <utility>
#include <tuple>
#include <vector>

//Not strictly for existence, more like for accessibility
//Which I basically need this for, so move on... 
bool file_exists(const char* fname)
{
	std::ifstream fs(fname);
	return fs.good();
}

//===========================================================================================//
//Resource loading

bool app_Scene::load_mesh(const char* name, const char* file) {
	if(!file_exists(file))
		return false;

	separated_mesh* resultMesh = new resultMesh;
	meshutil::load_obj(file, *resultMesh);
	m_Meshes.insert({name, resultMesh});

	return true;
}

bool app_Scene::load_texture(const char* name, const char* file) {
	if(!file_exists(file))
		return false;

	texture2d* resultTexture = new texture2d;
	resultTexture->upload(texutil::load_png(file), GL_RGBA);
	m_Textures.insert({name, resultTexture});

	return true;
}

bool app_Scene::load_shader(const char* name, const char* vertFile, const char* fragFile) {
	if(!file_exists(vertFile) || !file_exists(fragFile))
		return false;

	shader_program* resultShader = new shader_program;
	resultShader->create();

	if(!resultShader.attach(read_file(vertFile).c_str(), shader_program::shader_type::vertex)) {
		delete resultShader;
		return false;
	}

	if(!resultShader.attach(read_file(fragFile).c_str(), shader_program::shader_type::fragment)) {
		delete resultShader;
		return false;
	}

	glBindFragDataLocation(resultShader.handle(), 0, "outColor");
	resultShader->link();

	m_Shader.insert({name, resultShader});
	return true;
}

//===========================================================================================//
//App init

bool app_Scene::glew_init() {
	this->make_current();
	
	if(glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW init fail" << std::endl;
		return 0;
	}

	if(!GLEW_VERSION_4_4)
	{
		std::cerr << "OpenGL 4.4 not supported" << std::endl;
		return 0; 
	}
	
	return 1;
}

bool app_Scene::gl_init() {
	glEnable(GL_DEPTH_TEST);
	
	return 1;
}

bool app_Scene::load_resources() {
	std::vector<std::pair<const char*, const char*>> meshesToLoad = {
		{"room",			"data/meshes/room.obj"},
		{"table", 			"data/meshes/table.obj"},

		{"whisky", 			"data/meshes/whisky.obj"},
		{"whisky-glass",	"data/meshes/whisky-glass.obj"},

		{"ashtray", 		"data/meshes/ashtray.obj"},
		{"armchair", 		"data/meshes/armchair.obj"},
		{"projector", 		"data/meshes/projector.obj"}
	};

	std::vector<std::pair<const char*, const char*>> texturesToLoad = {
		{"room-diffuse", 		"data/textures/room-diffuse.png"}, 
		{"room-ao", 			"data/textures/room-ao.png"}, 
		{"room-specular", 		"data/textures/room-specular.png"}, 

		{"table-diffuse", 		"data/textures/table-diffuse.png"}, 
		{"table-ao", 			"data/textures/table-ao.png"}, 
		{"table-specular", 		"data/textures/table-specular.png"}, 

		{"armchair-diffuse", 	"data/textures/armchair-diffuse.png"}, 
		{"armchair-ao", 		"data/textures/armchair-ao.png"}, 
		{"armchair-specular", 	"data/textures/armchair-specular.png"}, 

		{"projector-diffuse", 	"data/textures/projector-diffuse.png"}, 
		{"projector-ao", 		"data/textures/projector-ao.png"}, 
		{"projector-specular", 	"data/textures/projector-specular.png"}
	};

	std::vector<std::tuple<const char*, const char*, const char*>> shadersToLoad = {
		//
	};

	std::cout << "Loading shaders... \n";
	//TODO

	std::cout << "Loading meshes... \n";
	for(std::pair<const char*, const char*> p : meshesToLoad) {
		std::cout << "\t" << p.first << " <- " << p.second << "... ";

		if(!load_mesh(p.first, p.second)) {
			std::cout << "fail!\n";
			return false;
		}

		std::cout << "success\n";
	}
	std::cout << std::endl;

	std::cout << "Loading textures... \n";
	for(std::pair<const char*, const char*> p : texturesToLoad) {
		std::cout << "\t" << p.first << " <- " << p.second << "... ";

		if(!load_texture(p.first, p.second)) {
			std::cout << "fail!\n";
			return false;
		}

		std::cout << "success\n";
	}
	std::cout << std::endl;
}

bool app_Scene::init_scene() {

}
