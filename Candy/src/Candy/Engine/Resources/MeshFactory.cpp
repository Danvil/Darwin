#include "MeshFactory.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>

namespace Candy
{
	namespace Wavefront
	{
		struct Triple {
			Triple() {}
			Triple(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}
			float x, y, z;
		};

		struct Face {
			int av, at, an;
			int bv, bt, bn;
			int cv, ct, cn;
		};

		struct Mesh {
			std::vector<Triple> v;
			std::vector<Triple> vt;
			std::vector<Triple> vn;
			std::vector<Face> f;
		};

		std::vector<std::string> split(const std::string &s, char delim) {
			std::stringstream ss(s);
			std::string item;
			std::vector<std::string> elems;
			while(std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}

		float toFloat(const std::string& s) {
			std::stringstream ss(s);
			float a;
			ss >> a;
			return a;
		}

		int toInt(const std::string& s, int def=-1) {
			if(s == "") {
				return def;
			}
			std::stringstream ss(s);
			int a;
			ss >> a;
			return a;
		}

		Mesh Read(const std::string& fn) {
			const float scl = 1.0f;
			Mesh mesh;
			// read lines from file
			std::ifstream s(fn.c_str());
			if(!s) {
				throw std::runtime_error("Could not open file '" + fn + "'");
			}
			while(!s.eof()) {
				std::string line;
				std::getline(s, line);
				if(line.size() == 0) {
					continue;
				}
				// split line into tokens
				std::vector<std::string> tokens = split(line, ' ');
				if(tokens.size() == 0) {
					break; // ???
				}
				if(tokens[0] == "v") {
					// vertex position
					mesh.v.push_back(Triple(scl * toFloat(tokens[1]), scl * toFloat(tokens[2]), scl * toFloat(tokens[3])));
				}
				else if(tokens[0] == "vt") {
					// vertex texture coordinates
					if(tokens.size() == 4) {
						mesh.vt.push_back(Triple(toFloat(tokens[1]), toFloat(tokens[2]), toFloat(tokens[3])));
					}
					else {
						mesh.vt.push_back(Triple(toFloat(tokens[1]), toFloat(tokens[2]), 0.0f));
					}
				}
				else if(tokens[0] == "vn") {
					// vertex normal
					mesh.vn.push_back(Triple(toFloat(tokens[1]), toFloat(tokens[2]), toFloat(tokens[3])));
				}
				else if(tokens[0] == "f") {
					std::vector<std::string> a = split(tokens[1], '/');
					std::vector<std::string> b = split(tokens[2], '/');
					std::vector<std::string> c = split(tokens[3], '/');
					Face f;
					// must subtract -1 because wavefront starts indexing at 1 ...
					f.av = toInt(a[0], 0) - 1;
					f.at = toInt(a[1], 0) - 1;
					f.an = toInt(a[2], 0) - 1;
					f.bv = toInt(b[0], 0) - 1;
					f.bt = toInt(b[1], 0) - 1;
					f.bn = toInt(b[2], 0) - 1;
					f.cv = toInt(c[0], 0) - 1;
					f.ct = toInt(c[1], 0) - 1;
					f.cn = toInt(c[2], 0) - 1;
					mesh.f.push_back(f);
				}
				else {
					continue;
				}
			}
			return mesh;
		}

		void WritePos(PTN::Vertex& v, const Triple& u) {
			v.px = u.x;
			v.py = u.y;
			v.pz = u.z;
		}

		void WriteTex(PTN::Vertex& v, const Triple& u) {
			v.u = u.x;
			v.v = u.y;
		}

		void WriteNormal(PTN::Vertex& v, const Triple& u) {
			v.nx = u.x;
			v.ny = u.y;
			v.nz = u.z;
		}

		void WritePos(PN::Vertex& v, const Triple& u) {
			v.px = u.x;
			v.py = u.y;
			v.pz = u.z;
		}

		void WriteNormal(PN::Vertex& v, const Triple& u) {
			v.nx = u.x;
			v.ny = u.y;
			v.nz = u.z;
		}

	}

