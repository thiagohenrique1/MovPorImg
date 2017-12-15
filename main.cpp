#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "structs.h"

using namespace cv;
using namespace std;

double im_max_y,im_max_x;
double real_max_x, real_max_y;

void print_img(const Mat &img);
void output_file(vector<vector<comando> > comandos, int num);
string format_float(double val);
string format_int(int val);
ponto get_pos(ponto p);

int main(int argc, const char * argv[]) {

    Mat image= imread("imagem.jpg");
    if (!image.data) {
        std::cout << "Imagem não encontrada\n";
        return 1;
    }

//	Aplica filtros
    cv::cvtColor(image, image, CV_BGR2GRAY);
	imwrite("imagem_cinza.jpg",image);
    cv::threshold(image, image, 100, 255, CV_THRESH_BINARY);
	imwrite("imagem_thresh.jpg",image);

	print_img(image);

//	Encontra contornos e armazena os pontos em 'contornos' e a imagem em 'contourOutput'
    vector<vector<Point> > contornos;
    cv::Mat contourOutput = image.clone();
    cv::findContours(contourOutput, contornos, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );

    cout << "Num de contornos: " << contornos.size() << endl;

	im_max_x = image.size().width;
	cout<<"max x: "<<im_max_x<<endl;
	im_max_y = image.size().height;
	cout<<"max y: "<<im_max_y<<endl;

	real_max_x = 660.325;
	real_max_y = 501.800;

	int num = 0;
	vector<vector<comando> > cmds;
	for (size_t i = 0; i < contornos.size(); i++) {
        approxPolyDP(contornos.at(i),contornos.at(i),2,1);

        cout << "Contorno " << i << ", " << contornos.at(i).size() << " pontos:" << endl;

//		Desenha contorno
		Mat contourImage(image.size(), CV_8UC3, cv::Scalar(255,255,255));
		drawContours(contourImage, contornos, i, Scalar(0, 0, 255),3);
		print_img(contourImage);
		if(i == 0) imwrite("contorno.jpg",contourImage);

		cout << "Incluir Contorno? [s/n]" << endl;
		string resp;
		cin >> resp;

//		Inclui caso desejado
		if(resp == "s" || resp == "sim") {
			vector<comando> vec;
			vec.push_back({"MOVL",num++,real_max_x/2,real_max_y/2,214.789});
			for (int j = 0; j < contornos.at(i).size(); j++){
				Point p = contornos.at(i).at(j);
				ponto p_real = get_pos({p.x,p.y,15.173});

				cout << "x: "<< contornos.at(i).at(j).x << ", y: " << contornos.at(i).at(j).y<< endl;
				vec.push_back({"MOVL",num++,p_real.x,p_real.y,p_real.z});
			}

//			Adiciona o primeiro ponto no final dos comandos para fechar a curva
			Point p = contornos.at(i).at(0);
			ponto primeiro = get_pos({p.x,p.y,15.173});
			vec.push_back({"MOVL",num++,primeiro.x,primeiro.y,primeiro.z});

			cmds.push_back(vec);
		}
    }

	vector<comando> vec;
	vec.push_back({"MOVL",num++,real_max_x/2,real_max_y/2,214.789});
	cmds.push_back(vec);

	output_file(cmds,num);

    return 0;
}

ponto get_pos(ponto p){
	return {real_max_x-p.y*(real_max_x/im_max_y),real_max_y-p.x*(real_max_y/im_max_x),p.z};
}

void output_file(vector<vector<comando> > comandos, int num){
	string arquivo;
	arquivo += "/JOB\r\n";
	arquivo += "//NAME MOV_IMG\r\n";
	arquivo += "//POS\r\n";
	stringstream ss;
	ss << num;
	arquivo += "///NPOS "+ss.str()+",0,0,0,0,0\r\n";
	arquivo += "///TOOL 1\r\n";
	arquivo += "///USER 10\r\n";
	arquivo += "///POSTYPE USER\r\n";
	arquivo += "///RECTAN\r\n";
	arquivo += "///RCONF 0,0,0,0,0,0,0,0\r\n";

	for (int i = 0; i < comandos.size(); ++i) {
		for (int j = 0; j < comandos.at(i).size(); ++j) {
			comando cmd_atual = comandos.at(i).at(j);

			arquivo += "C"+format_int(cmd_atual.pos)+"=";

			arquivo += format_float(cmd_atual.x)+",";
			arquivo += format_float(cmd_atual.y)+",";
			arquivo += format_float(cmd_atual.z)+",";
			arquivo += "179.5565,0.1831,0.3047\r\n";
		}
	}
	arquivo += "//INST\r\n";
	arquivo += "///DATE 2017/14/12 09:00\r\n";
	arquivo += "///ATTR SC,RW,RJ\r\n";
	arquivo += "///GROUP1 RB1\r\n";
	arquivo += "NOP\r\n";
	for (int i = 0; i < comandos.size(); ++i) {
		for (int j = 0; j < comandos.at(i).size(); ++j) {
			comando cmd_atual = comandos.at(i).at(j);

			arquivo += cmd_atual.cmd+' ';
			arquivo += "C"+format_int(cmd_atual.pos)+" ";
			arquivo += "V=41.7\r\n";
		}
	}
	arquivo += "END\r\n";

	ofstream file;
	file.open("OUTPUT.JBI");
	file << arquivo;
	file.close();
}

string format_float(double val) {
	stringstream stream;
	stream << fixed << setprecision(3) << val;
	return stream.str();
}

string format_int(int val){
	ostringstream ss;
	ss << std::setw(5) << setfill('0') << val;
	return ss.str();
}

void print_img(const Mat &img) {
	namedWindow("Imagem", WINDOW_NORMAL);
	imshow("Imagem", img);
	resizeWindow("Imagem", 600, 600);
	waitKey(0);
}
