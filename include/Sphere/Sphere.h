#ifndef SPHERE_H
#define SPHERE_H
#include <math.h>
class Sphere {
	std::vector<float> vertices;
	std::vector<int> indices;
	int k1, k2;
	float Radius;
	int Sectors;
	int Stack;
	float x, y, z, xy;
	float nx, ny, nz, lengthInv;
	float s, t;

	float sectorStep;
	float stackStep;
	float sectorAngle, stackAngle;

	public:
		Sphere(float radius, int sectors, int stack) {
			Radius = radius;
			Sectors = sectors;
			Stack = stack;
			stackStep = M_PI / Stack;
			sectorStep = 2 * M_PI / Sectors;
			lengthInv = 1.0f / radius;
		}
		
		std::vector<float> getVertices() {
			for(int i = 0; i<= Stack; i++) {
				stackAngle = M_PI / 2 - i * stackStep;
				xy = Radius * cosf(stackAngle);
				z = Radius * sinf(stackAngle);

				for(int j = 0; j <= Sectors; j++) {
					sectorAngle = j * sectorStep;

					x = xy * cosf(sectorAngle);
					y = xy * sinf(sectorAngle);
					vertices.push_back(x);
					vertices.push_back(y);
					vertices.push_back(z);

					nx = x * lengthInv;
					ny = y * lengthInv;
					nz = z * lengthInv;
					vertices.push_back(nx);
					vertices.push_back(ny);
					vertices.push_back(nz);

					s = (float)j / Sectors;
					t = (float)i / Stack;
					vertices.push_back(s);
					vertices.push_back(t);
				}
			}
			return vertices;
		}

		std::vector<int> getIndices() {
			for(int i = 0; i < Stack; i++) {
				k1 = i * (Sectors + 1);
				k2 = k1 + Sectors + 1;

				for(int j = 0; j < Sectors; j++, k1++,
						k2++) {
					if(i != 0) {
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					if(i != (Stack-1)) {

						indices.push_back(k1 + 1);
						indices.push_back(k2);
						indices.push_back(k2 + 1);
					}
				}
			}
			return indices;
		}

		int getVerticesSize() {
			return vertices.size();
		}

		int getIndicesSize() {
			return indices.size();
		}


		
};

#endif
