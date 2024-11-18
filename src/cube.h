#pragma once

#include <vector>
#include "glm/glm.hpp"

// Cube Ŭ���� ����
class Cube {
public:
    std::vector<glm::vec4> verts;      // vertex ����Ʈ
    std::vector<glm::vec4> normals;    // normal vector ����Ʈ
    std::vector<glm::vec2> texCoords;  // texCoords ����Ʈ

    Cube() {
        initialize();
    }
    ~Cube() {
        verts.clear();
        normals.clear();
        texCoords.clear();
    }

private:
    void initialize(); // Cube �����͸� �ʱ�ȭ�ϴ� �Լ�
};
