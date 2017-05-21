#include "RenderEngine.h"

RenderEngine::RenderEngine(int window_width, int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

RenderEngine::~RenderEngine()
{
	for (unsigned int i = 0; i < models.size(); i++)
	{
		delete models[i];
	}

	for (unsigned int i = 0; i < lights.size(); i++)
	{
		delete lights[i];
	}

	if (pigMesh) {
		pigMesh->cleanGLBuffers();
		delete pigMesh;
	}

	if (lightMesh) {
		lightMesh->cleanGLBuffers();
		delete lightMesh;
	}

	if (pigTexture) {
		pigTexture->cleanGLBuffers();
		delete pigTexture;
	}

	if (groundTexture) {
		groundTexture->cleanGLBuffers();
		delete groundTexture;
	}

	if (pointLightShader) {
		pointLightShader->cleanUp();
		delete pointLightShader;
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
		//SHADERS
		pointLightShader = new ShaderProgram("PointLightShader", "../Shaders/Tutorial6/point_light.vert", "../Shaders/Tutorial6/point_light.frag");
		uiShader = new ShaderProgram("uiShader", "../Shaders/Tutorial6/ui.vert", "../Shaders/Tutorial6/ui.frag");
		fboShader = new ShaderProgram("fboShader", "../Shaders/Tutorial6/fboscreen.vert", "../Shaders/Tutorial6/fboscreen.frag");

		//MESHES
		objLoader.loadFile("../Assets/pig.obj");
		pigMesh = new Mesh("PigMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());
		objLoader.loadFile("../Assets/projector.obj");
		lightMesh = new Mesh("LightMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());
		objLoader.loadFile("../Assets/plane.obj");
		groundMesh = new Mesh("GroundMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());

		//TEXTURES
		pigTexture = new Texture("PigTexture", GL_TEXTURE_2D, "../Assets/pig_diffuse_map.png");
		groundTexture = new Texture("GroundTexture", GL_TEXTURE_2D, "../Assets/floor_diff.jpg");

		//MODELS
		models.push_back(new Model("Pig", pigMesh, pigTexture));
		models.push_back(new Model("Ground", groundMesh, groundTexture));

		//LIGHTS
		lights.push_back(new Light("PointLight", point, glm::vec3(0, 5, 0), glm::vec3(0.1, 0.1, 0.1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), lightMesh));
	}
	catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}

	//FrameBuffer texture size is usally the same size of the screen (or window) but it's not mandatory!
	//In this example we want to use a 1024x1024 texture size
	initFrameBuffer(1280, 1280);
}

void RenderEngine::resize(int width, int height)
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, window_width, window_height);
	float ratio = (float)window_width / (float)window_height;
	camera.updateViewRatio(ratio);	

	//initFrameBuffer(1280, 1280);
}

void RenderEngine::render()
{
	//RENDER TO FRAMEBUFFER TEXTURE
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Because the texture size is different from the screen size (or window), 
	//Set the viewport accordingly to the texture size
	glViewport(0, 0, 1280, 1280);
	
	pointLightShader->bind();
	pointLightShader->uploadMat4("uProjectionMat", camera.getProjectionMatrix());
	pointLightShader->uploadMat4("uViewMat", camera.getViewMatrix());
	pointLightShader->uploadVec3("uCameraPosition", camera.getPosition());
	
	//APPLY LIGTHING
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		lights.at(i)->uploadUniformsToShader(pointLightShader);
	}

	//RENDER MODELS
	for (unsigned int i = 0; i < models.size(); i++)
	{
		pointLightShader->uploadMat4("uModelMat", models[i]->getModelMatrix());
		pointLightShader->uploadMat4("uNormalMat", models[i]->getNormalMatrix());
		models.at(i)->draw();
	}

	//RENDER UI ELEMENTS
	uiShader->bind();
	uiShader->uploadMat4("uProjectionMat", camera.getProjectionMatrix());
	uiShader->uploadMat4("uViewMat", camera.getViewMatrix());
	uiShader->uploadMat4("uModelMat", glm::mat4(1.0f));
	uiShader->uploadVec3("uColor", glm::vec3(1, 1, 1));
	uiGrid.draw();

	//RENDER LIGHTS MODELS
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		uiShader->uploadMat4("uModelMat", lights[i]->getModelMatrix());
		uiShader->uploadVec3("uColor", glm::vec3(1, 1, 0));
		lights[i]->draw();

		if (lights.at(i)->isBBVisible())
		{
			uiShader->uploadVec3("uColor", glm::vec3(0, 1, 0));
			lights.at(i)->drawBB();
		}
	}

	//DRAW BOUNDING BOXES
	for (unsigned int i = 0; i < models.size(); i++)
	{
		uiShader->uploadMat4("uModelMat", models.at(i)->getModelMatrix());
		if (models.at(i)->isBBVisible())
		{
			uiShader->uploadVec3("uColor", glm::vec3(0, 1, 0));
			models.at(i)->drawBB();
		}
	}

	//BACK TO DEFAULT BUFFER, RENDER TO SCREEN
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	fboShader->bind();
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

