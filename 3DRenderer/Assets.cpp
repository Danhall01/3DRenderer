#include "Assets.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//Help on maps & switch strings by article https://www.codeguru.com/cplusplus/switch-on-strings-in-c/
//By CodeGuru Staff

#define DEFAULT_MAP_KA "DEFAULTCLR"
#define DEFAULT_MAP_LIGHT "LIGHT"

//Bind strings to enum to allow for lookup tables
enum CMDVal
{
	cmd_notDefined,
	cmd_comment,
	cmd_v,
	cmd_vt,
	cmd_vn,
	cmd_o,
	cmd_s,
	cmd_g,
	cmd_f,
	cmd_mtllib,
	cmd_usemtl,

	cmd_newmtl,
	cmd_ns,
	cmd_ka,
	cmd_kd,
	cmd_ks,
	cmd_ke,
	cmd_mapKd,
	cmd_mapKs,
	cmd_mapKa,
	cmd_ni,
	cmd_d,
	cmd_illum,
	cmd_end
};
static const std::map<std::string, CMDVal> s_CMD = {
	//Obj file
	{"#", cmd_comment},
	{"v", cmd_v},
	{"vt", cmd_vt},
	{"vn", cmd_vn},
	{"o", cmd_o},
	{"s", cmd_s},
	{"g", cmd_g},
	{"f", cmd_f},
	{"mtllib", cmd_mtllib},
	{"usemtl", cmd_usemtl},
	//Texture file
	{"newmtl", cmd_newmtl},
	{"Ns", cmd_ns},
	{"Ka", cmd_ka},
	{"Kd", cmd_kd},
	{"Ks", cmd_ks},
	{"Ke", cmd_ke},
	{"map_Kd", cmd_mapKd},
	{"map_Ks", cmd_mapKs},
	{"map_Ka", cmd_mapKa},
	{"Ni", cmd_ni},
	{"d", cmd_d},
	{"illum", cmd_illum}
};

Assets::Assets() 
{
	Image defaultClrImg = {};
	Image lightImg = {};
	//       width * height * channels
	int area = 1024 * 1024 * 4;
	defaultClrImg.img = new unsigned char[area];
	defaultClrImg.channels = 4;
	defaultClrImg.height = 1024;
	defaultClrImg.width = 1024;
	std::fill(defaultClrImg.img, defaultClrImg.img + area, static_cast<unsigned char>(187));


	lightImg.img = new unsigned char[area];
	lightImg.channels = 4;
	lightImg.height = 1024;
	lightImg.width = 1024;
	std::fill_n(lightImg.img, area, static_cast<unsigned char>(253));

	m_images.try_emplace(DEFAULT_MAP_KA, defaultClrImg);
	m_images.try_emplace(DEFAULT_MAP_LIGHT, lightImg);
}
Assets::~Assets() 
{
	for (auto& images : m_images)
	{
		if (images.second.img != nullptr)
		{
			stbi_image_free(images.second.img);
		}
	}
}

// Data getters
bool Assets::GetMesh(std::string meshId, Mesh& mesh) const
{
	if (m_meshMap.count(meshId) > 0)
	{
		mesh = m_meshMap.at(meshId);
		return true;
	}
	return false;
}
const std::unordered_map<std::string, Mesh>& Assets::GetMeshMap() const
{
	return m_meshMap;
}

bool Assets::GetTextureData(const std::string& texId, TextureData& texture) const
{
	if (m_textureMap.count(texId) > 0)
	{
		texture = m_textureMap.at(texId).Data();
		return true;
	}
	return false;
}
bool Assets::GetTexture(const std::string& texId, Texture& texture) const
{
	if (m_textureMap.count(texId) > 0)
	{
		texture = m_textureMap.at(texId);
		return true;
	}
	return false;
}
const std::unordered_map<std::string, Texture>& Assets::GetTextureMap() const
{
	return m_textureMap;
}

bool Assets::GetImage(std::string imgId, Image& img) const
{
	if (m_images.count(imgId) > 0)
	{
		img = m_images.at(imgId);
		return true;
	}
	return false;
}
const std::unordered_map<std::string, Image>& Assets::GetImageMap() const
{
	return m_images;
}

void Assets::Clear()
{
	m_images.clear();
}


