#include "RenderEngine.h"

RenderEngine::RenderEngine(int window_width, int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
	isShowingDepthMap = false;
	isDirLight = true;
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

	if (colorMapShader) {
		colorMapShader->cleanUp();
		delete colorMapShader;
	}

	if (depthMapShader) {
		depthMapShader->cleanUp();
		delete depthMapShader;
	}

	if (uiShader) {
		uiShader->cleanUp();
		delete uiShader;
	}

	if (screenColorShader) {
		screenColorShader->cleanUp();
		delete screenColorShader;
	}

	if (fboScreenQuad) {
		fboScreenQuad->cleanGLBuffers();
		delete fboScreenQuad;
	}
	uiGrid.cleanGLBuffers();
	
	glDeleteTextures(1, &colorFBOColorMap);
	glDeleteTextures(1, &colorFBODepthMap);
	glDeleteTextures(1, &depthFBODepthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &colorFBO);
	glDeleteFramebuffers(1, &depthFBO);
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
		colorMapShader = new ShaderProgram("ColorShader", "../Shaders/Tutorial9/colormap_dir.vert", "../Shaders/Tutorial9/colormap_dir.frag");
		depthMapShader = new ShaderProgram("DepthShader", "../Shaders/Tutorial9/depthmap.vert", "../Shaders/Tutorial9/depthmap.frag");
		uiShader = new ShaderProgram("uiShader", "../Shaders/Tutorial9/ui.vert", "../Shaders/Tutorial9/ui.frag");
		screenColorShader = new ShaderProgram("fboShader", "../Shaders/Tutorial9/fboscreen_color.vert", "../Shaders/Tutorial9/fboscreen_color.frag");
		screenDepthShader = new ShaderProgram("fboShader", "../Shaders/Tutorial9/fboscreen_depth.vert", "../Shaders/Tutorial9/fboscreen_depth.frag");

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
		lights.push_back(new Light("SpotLight", spot, glm::vec3(5, 20, 20), glm::vec3(0.0f, -1.0f, -1.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), 1.0f, 0.022f, 0.0019f, 62.5f, 65.5f, lightMesh));
		lights.push_back(new Light("DirLight", directional, glm::vec3(-1, -5, 0), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), lightMesh));
	}
	catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}

	//FrameBuffer texture size is usally the same size of the screen (or window) but it's not mandatory!
	//In this example we want to use a 1024x1024 texture size
	initFrameBuffer(1280, 1280);
	initDepthFrameBuffer(1280, 1280);
}

void RenderEngine::resize(int width, int height)
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, window_width, window_height);
	float ratio = (float)window_width / (float)window_height;
	camera.updateViewRatio(ratio);	
}

void RenderEngine::render()
{
	if (isShowingDepthMap)
	{
		renderDepthMap(depthMapShader, 1280, 1280);
	}
	else
	{
		renderColorMap(colorMapShader, 1280, 1280);
	}
}

void RenderEngine::renderColorMap(ShaderProgram * shader, int texture_width, int texture_height)
{
	//RENDER TO FRAMEBUFFER COLOR TEXTURE
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Because the texture size is different from the screen size (or window), 
	//Set the viewport accordingly to the texture size
	glViewport(0, 0, texture_width, texture_height);

	shader->bind();
	shader->uploadMat4("uProjectionMat", camera.getProjectionMatrix());
	shader->uploadMat4("uViewMat", camera.getViewMatrix());
	shader->uploadVec3("uCameraPosition", camera.getPosition());

	//APPLY LIGTHING
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		lights.at(i)->uploadUniformsToShader(shader);
	}

	//RENDER MODELS
	for (unsigned int i = 0; i < models.size(); i++)
	{
		shader->uploadMat4("uModelMat", models[i]->getModelMatrix());
		shader->uploadMat4("uNormalMat", models[i]->getNormalMatrix());
		models.at(i)->draw(GL_TEXTURE0);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//Before drawing on the quad screen, set the viewport to the screen size (or window)
	glViewport(0, 0, window_width, window_height);

	//Bind the screenColorShader to render color map onto screen quad
	screenColorShader->bind();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorFBOColorMap);
	fboScreenQuad->draw();
}