	Ptr(PTN::Mesh) MeshFactory::LoadObjPTN(const std::string& fn)
	{
		Wavefront::Mesh data = Wavefront::Read(fn);

		typedef PTN::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();

		uint32_t fid = 0;
		V v;
		for(size_t i=0; i<data.f.size(); i++) {
			// first vertex
			if(data.f[i].av != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].av]);
			}
			if(data.f[i].at != -1) {
				Wavefront::WriteTex(v, data.vt[data.f[i].at]);
			}
			if(data.f[i].an != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].an]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
			// second vertex
			if(data.f[i].bv != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].bv]);
			}
			if(data.f[i].bt != -1) {
				Wavefront::WriteTex(v, data.vt[data.f[i].bt]);
			}
			if(data.f[i].bn != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].bn]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
			// third vertex
			if(data.f[i].cv != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].cv]);
			}
			if(data.f[i].ct != -1) {
				Wavefront::WriteTex(v, data.vt[data.f[i].ct]);
			}
			if(data.f[i].cn != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].cn]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
		}

		// create mesh
		PTN::MeshData mesh_data;
		mesh_data.set_vertices(vertices);
//		std::ofstream test("test.txt");
//		test << vertices->size() << std::endl;
//		for(size_t i=0; i<vertices->size(); i++) {
//			test << "v "  << vertices->at(i).px << " " << vertices->at(i).py << " " << vertices->at(i).pz << std::endl;
//		}
		mesh_data.set_indices(indices);
//		test << indices->size() << std::endl;
//		for(size_t i=0; i<indices->size(); i+=3) {
//			test << "f "  << indices->at(i) << " " << indices->at(i+1) << " " << indices->at(i+2) << std::endl;
//		}
		Ptr(PTN::Mesh) mesh(new PTN::Mesh());
		mesh->UpdateMesh(mesh_data);
		return mesh;
	}

	Ptr(PN::Mesh) MeshFactory::LoadObjPN(const std::string& fn)
	{
		Wavefront::Mesh data = Wavefront::Read(fn);

		typedef PN::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();

		uint32_t fid = 0;
		V v;
		for(size_t i=0; i<data.f.size(); i++) {
			// first vertex
			if(data.f[i].av != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].av]);
			}
			if(data.f[i].an != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].an]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
			// second vertex
			if(data.f[i].bv != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].bv]);
			}
			if(data.f[i].bn != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].bn]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
			// third vertex
			if(data.f[i].cv != -1) {
				Wavefront::WritePos(v, data.v[data.f[i].cv]);
			}
			if(data.f[i].cn != -1) {
				Wavefront::WriteNormal(v, data.vn[data.f[i].cn]);
			}
			vertices->push_back(v);
			indices->push_back(fid++);
		}

		// create mesh
		PN::MeshData mesh_data;
		mesh_data.set_vertices(vertices);
//		std::ofstream test("test.txt");
//		test << vertices->size() << std::endl;
//		for(size_t i=0; i<vertices->size(); i++) {
//			test << "v "  << vertices->at(i).px << " " << vertices->at(i).py << " " << vertices->at(i).pz << std::endl;
//		}
		mesh_data.set_indices(indices);
