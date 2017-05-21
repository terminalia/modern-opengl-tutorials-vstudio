#include "RenderEngine.h"

RenderEngine::RenderEngine()
{

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

	uiGrid.cleanGLBuffers();
}

void RenderEngine::initGL()
{
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	camera = OrbitCamera(45.0f, 15.0f, 45.0f, 25.0f);
	uiGrid = Grid(20, 1.0f);

	try {
		basicShader = new ShaderProgram("Basic", "../Shaders/Tutorial3/basic.vert", "../Shaders/Tutorial3/basic.frag");
		uiShader = new ShaderProgram("uiShader", "../Shaders/Tutorial3/ui.vert", "../Shaders/Tutorial3/ui.frag");
		objLoader.loadFile("../Assets/pig.obj");
		mesh = new Mesh("PigMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());
		texture =  new Texture("PigTexture", GL_TEXTURE_2D, "../Assets/pig_diffuse_map.png");
	}
	catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}

	modelMatrix = glm::mat4(1.0f);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

void RenderEngine::resize(int width, int height)
{
	glViewport(0, 0, width, height);
	float ratio = (float)width / (float)height;
	camera.updateViewRatio(ratio);	
}

void RenderEngine::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(basicShader->getProgram());
	updateUniformLoc(basicShader);
	uploadUniformsToShader(camera.getProjectionMatrix(), camera.getViewMatrix(), modelMatrix, normalMatrix);

	texture->bind(GL_TEXTURE0);
	mesh->draw();

	glUseProgram(uiShader->getProgram());
	updateUniformLoc(uiShader);
	uploadUniformsToShader(camera.getProjectionMatrix(), camera.getViewMatrix(), modelMatrix, normalMatrix);

	uiGrid.draw();
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