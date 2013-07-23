/*
 * testSVM.cpp

 *
 *  Created on: 17/06/2013
 *      Author: jplata
 */

#include <stdio.h>
#include <vector>
#include <mrpt/poses/CPose2D.h>
#include <mrpt/gui/CDisplayWindowPlots.h>

using namespace std;
using namespace mrpt::gui;


int main( int argc, const char* argv[] )
{

	vector<double> x,y;
	double px,py;

	CDisplayWindowPlots trayectoria("Trayectoria");
	trayectoria.hold_on();

	FILE *file,*salida;

	char nombre[100];
	int trayect=7;

	sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/trayect_interp/t%i_interp.dat",trayect);
	salida=fopen(nombre,"w");

	int indices[]={1,4,10,13};

	// Almacernar puntos de trayectoria original
	sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/trayectoria%i.dat",trayect);
	file=fopen(nombre,"r");


	while(!feof(file)){
		fscanf(file,"x:%lf,y:%lf\n",&px,&py);
		x.push_back(px);
		y.push_back(py);
		fprintf(salida,"x:%0.3f,y:%0.3f\n",px,py);
	}
	fclose(file);



	for(int i=0;i < sizeof(indices)/sizeof(int);i++){

		sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/T%i/t%i_%i.dat",trayect,trayect,indices[i]);

		// Abrir archivo
		file=fopen(nombre,"r");

		if(!file){
			cout << "¡¡¡¡Archivo no encontrado!!! Continuar con el siguiente" << endl;
			continue;
		}


		while(!feof(file)){

			fscanf(file,"x:%lf,y:%lf\n",&px,&py);
			if(px != -5){
				x.push_back(px);
				y.push_back(py);
				fprintf(salida,"x:%0.3f,y:%0.3f\n",px,py);
			}
		}
		fclose(file);
	}

	string fileName(nombre);

	stringstream ss;//create a stringstream
	ss << "Trayectoria ";
	ss << trayect;//add number to the stream

	trayectoria.clear();
	trayectoria.setWindowTitle(ss.str());
	trayectoria.plot(x,y,".b2");

	fclose(salida);

	cout << "Presione cualquier tecla para terminar:" << endl;

	mrpt::system::os::getch();

}





