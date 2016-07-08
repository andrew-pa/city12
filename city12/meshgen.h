#pragma once
#include <dxut/mesh.h>

namespace meshgen {
	struct face {
		vector<size_t> vertices;
	};

	struct pmesh {
		vector<vertex> vertices;
		vector<face> faces;
	
		
	};

	mesh_data generate(const pmesh& p) {
		vector<uint32_t> indices;

		for (const auto& f : p.faces) {
			for (int i = 0; i < f.vertices.size()-2; i+=3) {
				indices.push_back(f.vertices[i]);
				indices.push_back(f.vertices[i+1]);
				indices.push_back(f.vertices[i+2]);
			}
		}

		return{ p.vertices, indices };
	}
}