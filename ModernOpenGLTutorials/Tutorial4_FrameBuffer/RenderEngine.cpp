#include "RenderEngine.h"

RenderEngine::RenderEngine(int window_width, int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

RenderEngine::~RenderEngine()
{
	if (mesh) {
		mesh->cleanGLBuffers();
		delete mesh;
	}

	if (texture) {
		texture->cleanGLBuffers();
		delete texture;
	}

	if (basicShader) {
		basicShader->cleanUp();
		delete basicShader;
	}

	if (uiShader) {
		uiShader->cleanUp();
		delete uiShader;
	}

	if (fboShader) {
		fboShader->cleanUp();
		delete fboShader;
	}

	if (fboScreenQuad) {
		fboScreenQuad->cleanGLBuffers();
		delete fboScreenQuad;
	}
	uiGrid.cleanGLBuffers();
	
	glDeleteTextures(1, &colorFBOTextureId);
	glDeleteTextures(1, &depthFBOTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fboId);
}

void RenderEngine::initGL()
{
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	camera = OrbitCamera(45.0f, 15.0f, 45.0f, 25.0f);
	uiGrid = Grid(20, 1.0f);
	fboScreenQuad = new ScreenQuad();

	try {
		basicShader = new ShaderProgram("Basic", "../Shaders/Tutorial4/basic.vert", "../Shaders/Tutorial4/basic.frag");
		uiShader = new ShaderProgram("uiShader", "../Shaders/Tutorial4/ui.vert", "../Shaders/Tutorial4/ui.frag");
		fboShader = new ShaderProgram("fboShader", "../Shaders/Tutorial4/fboscreen.vert", "../Shaders/Tutorial4/fboscreen.frag");

		objLoader.loadFile("../Assets/pig.obj");
		mesh = new Mesh("PigMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());
		texture =  new Texture("PigTexture", GL_TEXTURE_2D, "../Assets/pig_diffuse_map.png");
	}
	catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}

	modelMatrix = glm::mat4(1.0f);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	//FrameBuffer texture size is usally the same size of the screen (or window) but it's not mandatory!
	//In this example we want to use a 1024x1024 texture size
	initFrameBuffer(1024, 1024);
}

void RenderEngine::resize(int width, int height)
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, window_width, window_height);
	float ratio = (float)window_width / (float)window_height;
	camera.updateViewRatio(ratio);	

	initFrameBuffer(1024, 1024);
}

void RenderEngine::render()
{
	//RENDER TO FRAMEBUFFER TEXTURE
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(basicShader->getProgram());
	updateUniformLoc(basicShader);
	uploadUniformsToShader(camera.getProjectionMatrix(), camera.getViewMatrix(), modelMatrix, normalMatrix);

	//Because the texture size is different from the screen size (or window), 
	//Set the viewport accordingly to the texture size
	glViewport(0, 0, 1024, 1024);
	texture->bind(GL_TEXTURE0);
	mesh->draw();

	glUseProgram(uiShader->getProgram());
	updateUniformLoc(uiShader);
	uploadUniformsToShader(camera.getProjectionMatrix(), camera.getViewMatrix(), modelMatrix, normalMatrix);

	uiGrid.draw();

	//BACK TO DEFAULT BUFFER, RENDER TO SCREEN
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(fboShader->getProgram());
	//Before drawing on the quad screen, set the viewport to the screen size (or window)
	glViewport(0, 0, window_width, window_height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorFBOTextureId);
	fboScreenQuad->draw();
}

void RenderEngine::initFrameBuffer(int texture_width, int texture_height)
{
	glDeleteTextures(1, &colorFBOTextureId);
	glDeleteTextures(1, &depthFBOTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fboId);

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	glGenTextures(1, &colorFBOTextureId);
	glBindTexture(GL_TEXTURE_2D, colorFBOTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorFBOTextureId, 0);

	glGenTextures(1, &depthFBOTextureId);
	glBindTexture(GL_TEXTURE_2D, depthFBOTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthFBOTextureId, 0);
																								  
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::updateUniformLoc(ShaderProgram *shader)
{
	projectionUniformLoc = glGetUniformLocation(shader->getProgram(), "uProjectionMat");
	viewUniformLoc = glGetUniformLocation(shader->getProgram(), "uViewMat");
	modelUniformLoc = glGetUniformLocation(shader->getProgram(), "uModelMat");
}

void RenderEngine::uploadUniformsToShader(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &modelMatrix, const glm::mat4 &normalMatrix)
{
	glUniformMatrix4fv(projectionUniformLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

OrbitCamera &RenderEngine::getCamera()
{
	return camera;
}