//Parser helper functions
std::array<float, 3> Assets::MakeFXYZ(std::istringstream& pstream)
{
	std::array<float, 3> f_xyz = {};
	for (int i = 0; i < 3; i++)
	{
		pstream >> f_xyz[i];
	}
	return f_xyz;
}
std::array<float, 2> Assets::MakeFXY(std::istringstream& pstream)
{
	std::array<float, 2> f_xy = {};
	for (int i = 0; i < 2; i++)
	{
		pstream >> f_xy[i];
	}
	return f_xy;
}
void Assets::AddMesh(const std::string& meshID, Mesh& mesh)
{
	m_meshMap.try_emplace(meshID, mesh);
}
inline void Assets::ParseMesh(Mesh& mesh,
	const std::vector<std::string>& indiceStrVec,
	const std::unordered_map<std::string, int>& indexCountMap,
	const std::vector<std::array<float, 3>>& posList,
	const std::vector<std::array<float, 3>>& normalList,
	const std::vector<std::array<float, 2>>& uvList)
{
	int ixyz[3] = {};
	// run through list with map, converting to ints for index buffer
	for (int i = 0; i < indiceStrVec.size(); i++)
	{
		mesh.AddIndice(indexCountMap.at(indiceStrVec[i]));
	}

	//Removes duplicates from indiceStrVec to ensure no duplicates are added as vertices
	std::vector<std::string> newVec = {};
	std::unordered_set<std::string> set = {};
	std::string element = {};
	for (int i = 0; i < indiceStrVec.size(); ++i)
	{
		element = indiceStrVec[i];
		if (set.count(element) == 0)
		{
			newVec.push_back(element);
			set.insert(element);
		}
	}

	// Generates the vertices
	for (auto& indice : newVec)
	{
		sscanf_s(indice.c_str(), "%d/%d/%d",
			&ixyz[0], &ixyz[1], &ixyz[2]); // Reads: Pos/UV/Normal

		// x/y/z is numbered from 1 and up
		mesh.AddVertice({ posList[(ixyz[0] - 1)], normalList[(ixyz[2] - 1)], uvList[(ixyz[1] - 1)] });
	}

}

inline void Assets::LoadToImage(const std::string& path, const std::string& filename)
{
	if (m_images.count(filename) > 0)
		return;
	Image image = {};
	image.img = stbi_load((path + filename).c_str(), &image.width, &image.height, &image.channels, 4);
	m_images.emplace(filename, image);
}


