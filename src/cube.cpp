#include "cube.h"

// Cube 초기화
void Cube::initialize() {
    // Cube의 vertices 정의
    glm::vec4 vertexData[8] = {
        {-0.5, -0.5,  0.5, 1.0}, {0.5, -0.5,  0.5, 1.0},
        { 0.5,  0.5,  0.5, 1.0}, {-0.5,  0.5,  0.5, 1.0},
        {-0.5, -0.5, -0.5, 1.0}, {0.5, -0.5, -0.5, 1.0},
        { 0.5,  0.5, -0.5, 1.0}, {-0.5,  0.5, -0.5, 1.0}
    };

    // Cube의 normal vectors 정의
    glm::vec4 normalData[6] = {
        { 0.0,  0.0,  1.0, 0.0},  // 정면
        { 1.0,  0.0,  0.0, 0.0},  // 오른쪽
        { 0.0,  0.0, -1.0, 0.0},  // 후면
        {-1.0,  0.0,  0.0, 0.0},  // 왼쪽
        { 0.0, -1.0,  0.0, 0.0},  // 하단
        { 0.0,  1.0,  0.0, 0.0}   // 상단
    };

    // Cube의 index 정의
    unsigned int indices[36] = {
        0, 1, 2, 0, 2, 3,  // 정면
        1, 5, 6, 1, 6, 2,  // 오른쪽
        5, 4, 7, 5, 7, 6,  // 후면
        4, 0, 3, 4, 3, 7,  // 왼쪽
        4, 5, 1, 4, 1, 0,  // 하단
        3, 2, 6, 3, 6, 7   // 상단
    };

    // vertices, normal vector 데이터 생성
    for (int i = 0; i < 36; ++i) {
        verts.push_back(vertexData[indices[i]]);
        normals.push_back(normalData[i / 6]);
    }

    // texCoords 정의
    glm::vec2 texData[4] = {
        {0.0f, 0.0f}, {1.0f, 0.0f},
        {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    for (int i = 0; i < 6; ++i) {
        texCoords.push_back(texData[0]);
        texCoords.push_back(texData[1]);
        texCoords.push_back(texData[2]);

        texCoords.push_back(texData[0]);
        texCoords.push_back(texData[2]);
        texCoords.push_back(texData[3]);
    }
}