void RenderEngine::renderDepthMap(ShaderProgram * shader, int texture_width, int texture_height)
{
	//RENDER TO FRAMEBUFFER DEPTH TEXTURE
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Because the texture size is different from the screen size (or window), 
	//Set the viewport accordingly to the texture size
	glViewport(0, 0, texture_width, texture_height);

	shader->bind();

	//RENDER THE SCENE FROM LIGHT POINT OF VIEW (compute lightSpaceMatrix)
	float near = 0.0f;
	float far = 0.0f;
	glm::mat4 lightSpaceMatrix;

	//Setup the matrix to render the scene from the point of view of the light (in otho projection)
	if (isDirLight)
	{
		near = -10.0f;
		far = 20.0f;
		Light *light = findElement("DirLight", lights);
		lightSpaceMatrix = computeOrthoLightMatrix(light->getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), -10, 10, -10, 10, near, far);
	}
	//Setup the matrix to render the scene from the point of view of the light (in perspective projection)
	else
	{
		float ratio = (float)texture_width / (float)texture_height;
		near = 0.1f;
		far = 100.0f;
		Light *light = findElement("SpotLight", lights);
		lightSpaceMatrix = computePerspectiveLightMatrix(light->getPosition(), light->getDirection(), 45.0f, ratio, near, far);
	}

	shader->uploadMat4("uLightSpaceMat", lightSpaceMatrix);

	//RENDER JUST THE MODELS IN THE SCENE FROM THE LIGHT POINT OF VIEW
	for (unsigned int i = 0; i < models.size(); i++)
	{
		shader->uploadMat4("uModelMat", models[i]->getModelMatrix());
		models[i]->draw(GL_TEXTURE0);
	}

	//BACK TO DEFAULT BUFFER, RENDER TO SCREEN
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Before drawing on the quad screen, set the viewport to the screen size (or window)
	glViewport(0, 0, window_width, window_height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthFBODepthMap);

	//Bind the screenDepthShader to render depth map onto screen quad
	screenDepthShader->bind();
	screenDepthShader->uploadInt("depthMap", 0);
	screenDepthShader->uploadFloat("near_plane", near);
	screenDepthShader->uploadFloat("far_plane", far);
	screenDepthShader->uploadBool("uIsDirLight", isDirLight);
	fboScreenQuad->draw();
}

void RenderEngine::initFrameBuffer(int texture_width, int texture_height)
{
	glDeleteTextures(1, &colorFBOColorMap);
	glDeleteTextures(1, &colorFBODepthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &colorFBO);

	glGenFramebuffers(1, &colorFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);

	glGenTextures(1, &colorFBOColorMap);
	glBindTexture(GL_TEXTURE_2D, colorFBOColorMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorFBOColorMap, 0);

	glGenTextures(1, &colorFBODepthMap);
	glBindTexture(GL_TEXTURE_2D, colorFBODepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, colorFBODepthMap, 0);
																								  
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::initDepthFrameBuffer(int texture_width, int texture_height)
{
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	glGenTextures(1, &depthFBODepthMap);
	glBindTexture(GL_TEXTURE_2D, depthFBODepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthFBODepthMap, 0);

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
			if (distance < minDistance)
			{
				minDistance = distance;
				pickedObjectID = models.at(i)->getName();
			}
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

void RenderEngine::showDepthMap()
{
	isShowingDepthMap = !isShowingDepthMap;
}

void RenderEngine::switchLight()
{
	isDirLight = !isDirLight;
}

glm::mat4 RenderEngine::computeOrthoLightMatrix(const glm::vec3 & lightPos, const glm::vec3 & lightDir, float left, float right, float bottom, float top, float near, float far)
{
	glm::mat4 depthProjectionMatrix = glm::ortho(left, right, bottom, top, near, far);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightDir, glm::vec3(0, 1, 0));
	
	return depthProjectionMatrix * depthViewMatrix;
}

glm::mat4 RenderEngine::computePerspectiveLightMatrix(glm::vec3 & lightPos, const glm::vec3 & lightDir, float fov, float ratio, float near, float far)
{
	//If we use perspective projection we have to invert the lightDir otherwise the depthmap is all white
	glm::vec3 invLightDir = -lightDir;
	glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(fov), ratio, near, far);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos - invLightDir, glm::vec3(0, 1, 0));

	return depthProjectionMatrix * depthViewMatrix;
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