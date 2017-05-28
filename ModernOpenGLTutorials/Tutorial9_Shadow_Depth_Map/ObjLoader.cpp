#include "objloader.h"

ObjLoader::ObjLoader()
{

}

ObjLoader::~ObjLoader()
{

}

void ObjLoader::loadFile(const std::string &filename)
{
	reset();

	std::vector<unsigned short> positionIndices;
	std::vector<unsigned short> normalIndices;
	std::vector<unsigned short> texcoordIndices;
	std::vector<glm::vec3> parsed_positions;
	std::vector<glm::vec3> parsed_normals;
	std::vector<glm::vec2> parsed_texcoords;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;

	FILE *file = fopen(filename.c_str(), "r");
	if (file == NULL) {
		throw std::runtime_error("Failed loading OBJ file " + filename + "!");
	}

	while (true)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		//Parse data from .obj file

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 position;
			fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
			parsed_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 texcoord;
			fscanf(file, "%f %f\n", &texcoord.x, &texcoord.y);
			//Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			texcoord.y = -texcoord.y;
			parsed_texcoords.push_back(texcoord);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			parsed_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int positionIndex[3], normalIndex[3], texcoordIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &positionIndex[0], &texcoordIndex[0], &normalIndex[0],
				&positionIndex[1], &texcoordIndex[1], &normalIndex[1],
				&positionIndex[2], &texcoordIndex[2], &normalIndex[2]);

			if (matches != 9) {
				throw std::runtime_error("Failed parsing OBJ file " + filename + "!");
			}

			positionIndices.push_back(positionIndex[0]);
			positionIndices.push_back(positionIndex[1]);
			positionIndices.push_back(positionIndex[2]);
			texcoordIndices.push_back(texcoordIndex[0]);
			texcoordIndices.push_back(texcoordIndex[1]);
			texcoordIndices.push_back(texcoordIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i<positionIndices.size(); i++)
	{
		glm::vec3 position = parsed_positions[positionIndices[i] - 1];
		glm::vec2 texcoord = parsed_texcoords[texcoordIndices[i] - 1];
		glm::vec3 normal = parsed_normals[normalIndices[i] - 1];

		positions.push_back(position);
		texcoords.push_back(texcoord);
		normals.push_back(normal);
	}

	//indexVBOSlow(positions, normals, texcoords);
	indexVBOFast(positions, normals, texcoords);

	/*
	std::cout << "Positions: " << indexed_positions.size() << std::endl;
	std::cout << "Normals: " << indexed_normals.size() << std::endl;
	std::cout << "TexCoords: " << indexed_texcoords.size() << std::endl;
	std::cout << "Indices: " << indices.size() << std::endl;
	*/
}

void ObjLoader::indexVBOFast(const std::vector<glm::vec3> &in_vertices, const std::vector<glm::vec3> &in_normals, const std::vector<glm::vec2> &in_texcoords)
{
	//Here we store vertex struct object we cannot find
	std::map<Vertex, unsigned short> vertexToOutIndex;

	//For each input vertex
	for (unsigned int i = 0; i<in_vertices.size(); i++)
	{
		//Start by creating a vertex struct holding vertices/normals/texcoords
		Vertex vert = { in_vertices[i], in_normals[i], in_texcoords[i] };
		unsigned short index;

		//Try to find a similar vertex in vertexToOutIndex
		bool found = getSimilarVertexFast(vert, vertexToOutIndex, index);

		// A similar vertex is already in vertexToOutIndex, use its index instead!
		if (found)
		{
			indices.push_back(index);
		}
		// If not, it needs to be added in the output data.
		else {
			indexed_positions.push_back(in_vertices[i]);
			indexed_normals.push_back(in_normals[i]);
			indexed_texcoords.push_back(in_texcoords[i]);
			unsigned short newIndex = (unsigned short)indexed_positions.size() - 1;
			indices.push_back(newIndex);
			vertexToOutIndex[vert] = newIndex;
		}
	}
}

bool ObjLoader::getSimilarVertexFast(Vertex &vertex, std::map<Vertex, unsigned short> &vertexToOutIndex, unsigned short &result)
{
	std::map<Vertex, unsigned short>::iterator it = vertexToOutIndex.find(vertex);
	if (it == vertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}

void ObjLoader::indexVBOSlow(const std::vector<glm::vec3> &in_positions, const std::vector<glm::vec3> &in_normals, const std::vector<glm::vec2> &in_texcoords)
{
	for (unsigned int i = 0; i<in_positions.size(); i++)
	{
		unsigned short index;
		bool found = getSimilarVertexSlow(in_positions[i], in_normals[i], in_texcoords[i], index);

		if (found)
		{
			indices.push_back(index);
		}
		else
		{
			indexed_positions.push_back(in_positions[i]);
			indexed_normals.push_back(in_normals[i]);
			indexed_texcoords.push_back(in_texcoords[i]);
			indices.push_back(indexed_positions.size() - 1);
		}
	}
}

bool ObjLoader::getSimilarVertexSlow(const glm::vec3 &in_position, const glm::vec3 &in_normal, const glm::vec2 &in_texcoord, unsigned short &result)
{
	for (unsigned int i = 0; i<indexed_positions.size(); i++)
	{
		if (isEqual(in_position.x, indexed_positions[i].x) &&
			isEqual(in_position.y, indexed_positions[i].y) &&
			isEqual(in_position.z, indexed_positions[i].z) &&

			isEqual(in_normal.x, indexed_normals[i].x) &&
			isEqual(in_normal.y, indexed_normals[i].y) &&
			isEqual(in_normal.z, indexed_normals[i].z) &&

			isEqual(in_texcoord.x, indexed_texcoords[i].x) &&
			isEqual(in_texcoord.y, indexed_texcoords[i].y))
		{
			result = i;
			return true;
		}
	}

	return false;
}

bool ObjLoader::isEqual(float v1, float v2)
{
	return fabs(v1 - v2) < 0.01f;
}

std::vector<glm::vec3> ObjLoader::getPositions()
{
	return indexed_positions;
}

std::vector<glm::vec3> ObjLoader::getNormals()
{
	return indexed_normals;
}

std::vector<glm::vec2> ObjLoader::getTexCoords()
{
	return indexed_texcoords;
}

std::vector<unsigned short> ObjLoader::getIndices()
{
	return indices;
}

void ObjLoader::reset()
{
	std::vector<glm::vec3>().swap(indexed_positions);
	std::vector<glm::vec3>().swap(indexed_normals);
	std::vector<glm::vec2>().swap(indexed_texcoords);
	std::vector<unsigned short>().swap(indices);
}