//		test << indices->size() << std::endl;
//		for(size_t i=0; i<indices->size(); i+=3) {
//			test << "f "  << indices->at(i) << " " << indices->at(i+1) << " " << indices->at(i+2) << std::endl;
//		}
		Ptr(PN::Mesh) mesh(new PN::Mesh());
		mesh->UpdateMesh(mesh_data);
		return mesh;
	}

	Ptr(P::Mesh) MeshFactory::LoadObjP(const std::string& fn)
	{
		float scl = 0.1f;
		typedef P::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();
		// read lines from file
		std::ifstream s(fn.c_str());
		if(!s) {
			throw std::runtime_error("Could not open file '" + fn + "'");
		}
		std::string line;
		while(!s.eof()) {
			std::getline(s, line);
			if(line.size() == 0) {
				continue;
			}
			if(line[0] == 'v') {
				std::stringstream line_ss(line);
				std::string token;
				line_ss >> token;
				V v;
				line_ss >> v.px;
				line_ss >> v.py;
				line_ss >> v.pz;
				v.px *= scl;
				v.py *= scl;
				v.pz *= scl;
				vertices->push_back(v);
			}
			else if(line[0] == 'f') {
				std::stringstream line_ss(line);
				std::string token;
				line_ss >> token;
				uint32_t a, b, c;
				line_ss >> a;
				line_ss >> b;
				line_ss >> c;
				//if(!line_ss.eof() && !line_ss.fail()) {
					// workaround faces with only 2 vertices 
					indices->push_back(a);
					indices->push_back(b);
					indices->push_back(c);
				//}
			}
			else {
				continue;
			}
		}
		// create mesh
		P::MeshData data;
		data.set_vertices(vertices);
//		std::ofstream test("test.txt");
//		test << vertices->size() << std::endl;
//		for(size_t i=0; i<vertices->size(); i++) {
//			test << "v "  << vertices->at(i).px << " " << vertices->at(i).py << " " << vertices->at(i).pz << std::endl;
//		}
		data.set_indices(indices);
//		test << indices->size() << std::endl;
//		for(size_t i=0; i<indices->size(); i+=3) {
//			test << "f "  << indices->at(i) << " " << indices->at(i+1) << " " << indices->at(i+2) << std::endl;
//		}
		Ptr(P::Mesh) mesh(new P::Mesh());
		mesh->UpdateMesh(data);
		return mesh;
	}

	Ptr(P::Mesh) MeshFactory::CreateBoxP(float size)
	{
		float a = 0.5f * size;
		typedef P::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		vertices->push_back(V(-a,-a,-a));
		vertices->push_back(V(-a,+a,-a));
		vertices->push_back(V(+a,+a,-a));
		vertices->push_back(V(+a,-a,-a));
		vertices->push_back(V(-a,-a,+a));
		vertices->push_back(V(-a,+a,+a));
		vertices->push_back(V(+a,+a,+a));
		vertices->push_back(V(+a,-a,+a));
		uint32_t indices_data[6][4] = {
			{0,3,2,1}, // z- bottom
			{0,1,5,4}, // x-
			{1,2,6,5}, // y+
			{2,3,7,6}, // x+
			{3,0,4,7}, // y-
			{4,5,6,7}  // z+ top
		};
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();
		for(size_t i=0; i<6; i++) {
			indices->push_back(indices_data[i][0]);
			indices->push_back(indices_data[i][1]);
			indices->push_back(indices_data[i][2]);
			indices->push_back(indices_data[i][0]);
			indices->push_back(indices_data[i][2]);
			indices->push_back(indices_data[i][3]);
		}

		P::MeshData data;
		data.set_vertices(vertices);
		data.set_indices(indices);

		Ptr(P::Mesh) mesh(new P::Mesh());
		mesh->UpdateMesh(data);
		return mesh;
	}

	Ptr(PTN::Mesh) MeshFactory::CreateBoxPTN(float size)
	{
		float a = 0.5f * size;
		float raw_normal[][3] = {
			{0,-1,0},
			{+1,0,0},
			{0,+1,0},
			{-1,0,0},
			{0,0,-1},
			{0,0,+1}
		};
		float raw_pos[][3] = {
			{-1,-1,-1},
			{-1,+1,-1},
			{+1,+1,-1},
			{+1,-1,-1},
			{-1,-1,+1},
			{-1,+1,+1},
			{+1,+1,+1},
			{+1,-1,+1}
		};
		int raw_indices[][4] = {
			{0,3,2,1}, // z- bottom
			{0,1,5,4}, // x-
			{1,2,6,5}, // y+
			{2,3,7,6}, // x+
			{3,0,4,7}, // y-
			{4,5,6,7}  // z+ top
		};

		typedef PTN::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();
		for(size_t i=0; i<6; i++) {
			int t1 = raw_indices[i][0];
			int t2 = raw_indices[i][1];
			int t3 = raw_indices[i][2];
			int t4 = raw_indices[i][3];
			float nx = raw_normal[i][0];
			float ny = raw_normal[i][1];
			float nz = raw_normal[i][2];
			vertices->push_back(V(
				a * raw_pos[t1][0], a * raw_pos[t1][1], a * raw_pos[t1][2],
				nx, ny, nz,
				0, 0
			));
			vertices->push_back(V(
				a * raw_pos[t2][0], a * raw_pos[t2][1], a * raw_pos[t2][2],
				nx, ny, nz,
				1, 0
			));
			vertices->push_back(V(
				a * raw_pos[t3][0], a * raw_pos[t3][1], a * raw_pos[t3][2],
				nx, ny, nz,
				1, 1
			));
			vertices->push_back(V(
				a * raw_pos[t4][0], a * raw_pos[t4][1], a * raw_pos[t4][2],
				nx, ny, nz,
				0, 1
			));
			indices->push_back(4*i);
			indices->push_back(4*i+1);
			indices->push_back(4*i+2);
			indices->push_back(4*i);
			indices->push_back(4*i+2);
			indices->push_back(4*i+3);
		}

		PTN::MeshData data;
		data.set_vertices(vertices);
		data.set_indices(indices);

		Ptr(PTN::Mesh) mesh(new PTN::Mesh());
		mesh->UpdateMesh(data);
		return mesh;
	}

	Ptr(P::Mesh) MeshFactory::CreateTetraederP(float size)
	{
		typedef P::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();
		P::MeshData data;
		data.set_vertices(vertices);
		data.set_indices(indices);
		Ptr(P::Mesh) mesh(new P::Mesh());
		mesh->UpdateMesh(data);
		return mesh;
	}

	Ptr(PTN::Mesh) MeshFactory::CreateTetraederPTN(float a)
	{
		float h = std::sqrt(3.0f) / 2.0f * a;
		float dx = 0.5f * a;
		float dy1 = 0.33333333f * h;
		float dy2 = 0.66666667f * h;
		float dz = std::sqrt(6.0f) / 3.0f * a;
		float raw_normal[][3] = {
			{0,0,-1},
			{0,-0.894427191f,0.4472135955f},
			{+1,0,0}, // FIXME fix normal
			{-1,0,0}  // FIXME fix normal
		};
		float raw_pos[][3] = {
			{-dx,-dy1,0},
			{+dx,-dy1,0},
			{0,+dy2,0},
			{0,0,dz},
		};
		int raw_indices[][4] = {
			{0,2,1}, // lower
			{0,1,3},
			{1,2,3},
			{2,0,3}
		};

		typedef PTN::Vertex V;
		std::vector<V>* vertices = new std::vector<V>();
		std::vector<uint32_t>* indices = new std::vector<uint32_t>();

		for(size_t i=0; i<4; i++) {
			int t1 = raw_indices[i][0];
			int t2 = raw_indices[i][1];
			int t3 = raw_indices[i][2];
			float nx = raw_normal[i][0];
			float ny = raw_normal[i][1];
			float nz = raw_normal[i][2];
			vertices->push_back(V(
				raw_pos[t1][0], raw_pos[t1][1], raw_pos[t1][2],
				nx, ny, nz,
				0, 0
			));
			vertices->push_back(V(
				raw_pos[t2][0], raw_pos[t2][1], raw_pos[t2][2],
				nx, ny, nz,
				1, 0
			));
			vertices->push_back(V(
				raw_pos[t3][0], raw_pos[t3][1], raw_pos[t3][2],
				nx, ny, nz,
				1, 1
			));
			indices->push_back(3*i);
			indices->push_back(3*i+1);
			indices->push_back(3*i+2);
		}

		PTN::MeshData data;
		data.set_vertices(vertices);
		data.set_indices(indices);
		Ptr(PTN::Mesh) mesh(new PTN::Mesh());
		mesh->UpdateMesh(data);
		return mesh;
	}
}
