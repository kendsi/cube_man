#include "cube.h"

// Cube �ʱ�ȭ
void Cube::initialize() {
    // Cube�� vertices ����
    glm::vec4 vertexData[8] = {
        {-0.5, -0.5,  0.5, 1.0}, {0.5, -0.5,  0.5, 1.0},
        { 0.5,  0.5,  0.5, 1.0}, {-0.5,  0.5,  0.5, 1.0},
        {-0.5, -0.5, -0.5, 1.0}, {0.5, -0.5, -0.5, 1.0},
        { 0.5,  0.5, -0.5, 1.0}, {-0.5,  0.5, -0.5, 1.0}
    };

    // Cube�� normal vectors ����
    glm::vec4 normalData[6] = {
        { 0.0,  0.0,  1.0, 0.0},  // ����
        { 1.0,  0.0,  0.0, 0.0},  // ������
        { 0.0,  0.0, -1.0, 0.0},  // �ĸ�
        {-1.0,  0.0,  0.0, 0.0},  // ����
        { 0.0, -1.0,  0.0, 0.0},  // �ϴ�
        { 0.0,  1.0,  0.0, 0.0}   // ���
    };

    // Cube�� index ����
    unsigned int indices[36] = {
        0, 1, 2, 0, 2, 3,  // ����
        1, 5, 6, 1, 6, 2,  // ������
        5, 4, 7, 5, 7, 6,  // �ĸ�
        4, 0, 3, 4, 3, 7,  // ����
        4, 5, 1, 4, 1, 0,  // �ϴ�
        3, 2, 6, 3, 6, 7   // ���
    };

    // vertices, normal vector ������ ����
    for (int i = 0; i < 36; ++i) {
        verts.push_back(vertexData[indices[i]]);
        normals.push_back(normalData[i / 6]);
    }

    // texCoords ����
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
