Garland&Heckbert Edge Collapse Mesh-Simplify algorithm 
============================
#1. 설명
---------------------------
  Garland & Heckbert 가 제시한 메시 데이터 간소화 알고리즘입니다.  
  CMake CMakeLists.txt 로 빌드한 이후 makefile 을 실행시키시면 됩니다.  
  단, 이 프로그램은 제 마칭 테트라헤드라의 오버 테설레이션 문제를 해결하기 위해 시험적으로  
  제작한 프로그램으로 세분화 되지 않은 Obj 파일에 대해서만 동작합니다.  
  오직 Vertex와 Face 정보만 존재해야하며  
  v v1.x v1.y v1.y  
  ...  

  f v1 v2 v3  
  ...  

  형식의 간단한 형태에 대해서만 동작하는 상태입니다.  

  소스코드를 빌드하신 이후 생성되는 실행파일을 커맨드 환경에서 실행하시면 됩니다.  

  예) mesh_simplify dragon.obj dragon_001.obj 0.01  

  첫번째 인자는 data 폴더 내에 존재하는 오브젝트 파일이며  
  두번째 인자는 output 폴더에 저장될 파일 명입니다.  
  세번째 인자는 원본 페이스 수의 몇 % 정도로 메쉬를 간소화 시킬 것인지에 대한 인자입니다.  
  0.1 을 주게되면 20만개의 페이스로 구성된 메시 데이터를 2만개 페이스로 표현하게 됩니다.  


#2. 알고리즘 설명
---------------------
  '[Garland & Heckberts, Surface Simplification Using Quadric Error Metrics](https://www.cs.cmu.edu/~./garland/Papers/quadrics.pdf)'' 을 참조하십시오.  

  해당 논문에 따라 구현한 알고리즘을 기반으로 Threshold 값을 0.1 로 시작하고  
  사용자가 원하는 비율의 페이스 숫자로 줄일 수 없다면 Threshold를 두배로 증가시켜  
  메쉬 간소화를 수행하도록 만들었습니다.  

  또한 해당 알고리즘을 단순히 적용할 경우 제거될 엣지를 공유하던 페이스들이 한 점으로 합쳐지면서  
  페이스의 방향이 뒤집히는 문제가 있었습니다.  그것을 해결하기 위해 합쳐지기 전 페이스의 내적 값과  
  합쳐진 이후의 페이스의 내적값을 구하여 0보다 작은 경우 (서로의 법선 벡터 방향이 반대인 경우)  
  인덱스 순서를 바꾸도록 재설계하여 해결하였습니다.  

#3. 만든 목적
------------
  제 석사 논문의 주제였던 GPU 기반의 BCC 마칭 테트라헤드라 구현 당시 정규화 된 사면체를 이용하여  
  마칭 테트라헤드라를 적용하면 기존의 문제였던 표현 품질 문제가 완화될 것이라고 생각하였습니다.  
  표현 품질에서는 기존의 정육면체를 6개로 분할하여. 사용한 마칭 테트라헤드라에 비하여 좋았으나  
  기존의 문제를 완전히 해결하지는 못하였습니다.  

  마칭 테트라헤드라가 마칭 큐브 알고리즘에 비해 갖는 단점은 다음과 같습니다.  
  1. 단위 다면체에서 추출되는 삼각형의 수가 마칭 큐브보다 적음에도 불구하고 일어나는 마칭큐브보다 심한 오버테설레이션.  
  2. 좌표계의 형태로 인해 발생하는 범핑 현상(매끈한 표면에 규칙적으로 파여있는 패턴이 나타나는 것).  

  위 두 문제를 해결하기 위해 마칭 테트라헤드라 이후 별도의 후처리 과정을 통해 조금 더 효율적인 메시를 추출해보고자  
  위 알고리즘을 적용하였습니다.  

#4. 결과물
----------

  결과는 다음과 같습니다. 아래의 메시는 약 10만개의 정점과 20만개의 면으로 구성된 등가면 추출 데이터입니다.  

  *Dragon 128 x 128 x 64 Marching Tetrahedra*  
  ![Dragon_128_128_64.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_128_128_64.png)  

  아래는 마칭 테트라헤드라로 추출한 등가면이 오버테설레이션 된 것을 보여줍니다.  

  *Dragon Over Tesselation Sample 1*  
  ![Dragon_128_128_64_overtesselation1.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_128_128_64_overtesselation1.png)  

  *Dragon Over Tesselation Sample 2*  
  ![Dragon_128_128_64_overtesselation2.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_128_128_64_overtesselation2.png)  

  본 페이지의 알고리즘을 적용하여 전체의 10% 가량의 데이터로 축소 표현한 데이터입니다.  

  *Dragon 128 x 128 x 64 Mesh-Simplified*  
  ![Dragon_010.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_010.png)  
  ![Dragon_010_Mesh.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_010_Mesh.png)  
  ![Dragon_010_result.png](https://github.com/elensar92/Mesh-Simplify/blob/master/img/Dragon_010_result.png)  


  기존 데이터에 비해 훨씬 줄어든 데이터 양으로 그럴듯한 표현력을 보여줍니다.  

#5. Enviroment
--------------
OS : Apple MAC 10.13 이상   
Language : C++ 14 이상(CMake 빌드 환경은 C++17 로 되어있음)  
CMake : 3.x 버전 이상  