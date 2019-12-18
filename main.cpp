#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#elif __APPLE__
#include <unistd.h>
#endif
#include "Mesh.h"

using namespace std;

int main(int argc, char * argv[])
{
	if(argc < 4)
	{
		cerr << "입력은 <원본.obj 출력명.obj 제거율> 형식으로 입력하세요." << endl;
		exit(EXIT_FAILURE);
	}

	string input_file_name(argv[1]);
	string output_file_name(argv[2]);
	double ratio = atof(argv[3]);
	char tmp_buf[1024];
	
	if(!getcwd(tmp_buf, 1024))
	{
		cerr << "현재 디렉토리 경로를 읽어오는데 실패하였습니다." << endl;
		exit(EXIT_FAILURE);
	}
	string base_path = string(tmp_buf);
	string full_path = base_path + "/data/" + input_file_name;
	cout << "full_path = " << full_path << endl;
	std::ifstream in(full_path);

	if(!in.is_open()){
		cerr << input_file_name << " 파일을 열 수 없습니다." << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Obj file loading start" << endl;
	mesim::Mesh mesh_simplifier(in);
	cout << "Obj file loading end" << endl;
	in.close();

	size_t origin = mesh_simplifier.getOldFaces();
	size_t remain = (size_t)(origin * ratio);


	cout << "Mesh Simplify algorithm start" << endl;
	mesh_simplifier.simplify(remain, 0.1);
	cout << "Mesh Simplify algorithm end" << endl;

	output_file_name = base_path + "/output/"+ output_file_name;
	ofstream out(output_file_name);

	if(!out.is_open())
	{
		cerr << output_file_name << " 에 결과를 쓸 수 없습니다. " << endl;
		exit(EXIT_FAILURE);
	}

	mesh_simplifier.dumpFile(out);
	out.close();

	return 0;
}