#include "app.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/io.hpp>

#include "glwrap/util.h"
#include "glwrap/meshutil.h"
#include "glwrap/texture_util.h"

#include <iostream>
#include <fstream>

#include <utility>
#include <tuple>
#include <vector>

#include <cstdlib> //std::exit
#include <cmath>

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
	glEnable(GL_BLEND);

	return 1;
}

bool app_Scene::load_resources() {
	std::vector<std::pair<const char*, const char*>> meshesToLoad = {
		{"room",			"data/meshes/room.obj"},
		{"table", 			"data/meshes/table.obj"},

		{"whisky", 			"data/meshes/whisky.obj"},
		{"whisky-glass",	"data/meshes/whisky-glass.obj"},

		{"ashtray", 		"data/meshes/ashtray.obj"},
		//{"armchair", 		"data/meshes/armchair.obj"},
		{"armchair", 		"data/meshes/whisky-glass.obj"},
		{"projector", 		"data/meshes/projector.obj"},

		{"buddha",			"data/meshes/buddha.obj"}
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
		{"projector-specular", 	"data/textures/projector-specular.png"},

		{"strawberry",			"data/textures/strawberry.png"}
	};

	std::vector<std::tuple<const char*, const char*, const char*>> shadersToLoad = {
		std::make_tuple("textured", "data/shaders/textured.vs", "data/shaders/textured.fs"), 
		std::make_tuple("opaque",	"data/shaders/opaque.vs", 	"data/shaders/opaque.fs"), 
		std::make_tuple("project", "data/shaders/projtex.vs", "data/shaders/projtex.fs"),  
		std::make_tuple("depth", "data/shaders/depth.vs", "data/shaders/depth.fs")
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
	m_Textures["strawberry"]->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	m_Textures["strawberry"]->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	std::cout << std::endl;

	std::cout << "Creating FBO... ";
	{
		std::cout << "\tCreating texture... ";
			GLuint textureHandle;
			glGenTextures(1, &textureHandle);
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

			//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_DepthMapSize.x, m_DepthMapSize.y, 0, GL_RED, GL_FLOAT, NULL);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, m_DepthMapSize.x, m_DepthMapSize.y);
		std::cout << "done\n";

		/*std::cout << "\tCreating RBO... ";
			GLuint rboHandle;
			glGenRenderbuffers(1, &rboHandle);
			glBindRenderbuffer(GL_RENDERBUFFER, rboHandle);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_DepthMapSize.x, m_DepthMapSize.y);
		std::cout << "done\n";*/
		
		m_ProjectorDepthMap.attach_texture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureHandle, 0, true);
		//m_ProjectorDepthMap.attach_rbo(GL_DEPTH_ATTACHMENT, rboHandle, true);

		m_ProjectorDepthMap.unbind();
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);

		m_DepthTexture = textureHandle;
	}
	std::cout << "Done";

	return true;
}

