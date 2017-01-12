#ifndef _GRAFO_INCLUDE
#define _GRAFO_INCLUDE

#define _MAX_NOS_GRAFO 100
#define _MAX_ARCOS_GRAFO 200

#define NORTE_SUL	0
#define ESTE_OESTE	1
#define PLANO		2

#include <vector>

using namespace std;

typedef struct No{
	float x,y,z,largura;
}No;

typedef struct Poi {
	string localId;
	int id;
	string nome;
	string categoria;
	string descricao;
	int duracaoVisita;
	string nomeLocal;
	float x;
	float y;

}Poi;

typedef struct Percurso {
	string nome;
	int id;
	string descricao;
	vector<Poi> pois;

}Percurso;

typedef struct Visita {
	string data;
	int UserId;
	int visitaId;
	int percursoId;
	string horaInicio;
	string descricao;
	Percurso rota;
}Visita;

typedef struct Arco{
	int noi,nof;
	float peso,largura;
}Arco;

extern No nos[];
extern Arco arcos[];
extern int numNos, numArcos;

void addNo(No);
void deleteNo(int);
void imprimeNo(No);
void listNos();
No criaNo(float, float, float);

void addArco(Arco);
void deleteArco(int);
void imprimeArco(Arco);
void listArcos();
Arco criaArco(int, int, float, float);

void gravaGrafo();
void leGrafo();
void leGrafoHTTP(int , int, vector<Poi>, vector<Arco>);

#endif