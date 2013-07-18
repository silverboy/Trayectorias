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
	CDisplayWindowPlots trayectoria2("Trayectoria");

	for(int i=7;i<8;i++){

		char nombre[100];
		x.clear();
		y.clear();

		sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/trayectoria%i.dat",i);
		cout << "Fichero:  " << nombre << endl;

		// Comprobar existencia del archivo
		FILE* file=fopen(nombre,"r");

		if(!file){
			cout << "¡¡¡¡Archivo no encontrado!!! Continuar con el siguiente" << endl;
			continue;
		}

		while(!feof(file)){

			fscanf(file,"x:%lf,y:%lf\n",&px,&py);
			x.push_back(-py);
			y.push_back(px);
		}


		if(i==1){

			TLine2D linea(TPoint2D(y[0],-x[0]),TPoint2D(y[y.size()-1],-x[x.size()-1]));

			double a=linea.coefs[0];
			double b=linea.coefs[1];
			double c=linea.coefs[2];

			//double y_t;

			vector<double> x_l,y_l;

			x_l.push_back(y[0]);
			y_l.push_back(-x[0]);

			for(double y_t=-x[0]-0.2;y_t > -x[x.size()-1];y_t-=0.2 ){

				x_l.push_back(-(b*y_t + c)/a);
				y_l.push_back(y_t);
			}

			x_l.push_back(y[y.size()-1]);
			y_l.push_back(-x[x.size()-1]);

			trayectoria2.clear();
			trayectoria2.plot(x_l,y_l,".b2");

			cout << x_l << endl;
			cout << y_l << endl;


			cout << "Presione cualquier tecla para pasar a la siguiente muestra" << endl;

			mrpt::system::os::getch();





		}

		string fileName(nombre);

		trayectoria.clear();
		trayectoria.setWindowTitle("Trayectoria - " + fileName.substr(fileName.find_last_of("/")+1));
		trayectoria.plot(x,y,".b2");

		trayectoria2.clear();
		trayectoria2.plot(x,y,"-b2");

		fclose(file);


		cout << "Presione cualquier tecla para pasar a la siguiente muestra" << endl;

		mrpt::system::os::getch();

	}

	cout << "Presione cualquier tecla para terminar:" << endl;

	mrpt::system::os::getch();



}





