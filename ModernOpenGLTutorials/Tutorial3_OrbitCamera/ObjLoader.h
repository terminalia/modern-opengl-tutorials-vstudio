#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include <map>
#include <vector>
#include <exception>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	bool operator< (const Vertex other) const {
		return memcmp((void*)this, (void*)&other, sizeof(Vertex)) > 0;
	}
};

class ObjLoader
{
private:
	std::vector<glm::vec3> indexed_positions;
	std::vector<glm::vec3> indexed_normals;
	std::vector<glm::vec2> indexed_texcoords;
	std::vector<unsigned short> indices;

	bool isEqual(float v1, float v2);
	void indexVBOSlow(const std::vector<glm::vec3> &in_vertices, const std::vector<glm::vec3> &in_normals, const std::vector<glm::vec2> &in_texcoords);
	void indexVBOFast(const std::vector<glm::vec3> &in_vertices, const std::vector<glm::vec3> &in_normals, const std::vector<glm::vec2> &in_texcoords);
	bool getSimilarVertexSlow(const glm::vec3 &in_vertex, const glm::vec3 &in_normal, const glm::vec2 &in_texcoord, unsigned short &result);
	bool getSimilarVertexFast(Vertex &vertex, std::map<Vertex, unsigned short> &vertexToOutIndex, unsigned short &result);

public:
	ObjLoader();
	~ObjLoader();

	void loadFile(const std::string &filename);
	std::vector<glm::vec3> getPositions();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec2> getTexCoords();
	std::vector<unsigned short> getIndices();
	void reset();
};

#endif // OBJLOADER_H
