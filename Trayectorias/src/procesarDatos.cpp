#include "Detector.h"
#include "svm.h"
#include <mrpt/gui/CDisplayWindowPlots.h>

/**
 * Fichero para procesar trayectorias 1 a 6 de Pablo, Eugenio y Jorge
 * Las muestreadas a 100 ms (Jorge T7 y Santi) se tratan  en otro programan
 *
 *
 */

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace mrpt::gui;


int obtenerNumeroLineas(char *filename);


int main( int argc, const char* argv[] )
{

	int inicio_scan=44;

	// Indice de archivo de almacenamiento para cada trayectoria (6 en este caso)
	// T7 se trata en caso aparte
	int t_indices[]={0,0,0,0,0,0};

	int fin_scan,n_trayectoria,n_persona,n_posiciones(0);

	char nombre[100];
	char nombre_perfil[100];
	char nombre_sonar[100];

	FILE *perfil,*file;

	Detector detector;

	// Vectores de piernas y cluster
	vector<Cluster> conjuntos;
	vector<Cluster> piernas;



	// Cargar modelo SVM
	struct svm_model *model=svm_load_model("svm_model");
	struct svm_node *instancia;
	double target;

	// datos de cada instancia, 4 en total: 3 carcateristicas más -1 indicando fin
	instancia=Malloc(struct svm_node,4);


	for(int i=1; i <= 108 ; i++){

		// Para cada archivo de sonar
		// Tomo el inicio y leo del archivo de sónar el número de medidas
		sprintf(nombre_sonar,"/home/jplata/Eclipse/MedidasPiernas/17Julio/Datos/sonar%i.dat",i);

		fin_scan=inicio_scan + obtenerNumeroLineas(nombre_sonar);

		n_persona=ceil((i-1)/36);
		n_trayectoria=ceil((i-n_persona*36-1)/6);

		cout << i << " " << n_persona << " " << n_trayectoria << endl;

		// Nombre de archivo donde almacenar la trayectoria
		sprintf(nombre_perfil,"/home/jplata/Eclipse/MedidasPiernas/17Julio/T%i/t%i_%i.dat",
				n_trayectoria+1,n_trayectoria+1,t_indices[n_trayectoria]);


		// Fichero para escribir posciones de la persona
		perfil=fopen(nombre_perfil,"w");
		cout << "Nombre perfil: " << nombre_perfil << endl;



		for(int j=inicio_scan; j < fin_scan; j++){

			// Itero en ese conjunto de datos


			sprintf(nombre,"/home/jplata/Eclipse/MedidasPiernas/17Julio/Datos/laser%i.dat",j);


			// Comprobar existencia del archivo
			file=fopen(nombre,"r");

			if(!file){
				cout << "¡¡¡¡Archivo " << nombre << " no encontrado!!! Continuar con el siguiente" << endl;
				continue;
			}
			fclose(file);

			detector.abrirFichero(nombre,false);

			Eigen::MatrixXf rectas=detector.eliminarRectas(30,181);

			conjuntos.clear();
			piernas.clear();

			conjuntos=detector.clusterizar(0.1,3);


			for(int k=0;k < conjuntos.size();k++){

				// Determinar si es pierna o no
				instancia[0].index=1;
				instancia[1].index=2;
				instancia[2].index=3;
				instancia[3].index=-1;

				instancia[0].value=conjuntos[k].getContorno();
				instancia[1].value=conjuntos[k].getAncho();
				instancia[2].value=conjuntos[k].getProfundidad();

				target=svm_predict(model,instancia);

				if(target==1){
					// El clasificador SVM lo reconoce como pierna
					// Lo agrego al vector de piernas
					piernas.push_back(conjuntos[k]);
				}
			}

			vector<CPose2D> personas=detector.buscarPersonas(piernas);

			if(personas.size() > 1){
				cout << "ATENCION: Detectadas más de 1 persona en vector de piernas!!!!" << endl;
				continue;
			}

			if(personas.size() == 1){
				// Solo se ha detectado una persona, es lo esperado
				// Almaceno su posición en fichero
				fprintf(perfil,"x:%0.3f,y:%0.3f\n",personas[0].x(),personas[0].y());
				n_posiciones++;
			}
			else{
				// No se ha detectado ninguna persona, si se detecta más de una
				// pasa a la siguiente iteracion

				// Si hay dos cluster probablemente sean las dos piernas
				if(conjuntos.size() == 2){
					if(conjuntos[0].getCentro().distanceTo(conjuntos[1].getCentro()) < 0.8){
						CPose2D centro=conjuntos[0].getCentro()+conjuntos[1].getCentro();
						centro.x(centro.x()/2);
						centro.y(centro.y()/2);
						fprintf(perfil,"x:%0.3f,y:%0.3f\n",centro.x(),centro.y());
						n_posiciones++;
					}
				}
				else{
					// No se detectó persona, se indica con posicion (-5,-5)
					fprintf(perfil,"x:%0.3f,y:%0.3f\n",-5,-5);
					n_posiciones++;
				}
			}
		}

		cout << "Inicio: " << inicio_scan << " Fin: " << fin_scan-1 << "  " << nombre_perfil << endl;
		cout << "Puntos: " << n_posiciones << endl;

		if(n_posiciones != fin_scan - inicio_scan){
			cout << "ERROR: No se escribio dato de posicion por scan" << endl;
			exit(0);
		}

		// Cerramos archivo donde se almacena el perfil
		fclose(perfil);

		// Preparamos siguiente iteracion
		inicio_scan=fin_scan;
		t_indices[n_trayectoria]++;
		n_posiciones=0;
	}
}


int obtenerNumeroLineas(char *filename){

	int number_of_lines = 0;
	std::string line;
	std::ifstream myfile(filename);

	while (std::getline(myfile, line))
	{
		++number_of_lines;
	}

	myfile.close();

	return number_of_lines;
}

