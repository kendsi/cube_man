#pragma once

#include <vector>
#include "glm/glm.hpp"

// Cube 클래스 정의
class Cube {
public:
    std::vector<glm::vec4> verts;      // vertex 리스트
    std::vector<glm::vec4> normals;    // normal vector 리스트
    std::vector<glm::vec2> texCoords;  // texCoords 리스트

    Cube() {
        initialize();
    }
    ~Cube() {
        verts.clear();
        normals.clear();
        texCoords.clear();
    }

private:
    void initialize(); // Cube 데이터를 초기화하는 함수
};
