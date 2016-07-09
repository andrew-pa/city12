#pragma once
#include <dxut/mesh.h>

namespace meshgen {
	struct quad {
		size_t v[4];
	};

	struct qmesh {
		vector<vertex> vertices;
		vector<quad> quads;
	
		qmesh(const vector<vertex>& V = {}, const vector<quad>& Q = {})
			: vertices(V), quads(Q) {}


		mesh_data generate() {
			vector<uint32_t> indices;

			for (const auto& f : quads) {
				indices.push_back(f.v[0]);
				indices.push_back(f.v[1]);
				indices.push_back(f.v[2]);

				indices.push_back(f.v[0]);
				indices.push_back(f.v[2]);
				indices.push_back(f.v[3]);
			}

			return{ vertices, indices };
		}
	};
}