void RenderEngine::castRay(int mouse_x, int mouse_y, int screen_width, int screen_height)
{
	pickedObjectID = "";
	glm::vec3 startRay = camera.getPosition();
	glm::vec3 rayDir = unproject(mouse_x, mouse_y, screen_width, screen_height, camera.getViewMatrix(), camera.getProjectionMatrix());

	float distance = 0.0f;
	float minDistance = 1000000.0f;

	//Check if ray intersects a model
	for (unsigned int i = 0; i < models.size(); i++)
	{
		models.at(i)->showBB(false);
		if (rayInBoundingBox(startRay, rayDir, models.at(i)->getMesh(), models.at(i)->getModelMatrix(), distance))
		{
			minDistance = distance;
			pickedObjectID = models.at(i)->getName();
		}
	}

	//Check if ray intersects a light
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		lights.at(i)->showBB(false);
		if (rayInBoundingBox(startRay, rayDir, lights.at(i)->getMesh(), lights.at(i)->getModelMatrix(), distance))
		{
			minDistance = distance;
			pickedObjectID = lights.at(i)->getName();
		}
	}

	if (!pickedObjectID.empty())
	{
		Model *model = findElement(pickedObjectID, models);
		if (model) {
			model->showBB(true);
		}
		else {
			Light *light = findElement(pickedObjectID, lights);
			light->showBB(true);
		}
	}
}

void RenderEngine::findObjectInScreen(int mouse_x, int mouse_y, int screen_width, int screen_height)
{
	if (!pickedObjectID.empty())
	{
		glm::vec3 objectWorldPos;
		Model *model = findElement(pickedObjectID, models);
		
		//Find the object position in world space
		if (model)
		{
			objectWorldPos = model->getPosition();
		}
		else {
			Light *light = findElement(pickedObjectID, lights);
			objectWorldPos = light->getPosition();
		}

		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = camera.getProjectionMatrix();
		glm::vec4 viewport = glm::vec4(0, 0, screen_width, screen_height);
		glm::mat4 modelMat = glm::mat4(1.0f);

		//Compute object position in screen space
		//We don't use the model matrix of the object because the position of the object is already in world space (multiplied by its model matrix)
		objectScreenPoint = glm::project(objectWorldPos, view * modelMat, proj, viewport);

		//Compute mouse position in world space
		//We use the identity model matrix
		glm::vec3 mouseWorldPos = glm::unProject(glm::vec3(mouse_x, 1 - mouse_y, objectScreenPoint.z), view * glm::mat4(1.0f), proj, viewport);
		//Compute the offset between the object position and where the user has clicked (when collision is detected)
		objectMouseOffset = objectWorldPos - mouseWorldPos;
	}
}

void RenderEngine::moveObject(int mouse_x, int mouse_y, int screen_width, int screen_height)
{
	if (!pickedObjectID.empty())
	{
		glm::mat4 modelMat = glm::mat4(1.0f);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = camera.getProjectionMatrix();
		glm::vec4 viewport = glm::vec4(0, 0, screen_width, screen_height);

		//When dragging the mouse, compute mouse position in world space
		glm::vec3 curMouseWorldPos = glm::unProject(glm::vec3(mouse_x, 1 - mouse_y, objectScreenPoint.z), view * modelMat, proj, viewport);
		//When moving the mouse, the offset between the object position and the mouse position is always the same
		//so we have to take it in account when computing the new position of the object
		glm::vec3 newEntityWorldPos = curMouseWorldPos + objectMouseOffset;

		Model *model = findElement(pickedObjectID, models);
		if (model)
		{
			model->translate(newEntityWorldPos);
		}
		else {
			Light *light = findElement(pickedObjectID, lights);
			light->translate(newEntityWorldPos);
		}
	}
}

OrbitCamera &RenderEngine::getCamera()
{
	return camera;
}

template<typename T>
inline T RenderEngine::findElement(const std::string & name, const std::vector<T>& vec)
{
	for (unsigned int i = 0; i<vec.size(); i++)
	{
		if (vec[i]->getName().compare(name) == 0)
			return vec[i];
	}

	return NULL;
}