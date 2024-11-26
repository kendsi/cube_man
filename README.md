# CAU 24-2 COMPUTER GRAPHICS PROJECT

## Project01 - Cubeman running - 🆗

### 프로젝트 내용: 정육면체(cube)를 변형하여 사람 모델을 생성하고 사람이 뛰어가는 애니메이션을 생성한다.

1. 모델 생성
   가로, 세로, 높이가 1인 정육면체로(cube)를 변형하여 사람 모델 생성

2. 애니메이션 생성
   사람이 뛰어가는 효과를 내주는 Transformation을 계산하여 적용
   freeGLUT의 idlecallback function에서 시간에 따라 변하는 Transformation을 계산
   이를 위해 속도를 설정하고 매 프레임마다 팔과 다리를 회전하는 Rotation을 계산

3. 시점 생성
   키 입력을 통해 side view, over-the-shoulder, front view의 세가지 시점을 생성

## Project02 - Project02 - lighting on a textured running cubeman - 🆗

### 프로젝트 내용: 지난 프로젝트의 뛰어가는 사람 모델에 조명과 텍스처 매핑을 추가한다.

1. 조명 illumination
   뛰는 사람 모델에 조명을 추가
   조명의 파라미터는 각자 정의하여 생성
   조명 생성을 위해 모델에 노말 벡터(normal vector)를 추가

2. 텍스처 매핑 texture mapping\
   뛰는 사람 모델에 텍스처 매핑을 추가
   텍스처 매핑을 위해 모델에 texture coordinate 추가
   노말 벡터를 추가하는 것과 유사하게 텍스처 좌표를 저장하는 배열을 추가

3. 정육면체 외에 여러 도형을 사용
   얼굴은 구 (sphere)로 표현하고 몸통, 팔다리는 육면체로 표현하는 등 2개 이상의 도형을 사용
