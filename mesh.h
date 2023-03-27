#pragma once
#include <fstream>
#include <vector>
#include "vec.h"
#include <iostream>
#include <string>
//#include <sstream>

class Mesh {
public:
	Mesh(std::string filename, Vec3<float> origin) : origin(origin) {
        std::ifstream test(filename, std::ios::binary);
        for (int i = 0; i < 142120; i++) {
            std::string line;
            std::getline(test, line);
            if (line.size() > 0) {
                if (line[0] == 'v' && line[1] == ' ') {//vertex
                    Vec3<float> newvec(0,0,0);
                    int sin = 0;
                    int sle = 0;
                    int nflo = 1;
                    for (int j = 0; j < line.size(); j++) {
                        if (line[j] != 'v' && line[j] != ' ') {
                            if(sle == 0)
                                sin = j;
                            sle += 1;
                        }
                        else if(sle != 0) {
                            if (nflo == 1) {
                                newvec.x = std::stof(line.substr(sin, sle));
                            }
                            else {
                                newvec.y = std::stof(line.substr(sin, sle));
                            }
                            nflo++;
                            sle = 0;
                        }
                    }
                    newvec.z = std::stof(line.substr(sin, sle));
                    vertices.push_back(newvec);
                }
                else if (line[0] == 'f') {//face that contains indices
                    int sin = 0;
                    int sle = 0;
                    for (int j = 0; j < line.size(); j++) {
                        if (line[j] != 'f' && line[j] != ' ' && line[j] != '/') {
                            if (sle == 0 && line[j - 1] == ' ') {
                                sin = j;
                                sle += 1;
                            }
                            else if (sle != 0) {
                                sle++;
                            }
                        }
                        else if (sle != 0) {
                            this->indices.push_back(std::stoi(line.substr(sin, sle)) - 1);
                            sle = 0;
                        }
                    }
                    //this->indices.push_back(std::stoi(line.substr(sin, sle)));
                }
            }
        }
	}
    Mesh(std::vector<Vec3<float>> vertices, std::vector<int> indices, Vec3<float> origin) : vertices(vertices), indices(indices), origin(origin) {}

public:
    Vec3<float> origin;
    std::vector<Vec3<float>> vertices;
    std::vector<Vec3<float>> normals;
    std::vector<int> indices;
};
