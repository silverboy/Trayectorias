#include "Detector.h"
#include "svm.h"
#include <mrpt/gui/CDisplayWindowPlots.h>


#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace mrpt::gui;




int main( int argc, const char* argv[] )
{

	int v_inicio[]={44,1142,2521};
	int v_fin[]={213,1336,2735};

	int inicio,fin;

	int indice=0;

	char nombre[100];
	char nombre_perfil[100];

	Detector detector;


	// Cargar modelo SVM
	struct svm_model *model=svm_load_model("svm_model");
	struct svm_node *instancia;
	double target;

	// datos de cada instancia, 4 en total: 3 carcateristicas más -1 indicando fin
	instancia=Malloc(struct svm_node,4);


	for(int i=0; i < 3 ; i++){

		// Para cada conjunto de datos de una persona concreta
		inicio=v_inicio[i];
		fin=v_fin[i];

		sprintf(nombre_perfil,"/home/jplata/Eclipse/MedidasPiernas/17Julio/laser%i.dat",i);

		// Fichero para escribir posciones de la persona

		for(int j=inicio; j <= fin; j++){

			// Itero en ese conjunto de datos, me encontraré con la misma trayectoria 6 veces




			sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/Datos/laser%i.dat",i);
			//cout << "Fichero:  " << nombre << endl;

			// Comprobar existencia del archivo
			FILE* file=fopen(nombre,"r");

			if(!file){
				cout << "¡¡¡¡Archivo " << nombre << " no encontrado!!! Continuar con el siguiente" << endl;
				continue;
			}


			detector.abrirFichero(nombre,false);


			// Medidas
			puntos=detector.getPuntos();

			gettimeofday(&t_ini,NULL);
			Eigen::MatrixXf rectas=detector.eliminarRectas(30,181);
			gettimeofday(&t_fin,NULL);
			cout << "Tiempo eliminar Rectas: " << timeval_diff(&t_fin,&t_ini) << endl;
			cout << rectas << endl;

			tiempos.push_back(timeval_diff(&t_fin,&t_ini));

			gettimeofday(&t_ini,NULL);

			vector<Cluster> piernas=detector.clusterizar(0.1,3);;

			mx.clear();
			my.clear();


			for(unsigned int i=0;i<puntos->size();i++){
				mx.push_back(puntos->at(i).x());
				my.push_back(puntos->at(i).y());
			}

			medidasPlot.clear();
			string fileName(nombre);
			medidasPlot.setWindowTitle("Medidas - " + fileName.substr(fileName.find_last_of("/")+1));
			medidasPlot.plot(mx,my,".b2");

			for(int j=0;j < rectas.rows();j++){

				Grafico::dibujarLinea(&medidasPlot,rectas(j,0),rectas(j,1),limites);

			}


			clusterPlot.clear();
			clusterPlot.setWindowTitle("Cluster - " + fileName.substr(fileName.find_last_of("/")+1));
			clusterPlot.hold_on();

			piernasPlot.clear();
			piernasPlot.setWindowTitle("Piernas - " + fileName.substr(fileName.find_last_of("/")+1));
			piernasPlot.hold_on();



			// Obtengo puntos clusters
			string formato[2];
			formato[0]=".r2";
			formato[1]=".b2";

			for(int j=0;j < piernas.size();j++){


				puntos=piernas[j].getPuntos();
				px.clear();
				py.clear();

				for(unsigned int k=0;k<puntos->size();k++){
					px.push_back(puntos->at(k).x());
					py.push_back(puntos->at(k).y());
				}
				clusterPlot.plot(px,py,formato[j%2]);

				// Determinar si es pierna o no
				instancia[0].index=1;
				instancia[1].index=2;
				instancia[2].index=3;
				instancia[3].index=-1;

				instancia[0].value=piernas[j].getContorno();
				instancia[1].value=piernas[j].getAncho();
				instancia[2].value=piernas[j].getProfundidad();

				target=svm_predict(model,instancia);

				if(target==1){
					// El clasificador SVM lo reconoce como pierna
					piernasPlot.plot(px,py,formato[j%2]);
				}
				else{
					// No es una pierna, lo elimino del vector
					piernas.erase(piernas.begin()+j);
					j--;
				}

			}

			vector<CPose2D> personas=detector.buscarPersonas(piernas);
			cout << "Personas detectadas: " << personas.size() << endl;

			detector.printClusters(piernas);
			px.clear();
			py.clear();
			for(int k=0;k < personas.size(); k++){
				px.push_back(personas[k].x());
				py.push_back(personas[k].y());
			}
			piernasPlot.plot(px,py,".c4");

			medidasPlot.axis(-0.5,3,-3,3);
			clusterPlot.axis(-0.5,3,-3,3);
			piernasPlot.axis(-0.5,3,-3,3);
			gettimeofday(&t_fin,NULL);
			cout << "Tiempo resto proceso: " << timeval_diff(&t_fin,&t_ini) << endl;

			cout << "Presione cualquier tecla para pasar a la siguiente muestra" << endl;

			mrpt::system::os::getch();














		}


















	}



	vector<double> mx,my,px,py;

	CDisplayWindowPlots medidasPlot("Medidas");
	CDisplayWindowPlots piernasPlot("Cluster");
	vector<CPose2D>* puntos;

	CPose2D pataMesa=getPataMesa();

	// Abrir archivo con perfiles a descartar
	// Dicho archivo contiene por líneas los número de los perfil a descartar
	FILE* file=fopen("descartados.dat","r");
	vector<int> ignorados;

	// Fichero para guardar datos pasa la SVM
	FILE* svm_file=fopen("svm_data.dat","w");

	if(file){
		// Archivo existe
		int indice;
		while(!feof(file)){
			fscanf(file,"%i\n",&indice);
			ignorados.push_back(indice);
		}
		fclose(file);
	}


	for(int i=0;i<552;i++){

		// Comprobar que el archivo no se encuentre entre los ignorados
		if(binary_search(ignorados.begin(),ignorados.end(),i)){
			continue;
		}

		// Salto los correspondientes al maletin
		if(i >= 377 && i <410){
			continue;
		}

		Detector detector;

		char nombre[100];

		sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/23Mayo/laser%i.dat",i);
		cout << "Fichero:  " << nombre << endl;

		// Comprobar existencia del archivo
		FILE* file=fopen(nombre,"r");

		if(!file){
			cout << "¡¡¡¡Archivo no encontrado!!! Continuar con el siguiente" << endl;
			continue;
		}



		detector.abrirFichero(nombre,false);

		// Medidas
		puntos=detector.getPuntos();
		vector<Cluster> piernas;

		if(i > 506){
		// Clusteres detectados
			if(i > 523){
				detector.filtrarDatos();
			}
			piernas=detector.clusterizar(0.4,3);
		}
		else{
			piernas=detector.clusterizar(0.1,3);
		}



		mx.clear();
		my.clear();




		for(unsigned int i=0;i<puntos->size();i++){
			mx.push_back(puntos->at(i).x());
			my.push_back(puntos->at(i).y());
		}

		piernasPlot.clear();
		string fileName(nombre);
		piernasPlot.setWindowTitle("Cluster - " + fileName.substr(fileName.find_last_of("/")+1));
		piernasPlot.hold_on();


		// Obtengo puntos clusters
		string formato[2];
		formato[0]=".r2";
		formato[1]=".b2";

		for(int j=0;j < piernas.size();j++){

			// Si algun cluster coincide con la pata de la mesa lo elimino
			if(pataMesa.distanceTo(piernas[j].getCentro()) < 0.1){
				piernas.erase(piernas.begin()+j);
				j--;

				continue;
			}

			// Guardar datos para la SVM
			if( i<409 ){
				fprintf(svm_file,"%d 1:%f 2:%f 3:%f\n",1, piernas[j].getContorno(), piernas[j].getAncho(), piernas[j].getProfundidad());
			}
			else{
				fprintf(svm_file,"%d 1:%f 2:%f 3:%f\n",-1, piernas[j].getContorno(), piernas[j].getAncho(), piernas[j].getProfundidad());
			}


			puntos=piernas[j].getPuntos();
			px.clear();
			py.clear();

			for(unsigned int k=0;k<puntos->size();k++){
				px.push_back(puntos->at(k).x());
				py.push_back(puntos->at(k).y());
			}
			piernasPlot.plot(px,py,formato[j%2]);
		}

		/*detector.printClusters(piernas);

		medidasPlot.setWindowTitle("Medidas - " + fileName.substr(fileName.find_last_of("/")+1));
		medidasPlot.plot(mx,my,".b2");
		//piernasPlot.plot(px,py,".r2");

		medidasPlot.axis(-0.1,3,-3,3);
		piernasPlot.axis(-0.1,3,-3,3);

		cout << "Presione cualquier tecla para pasar a la siguiente muestra" << endl;

		mrpt::system::os::getch();
*/


	}

	fclose(svm_file);

	//print_vector("%f\t",ancho);



	//plot1.axis(-0.1,0.5,-0.1,0.5);
	//plot2.axis(-0.1,0.5,-0.1,0.5);


	cout << "Presione cualquier tecla para terminar:" << endl;

	mrpt::system::os::getch();



}