//Parsers
bool Assets::ParseFromObjFile(std::string path, std::string filename, bool severity)
{
	std::ifstream file(path + filename, std::ifstream::in);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::string word;

	Mesh mesh = {};
	std::string currentMeshId;

	Submesh submesh = {};
	std::string currentSubmeshId = "default";

	// Data for the Parser, ensures no duplicates
	std::unordered_map<std::string, int> indexCountMap;
	int currentIndexSize = 0;
	int indexCount = 0;

	//Data for current OBJ file being read
	std::vector<std::array<float, 3>> posList;
	std::vector<std::array<float, 3>> normalList;
	std::vector<std::array<float, 2>> uvList;
	std::vector<std::string> indiceStrVec;
	
	while (file.good())
	{
		std::getline(file, line);
		if (line.empty()) { continue; }

		size_t cmdl = line.find(" ");
		std::string cmd = line.substr(0, cmdl);
		std::istringstream pstream(line.substr(cmdl, line.size()));
		
		switch (s_CMD.at(cmd))
		{
		//Special commands
		case cmd_comment:
			continue;
		case cmd_o:
			// Create a new object
			if (!submesh.Empty())
			{
				submesh.indiceCount = indexCount;
				mesh.AddSubmesh(currentSubmeshId, submesh);
				indexCount = 0;
			}
			if (!mesh.Empty())
			{
				ParseMesh(mesh,
					indiceStrVec,
					indexCountMap,
					posList,
					normalList,
					uvList);
				AddMesh(currentMeshId, mesh);

				// Reset data
				currentIndexSize = 0;

				mesh.Clear();
				submesh.Clear();
				currentMeshId.clear();
				currentSubmeshId.clear();

				posList.clear();
				normalList.clear();
				uvList.clear();

				indiceStrVec.clear();
				indexCountMap.clear();
			}

			pstream >> currentMeshId;
			submesh.indiceStartIndex = 0;
			submesh.verticeStartIndex = 0;

			break;
		case cmd_s:
			//TODO: Currently not used
			break;

		//Manage vertex data
		case cmd_v:
			posList.push_back(MakeFXYZ(pstream));
			break;

		case cmd_vt:
			uvList.push_back(MakeFXY(pstream));
			break;

		case cmd_vn:
			normalList.push_back(MakeFXYZ(pstream));
			break;

		case cmd_g:
			// Create a group aka submesh
		
			//Push submesh to mesh
			if (indexCount > 0)
			{
				submesh.indiceCount = indexCount;
				mesh.AddSubmesh(currentSubmeshId, submesh);
				submesh.Clear();
				indexCount = 0;
			}


			//Create new submesh
			pstream >> currentSubmeshId;
			submesh.indiceStartIndex = currentIndexSize;
			submesh.verticeStartIndex = 0;

			break;
		case cmd_f:
			//Read all faces on line to last group
			while (!pstream.eof())
			{
				pstream >> word;
				indexCountMap.try_emplace(word, static_cast<int>(indexCountMap.size()));

				indiceStrVec.push_back(word);
				++currentIndexSize;
				++indexCount;
			}
			break;

		//Texture manager
		case cmd_mtllib:
			pstream >> word;
			if (!ParseFromMtlFile(path, word, severity))
				return false;
			break;
		case cmd_usemtl:
			pstream >> word;
			submesh.textureId = word;
			break;

		default: std::cerr << "Error, command not recognized! (" << cmd << ")" << std::endl; 
			if (severity) 
				return false;
			break;
		}
	}
	if (indexCount > 0)
	{
		submesh.indiceCount = indexCount;
		mesh.AddSubmesh(currentSubmeshId, submesh);
		//indexCount = 0;
	}
	if (!mesh.Empty())
	{
		ParseMesh(mesh,
			indiceStrVec,
			indexCountMap,
			posList,
			normalList,
			uvList);
		AddMesh(currentMeshId, mesh);
		//currentIndexSize = 0;
	}

	return file.eof();
	//Fstream is automatically closed on destruction
}
bool Assets::ParseFromMtlFile(std::string path, std::string filename, bool severity)
{
	std::ifstream file(path + filename, std::ifstream::in);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::string word;
	std::string currentTexID;
	Texture tex = {};

	while (file.good())
	{
		std::getline(file, line);
		if (line.empty()) { continue; }

		size_t cmdl = line.find(" ");
		std::string cmd = line.substr(0, cmdl);
		std::istringstream pstream(line.substr(cmdl, line.size()));

		float fline = 0;
		int iline = 0;

		switch (s_CMD.at(cmd))
		{		
		case cmd_comment:
			continue;
		case cmd_newmtl:
			if (!tex.Data().Empty())
			{
				m_textureMap.try_emplace(currentTexID, tex);
				tex.Clear();
			}
			pstream >> currentTexID;
			tex.SetImageKa(DEFAULT_MAP_KA);
			tex.SetImageKd(DEFAULT_MAP_LIGHT);
			tex.SetImageKs(DEFAULT_MAP_LIGHT);
			break;

		case cmd_ns:
			pstream >> fline;
			tex.SetNs(fline);
			break;
		case cmd_ka:
			tex.SetKa(MakeFXYZ(pstream));
			break;
		case cmd_kd:
			tex.SetKd(MakeFXYZ(pstream));
			break;
		case cmd_ks:
			tex.SetKs(MakeFXYZ(pstream));
			break;

		case cmd_mapKa:
			pstream >> word;
			LoadToImage(path, word);
			tex.SetImageKa(word);
			break;
		case cmd_mapKd:
			pstream >> word;
			LoadToImage(path, word);
			tex.SetImageKd(word);
			break;
		case cmd_mapKs:
			pstream >> word;
			LoadToImage(path, word);
			tex.SetImageKs(word);
			break;


		case cmd_ke:
			// Not in use
			break;
		case cmd_ni:
			// Not in use
			break;
		case cmd_d:
			// Not in use
			break;
		case cmd_illum:
			// Not in use
			break;


		default: std::cerr << "Error, command not recognized! (" << cmd << ")" << std::endl;
			if (severity) return false;
			break;
		}
	}
	if (!tex.Empty())
	{
		m_textureMap.try_emplace(currentTexID, tex);
	}
	return file.eof();
	//Fstream is automatically closed on destruction
}
