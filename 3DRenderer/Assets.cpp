#include "Assets.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>




//Help on maps & switch strings by article https://www.codeguru.com/cplusplus/switch-on-strings-in-c/
//By CodeGuru Staff

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
	cmd_f,
	cmd_mtllib,
	cmd_usemtl,

	cmd_newmtl,
	cmd_ns,
	cmd_ka,
	cmd_kd,
	cmd_mapKd,
	cmd_ks,
	cmd_ke,
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
	{"f", cmd_f},
	{"mtllib", cmd_mtllib},
	{"usemtl", cmd_usemtl},
	//Texture file
	{"newmtl", cmd_newmtl},
	{"Ns", cmd_ns},
	{"Ka", cmd_ka},
	{"Kd", cmd_kd},
	{"map_Kd", cmd_mapKd},
	{"Ks", cmd_ks},
	{"Ke", cmd_ke},
	{"Ni", cmd_ni},
	{"d", cmd_d},
	{"illum", cmd_illum}
};

Assets::Assets() {}
Assets::~Assets() {}


const Texture Assets::GetTexture(std::string texId) const
{
	return m_textureMap.at(texId);
}
bool Assets::GetMesh(std::string meshId, Mesh*& mesh) const
{
	if (m_meshMap.count(meshId) > 0)
	{
		*mesh = m_meshMap.at(meshId);
		return true;
	}
	mesh = nullptr;
	return false;
}
const std::unordered_map<std::string, Mesh> Assets::GetMeshMap() const
{
	return m_meshMap;
}

const std::vector<int>& Assets::GetIndiceVector() const
{
	return m_indiceList;
}
const std::vector<Vertex>& Assets::GetVertexVector() const
{
	return m_verticeList;
}

const Vertex* Assets::GetVertexData() const
{
	return m_verticeList.data();
}
const UINT Assets::GetVertexByteWidth() const
{
	return (UINT)m_verticeList.size() * sizeof(Vertex);
}

const int* Assets::GetIndexData() const
{
	return m_indiceList.data();
}
const UINT Assets::GetIndexByteWidth() const
{
	return (UINT)m_indiceList.size() * sizeof(int);
}



bool Assets::ParseFromObjFile(std::string path, std::string filename, bool severity)
{
	std::ifstream file(path + filename, std::ifstream::in);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::string word;
	Mesh mesh;
	int ixyz[3] = {};
	int indiceSize = 0;

	//Data for current OBJ file being read
	std::vector<std::array<float, 3>> posList;
	std::vector <std::array<float, 3>> normalList;
	std::vector <std::array<float, 2>> uvList;
	std::vector<std::string> indiceStrVec; //To be added to the list
	


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
			if (!mesh.Empty())
			{
				AddMesh(mesh, indiceSize);
				indiceSize = 0;
			}
			pstream >> word;
			mesh.SetId(word);
			mesh.SetTextureId("Default");
			mesh.SetIndiceStartIndex(0);
			mesh.SetVerticeStartIndex(0);
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

		case cmd_f:
			while (!pstream.eof())
			{
				pstream >> word;
				if (m_indiceMap.try_emplace(word, m_indiceMapIter).second)
					// True if a new element is added // False if element already exists
				{
					++m_indiceMapIter;
				}
				indiceStrVec.push_back(word);
				++indiceSize;
			}
			break;

		//Texture manager
		case cmd_mtllib:
			pstream >> word;
			if (!ParseFromMtlFile(path + word, severity))
				return false;
			break;
		case cmd_usemtl:
			pstream >> word;
			mesh.SetTextureId(word);
			break;

		default: std::cerr << "Error, command not recognized! (" << cmd << ")" << std::endl; 
			if (severity) return false;
			break;
		}
	}
	if (!mesh.Empty())
	{
		AddMesh(mesh, indiceSize);
		indiceSize = 0;
	}


	// run through list with map, converting to ints for index buffer
	for (int i = 0; i < indiceStrVec.size(); i++)
	{
		m_indiceList.push_back( m_indiceMap.at(indiceStrVec[i]) );
	}
	
	//Removes duplicates from indiceStrVec to ensure no duplicates are added as vertices
	std::vector<std::string> newVec = {};
	std::unordered_set<std::string> set = {};
	std::string element;
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
		sscanf_s(indice.c_str(),"%d/%d/%d",
				&ixyz[0], &ixyz[1], &ixyz[2]); // Reads: Pos/UV/Normal

		// x/y/z is numbered from 1 and up
		m_verticeList.push_back({ posList[(ixyz[0]-1)], normalList[(ixyz[2]-1)], uvList[(ixyz[1]-1)] });
	}

	return file.eof();
	//Fstream is automatically closed on destruction
}
bool Assets::ParseFromMtlFile(std::string path, bool severity)
{
	std::ifstream file(path, std::ifstream::in);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::string word;
	Texture tex;

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
			if (!tex.Empty())
			{
				m_textureMap[tex.GetId()] = tex;
				tex.Clear();
			}
			pstream >> word;
			tex.SetId(word);
			break;

		case cmd_ns:
			pstream >> fline;
			tex.SetSpecularHighlight(fline);
			break;
		case cmd_ka:
			tex.SetAmbientClr(MakeFXYZ(pstream));
			break;
		case cmd_kd:
			tex.SetDiffuseClr(MakeFXYZ(pstream));
			break;
		case cmd_mapKd:

			break;
		case cmd_ks:
			tex.SetSpecularClr(MakeFXYZ(pstream));
			break;
		case cmd_ke:
			tex.SetEmissiveCoef(MakeFXYZ(pstream));
			break;
		case cmd_ni:
			pstream >> fline;
			tex.SetOpticalDensity(fline);
			break;
		case cmd_d:
			pstream >> fline;
			tex.SetDissolve(fline);
			break;
		case cmd_illum:
			//Ignored
			// 
			//pstream >> iline;
			//tex.SetIllum(iline);
			break;


		default: std::cerr << "Error, command not recognized! (" << cmd << ")" << std::endl;
			if (severity) return false;
			break;
		}
	}
	if (!tex.Empty())
	{
		m_textureMap.try_emplace(tex.GetId(), tex);
	}
	return file.eof();
	//Fstream is automatically closed on destruction
}

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
void Assets::AddMesh(Mesh mesh, int indiceSize)
{
	//Adds or overwrites the mesh based on ID to map
	mesh.SetIndiceCount(indiceSize);
	m_meshMap.try_emplace(mesh.GetId(), mesh);
	mesh.Clear();
}