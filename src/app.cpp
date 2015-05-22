#include "app.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "glwrap/util.h"
#include "glwrap/meshutil.h"
#include "glwrap/texture_util.h"

#include <iostream>
#include <fstream>

#include <utility>
#include <tuple>
#include <vector>

#include <cstdlib> //std::exit

#define dieret(msg, val) {std::cerr << msg << std::endl; return val;}
#define die(msg) {std::cerr << msg << std::endl; std::exit(1);}

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

	separated_mesh* resultMesh = new separated_mesh;
	meshutil::load_obj(file, *resultMesh);
	m_Meshes.insert({name, resultMesh});

	return true;
}

bool app_Scene::load_texture(const char* name, const char* file) {
	if(!file_exists(file))
		return false;

	texture2d* resultTexture = new texture2d;
	resultTexture->upload(texutil::load_png(file), GL_RGB);
	resultTexture->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	resultTexture->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	resultTexture->generate_mipmaps();
	m_Textures.insert({name, resultTexture});

	return true;
}

bool app_Scene::load_shader(const char* name, const char* vertFile, const char* fragFile) {
	if(!file_exists(vertFile) || !file_exists(fragFile))
		return false;

	shader_program* resultShader = new shader_program;
	resultShader->create();

	if(!resultShader->attach(read_file(vertFile).c_str(), shader_program::shader_type::vertex)) {
		delete resultShader;
		return false;
	}

	if(!resultShader->attach(read_file(fragFile).c_str(), shader_program::shader_type::fragment)) {
		delete resultShader;
		return false;
	}

	glBindFragDataLocation(resultShader->handle(), 0, "outColor");
	resultShader->link();

	m_Shaders.insert({name, resultShader});
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

	if(!GLEW_VERSION_3_3)
	{
		std::cerr << "OpenGL 3.3 not supported" << std::endl;
		return 0; 
	}
	
	return 1;
}

bool app_Scene::gl_init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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
		std::make_tuple("textured", "data/shaders/textured.vs", "data/shaders/textured.fs"),
		std::make_tuple("opaque",	"data/shaders/opaque.vs", 	"data/shaders/opaque.fs")
	};

	std::cout << "Loading shaders... \n";
	for(std::tuple<const char*, const char*, const char*> t : shadersToLoad) {
		std::cout << "\t" << std::get<0>(t) << " <- " << std::get<1>(t) << ", " << std::get<2>(t) << "... ";

		if(!load_shader(std::get<0>(t),std::get<1>(t),std::get<2>(t))) {
			std::cout << "fail!\n";
			return false;
		}

		std::cout << "success\n";
	}
	std::cout << std::endl;

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

	return true;
}

bool app_Scene::init_scene() {
	std::cout << "Initializing scene... ";

	//Room
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 180.0f));
			item.transform.scale = glm::vec3(1.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["room-diffuse"];
			item.material.specularTexture = m_Textures["room-specular"];
			item.material.aoTexture = m_Textures["room-ao"];

			item.mesh = m_Meshes["room"];

		m_Renderables.insert({renderPhase_Opaque, item});
	}

	//Armchair
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f, 0.0f, 1.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 180.0f));
			item.transform.scale = glm::vec3(1.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["armchair-diffuse"];
			item.material.specularTexture = m_Textures["armchair-specular"];
			item.material.aoTexture = m_Textures["armchair-ao"];

			item.mesh = m_Meshes["armchair"];

		m_Renderables.insert({renderPhase_Opaque, item});
	}

	//Table
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(2.88252f, 0.28706f, 2.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 196.513f));
			item.transform.scale = glm::vec3(1.0f / 3.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["table-diffuse"];
			item.material.specularTexture = m_Textures["table-specular"];
			item.material.aoTexture = m_Textures["table-ao"];

			item.mesh = m_Meshes["table"];

		m_Renderables.insert({renderPhase_Opaque, item});
	}

	//Projector
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f, -3.79882f, 5.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 180.0f));
			item.transform.scale = glm::vec3(1.0f / 2.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["projector-diffuse"];
			item.material.specularTexture = m_Textures["projector-specular"];
			item.material.aoTexture = m_Textures["projector-ao"];

			item.mesh = m_Meshes["projector"];

		m_Renderables.insert({renderPhase_Opaque, item});
	}

	//Light
	{
		m_LightPos = glm::vec3(0.0f, 6.0f, 5.0f);
		m_LightColor = glm::vec3(1.0f);
	}

	//Camera
	{
		m_Camera.pos = glm::vec3(-2.12785f, 9.71749f, 11.49396f);
		m_Camera.rot = glm::radians(glm::vec3(50.134f, 0.0f, 193.717f));
		m_Camera.scale = glm::vec3(1.0f);
	}
	std::cout << "done\n";
	return true;
}

//===========================================================================================//
//Shutdown funcs

void app_Scene::free_resources() {
	std::cout << "Freeing shaders... \n";
	for(auto& p : m_Shaders) {
		std::cout << "\t" << p.first << "\n";
		delete p.second;
	}
	std::cout << std::endl;

	std::cout << "Freeing meshes... \n";
	for(auto& p : m_Meshes) {
		std::cout << "\t" << p.first << "\n";
		delete p.second;
	}
	std::cout << std::endl;

	std::cout << "Freeing textures... \n";
	for(auto& p : m_Textures) {
		std::cout << "\t" << p.first << "\n";
		delete p.second;
	}
	std::cout << std::endl;
}

//===========================================================================================//
//App events

void app_Scene::on_open()
{
	if(!glew_init())
		die("GLEW init fail");
	
	if(!gl_init())
		die("GL init fail");
	
	if(!load_resources())
		die("Couldn't load resources");

	if(!init_scene())
		die("Couldn't init scene");
	
	//Initial resize ( window init kinda )
	{
		int w, h;
		glfwGetWindowSize(this->handle(), &w, &h);
		this->on_resize(w,h);
	}
}

void app_Scene::on_resize(int w, int h)
{
	resizable_window::on_resize(w,h);
	m_CameraProjection = glm::perspective(m_CameraFOV, (float)m_WindowAspect, 0.0625f, 8192.0f);
}


void app_Scene::on_key(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(this->handle(), 1);
	}
}

void app_Scene::on_refresh()
{
	//update();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_program* currentShader = m_Shaders["opaque"];
	currentShader->use();

	glm::mat4 matView = m_Camera.calculateView();
	glm::mat4 matWorld;
	glm::mat4 matMatrix;

	for(const auto& p : m_Renderables) {
		const renderable_t& r = p.second;
		matWorld = r.transform.calculateWorld();

		currentShader->set_uniform("uWorldMatrix", matWorld);
		currentShader->set_uniform("uMVP", m_CameraProjection * matView * matWorld);
		currentShader->set_uniform("uNormalMatrix", glm::transpose(glm::inverse(matWorld)));

		currentShader->set_uniform("uLightPos", m_LightPos);
		currentShader->set_uniform("uLightColor", m_LightColor);

		r.material.diffuseTexture->use();
		r.mesh->bind();
		r.mesh->draw();
	}

	glfwSwapBuffers(this->handle());
}

void app_Scene::on_close() {
	free_resources();
}

//===========================================================================================//
//Runner code

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

int main() 
{
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
		return 1;
		
	app_Scene wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	wnd.open(640,360, "scene");
	if(!wnd)
		return 3;
	
	wnd.make_current();
	
	std::cout << "Init done, starting loop" << std::endl;
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}