bool app_Scene::init_scene() {
	std::cout << "Initializing scene... ";

	//Room
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f));
			item.transform.scale = glm::vec3(1.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["room-diffuse"];
			item.material.specularTexture = m_Textures["room-specular"];
			item.material.aoTexture = m_Textures["room-ao"];

			item.mesh = m_Meshes["room"];

		m_Renderables.insert({renderPhase_Opaque, new renderable_t(item)});
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

		m_Renderables.insert({renderPhase_Opaque, new renderable_t(item)});
	}

	//Table
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f));
			item.transform.scale = glm::vec3(1.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["table-diffuse"];
			item.material.specularTexture = m_Textures["table-specular"];
			item.material.aoTexture = m_Textures["table-ao"];

			item.mesh = m_Meshes["table"];

		m_Renderables.insert({renderPhase_Opaque, new renderable_t(item)});
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

		m_Projector = new renderable_t(item);
		m_Renderables.insert({renderPhase_Opaque, m_Projector});
	}

	//Buddha
	{
		renderable_t item;
			item.transform.pos   = glm::vec3(0.0f);
			item.transform.rot   = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f));
			item.transform.scale = glm::vec3(1.0f);

			item.material.attributes["matDiffuse"] = glm::vec4(1.0f);
			item.material.attributes["matSpecular"] = glm::vec4(1.0f, 1.0f, 1.0f, 50.0f);

			item.material.diffuseTexture = m_Textures["room-ao"];
			item.material.specularTexture = m_Textures["room-specular"];
			item.material.aoTexture = m_Textures["room-ao"];

			item.mesh = m_Meshes["buddha"];

		m_Renderables.insert({renderPhase_Opaque, new renderable_t(item)});
	}

	//Light
	{
		m_LightPos = glm::vec3(0.0f, 6.0f, 5.0f);
		m_LightColor = glm::vec3(1.0f);
	}

	//Camera
	{
		m_Camera.pos = glm::vec3(-2.12785f, 9.71749f, 11.49396f);
		m_Camera.rot = glm::radians(glm::vec3(-50.134f, 0.0f, 90.0f));
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

	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) 
	{
		m_CameraControl = !m_CameraControl;

		if(m_CameraControl) {
			glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_CameraGrabAt = m_Mouse;
		} 
		else {
			glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void app_Scene::on_mousepos(double x, double y) {
	m_Mouse = glm::vec2(x,y);

	if(m_CameraControl)
	{
		glm::vec2 delta = m_CameraGrabAt - m_Mouse;
		m_Camera.rot.x = glm::clamp(m_Camera.rot.x + delta.y / 64.0f, -glm::radians(89.0f), glm::radians(89.0f));
		m_Camera.rot.z = std::fmod(m_Camera.rot.z - delta.x / 64.0f, glm::two_pi<float>());
		
		m_CameraGrabAt = m_Mouse;
	}
}

void app_Scene::update() {
	static double lastFrameCountStart = glfwGetTime();
	static unsigned frameCount = 0;
	static double lastCheck = glfwGetTime();

	double frameTime = glfwGetTime() - lastCheck;
	lastCheck += frameTime;

	m_ProjectTransform.pos = m_Projector->transform.pos + m_Projector->transform.right()*0.5f;
	m_ProjectTransform = m_Projector->transform;
	m_ProjectTransform.rot.z += glm::radians(90.0f);
	m_ProjectTransform.pos += m_ProjectTransform.forward() * 2.0f;

	//m_ProjectTransform.rot.z = glm::radians(360.0f * (float)fmod(glfwGetTime(), 8.0f) / 8.0f);

	if(m_CameraControl) {
		float speed = frameTime * m_CameraSpeed * (glfwGetKey(this->handle(), GLFW_KEY_LEFT_SHIFT) ? 2.0f : 1.0f);

		if(glfwGetKey(this->handle(), GLFW_KEY_W))
			m_Camera.pos += speed * m_Camera.forward();

		if(glfwGetKey(this->handle(), GLFW_KEY_S))
			m_Camera.pos -= speed * m_Camera.forward();

		if(glfwGetKey(this->handle(), GLFW_KEY_D))
			m_Camera.pos += speed * m_Camera.right();

		if(glfwGetKey(this->handle(), GLFW_KEY_A))
			m_Camera.pos -= speed * m_Camera.right();
	}
	
	frameCount++;
	if(glfwGetTime() - lastFrameCountStart >= 1.0) {
		std::cout << "FPS: " << frameCount << '\r';
		frameCount = 0;
		lastFrameCountStart = glfwGetTime();
	}
}

void app_Scene::on_refresh()
{
	update();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	shader_program* currentShader;
	glm::mat4 matView;
	glm::mat4 matWorld;

	//Render depth map
	m_ProjectorDepthMap.bind();
	glViewport(0, 0, m_DepthMapSize.x, m_DepthMapSize.y);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

	glm::mat4 matImageView =  m_ProjectTransform.calculateView();
		//matImageView = glm::rotate(matImageView, glm::radians(360.0f * (float)fmod(glfwGetTime()/8.0f, 8.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
		//matImageView = glm::rotate(matImageView, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matImageProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.2f, 1000.0f);
    glm::mat4 matImageST = glm::translate(glm::mat4(), glm::vec3(0.5f)) * glm::scale(glm::mat4(), glm::vec3(0.5f));
	glm::mat4 matImage = matImageST * matImageProj * matImageView;

	currentShader = m_Shaders["depth"];
	currentShader->use();

	currentShader->set_uniform("depthBias", +1.0f/8.0f);

	for(const auto& p : m_Renderables) {
		const renderable_t& r = *p.second;
		matWorld = r.transform.calculateWorld();

		currentShader->set_uniform("uMV", matImageView * matWorld);
		currentShader->set_uniform("uProjection", matImageProj);
		
		r.mesh->bind();
		r.mesh->draw();
	}
	m_ProjectorDepthMap.unbind();

	//Render opaque
	glViewport(0,0, m_WindowWidth, m_WindowHeight); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
	currentShader = m_Shaders["opaque"];
	currentShader->use();

	matView = m_Camera.calculateView();

	currentShader->set_uniform("uLightPos", m_LightPos);
	currentShader->set_uniform("uLightColor", m_LightColor);
	currentShader->set_uniform("uLightRange", 30.0f);

	currentShader->set_uniform("uDiffuseTexture", 0);
	currentShader->set_uniform("uSpecularTexture", 1);
	currentShader->set_uniform("uAOTexture", 2);

	currentShader->set_uniform("uViewPos", m_Camera.pos);

	for(const auto& p : m_Renderables) {
		if(p.first != renderPhase_Opaque)
			continue;

		const renderable_t& r = *p.second;
		matWorld = r.transform.calculateWorld();

		currentShader->set_uniform("uWorldMatrix", matWorld);
		currentShader->set_uniform("uMVP", m_CameraProjection * matView * matWorld);
		currentShader->set_uniform("uNormalMatrix", glm::transpose(glm::inverse(matWorld)));

		glActiveTexture(GL_TEXTURE0);
		r.material.diffuseTexture->use();

		glActiveTexture(GL_TEXTURE1);
		r.material.specularTexture->use();

		glActiveTexture(GL_TEXTURE2);
		r.material.aoTexture->use();

		r.mesh->bind();
		r.mesh->draw();
	}
	glActiveTexture(GL_TEXTURE0);

	//Render projected
	glBlendFunc(GL_ONE, GL_ONE); //Add colors
	glDepthFunc(GL_EQUAL);

	currentShader = m_Shaders["project"];
	currentShader->use();

	glActiveTexture(GL_TEXTURE0);
	m_Textures["strawberry"]->use();
	//glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	currentShader->set_uniform("ProjectorTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	currentShader->set_uniform("ShadowMap", 1);

	for(const auto& p : m_Renderables) {
		const renderable_t& r = *p.second;
		matWorld = r.transform.calculateWorld();

		currentShader->set_uniform("ModelMatrix", matWorld);
		currentShader->set_uniform("MVP", m_CameraProjection * matView * matWorld);
		currentShader->set_uniform("ProjectorMatrix", matImage);

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