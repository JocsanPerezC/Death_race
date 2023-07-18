#pragma once

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <algorithm> 

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//Incluimos la cabezera que contiene el juego.
#include "Juego.h"

using namespace std;
#pragma warning(disable:4996);

//Estadisticas globales, puntos del juego y muertes a personas.
int puntos = 0;
int homi = 0;

//Nodo de lista enlazada resultados, el cual tiene array de chars para el nombre del jugador, el tiempo que duro, y los puntos obtenidos.
typedef struct resultados
{
	char Nombre[20];
	char Tiempo[20];
	char Puntaje[500];
	resultados* Siguiente;

}*PtrResultados;

//Nodo de lista enlazada mancha, el cual tiene coordenadas x, y para saber donde imprimir una mancha de sangre.
typedef struct mancha {
	int x;
	int y;
	mancha* Siguiente;
}*PtrMancha;

//Nodo de lista enlazada coche, este es el elemento del jugador.
typedef struct coche {
	int codigo;
	int SizeY; //Se refiere al alto.
	int SizeX; //Se refiere al ancho.
	int x; //Coordenada en eje x.
	int y; // Coordenada en eje y.
	int velocidad; //Velocidad del coche.
	bool estado; //Si esta vivo o muerto.
	int vida; //Cantidad de vida del coche.
	int cont; //Contador para la animacion de explosion.
	int dano; //Daño inflinjido al chocar.
	coche* Siguiente;
}*PtrCoche;

//Nodo de lista enlazada balaEsp, estas sera las balas que dispare el jugador.
typedef struct balaEsp {
	int x; //Coordenada en eje x.
	int y; // Coordenada en eje y.
	int SizeY; //Se refiere al alto.
	int SizeX; //Se refiere al ancho.
	int velocidad;//Velocidad de la bala.
	int direccion; //Direccion en la que se mueva la bala.
	int dano; //Daño hecho por la bala.
	bool estado; //Si esta en el aire o no.
	balaEsp* Siguiente;
}*PtrBalaEsp;

//Nodo de lista enlazada obsta, estos seran los barriles que exploten al hacerles daño.
typedef struct obsta {
	int cont; //Contador para la animacion de explosion, y de reaparicion.
	int SizeY; //Se refiere al alto.
	int SizeX; //Se refiere al ancho.
	int x; //Coordenada en eje x.
	int y; // Coordenada en eje y.
	bool estado; //Si esta en pantalla o no.
	int dano; //Daño que hacer al explotar con el coche.
	obsta* Siguiente;
}*PtrObsta;

//Nodo de lista enlazada Persona, estos seran los objetivos a ser asesinados.
typedef struct Persona {
	int codigo; //Tipo de movimiento.
	int cont; //Contador para la animacion de sangre, y de reaparicion.
	int SizeY; //Se refiere al alto.
	int SizeX; //Se refiere al ancho.
	int x; //Coordenada en eje x.
	int y; // Coordenada en eje y.
	int vida; //Vida de la persona.
	int dir; //Direccion del movimiento.
	bool estado; //Si esta vivo o muerto.
	int velocidad; 
	Persona* Siguiente;
}*PtrPerso;

//Dado una coordenada en X y un ancho genera un valor aleatorio que coincida dentro de un rango, para la coordenada X de un obstaculo.
int randomX(int R, int x) { 
	int movR;
	bool ciclo = true;
	movR = 200 + rand() % (R - 330 - x);
	while (ciclo) {
		if (movR < (R - x) && movR > 200)
		return movR;
	}
}

//Dado una coordenada en Y y un alto genera un valor aleatorio que coincida dentro de un rango, para la coordenada Y de un obstaculo.
int randomY(int R, int y) {
	int movR;
	bool ciclo = true;
	movR = 90 + rand() % (R - 90 - y);
	while (ciclo) {
		if (movR < R - y && movR > 90)
		return movR;
	}
}

//Funcion que inicializa al jugador reciviendo la referencia de coche, las dimensiones de la pantalla y el bitmap del coche.
void inicializarPlayer(coche& jugador, int x, int y, ALLEGRO_BITMAP* map) {
	//Se crea la función inicializar jugador que ingresará os valores para cada estructura insertada, en este caso la nave jugador que se inicia a la mitad de la pantalla y en una posición fija en Y .
	jugador.x = x / 2;
	jugador.y = y / 2;
	jugador.vida = 10;
	jugador.velocidad = 10;
	jugador.dano = 3;
	//Toma las medidas de alto y ancho del bitmap.
	jugador.SizeY = al_get_bitmap_height(map); 
	jugador.SizeX = al_get_bitmap_width(map);
	jugador.estado = true;
	jugador.Siguiente = NULL;
	jugador.cont = 0;
}

//Dado dos juegos de coordenada en x, coordenada en y, ancho y largo, devuelve true si detecta una colision.
bool colisionObjetos(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
	if (x1 > x2 + w2 || x1 + w1 < x2 || y1 > y2 + h2 || y1 + h1 < y2) {
		return false;
	}
	else {
		return true;
	}
}

//Se inicializa un obstaculo igual que al jugador solo que se recibe una referencia obstaculo a inciar y el coche como referencia para no generar obstaculos sobre el.
void inicializarObs(obsta& obs, int x, int y, ALLEGRO_BITMAP* map, coche player) {
	bool ciclo = true;
	while (ciclo) {
		obs.x = randomX(x, al_get_bitmap_width(map));
		obs.y = randomY(y, al_get_bitmap_height(map));
		//Si las coordenadas generadas hacen que halla colision con el area jugable pero no con el jugador, se mantienen.
		if (!colisionObjetos(obs.x, obs.y, al_get_bitmap_width(map), al_get_bitmap_height(map), player.x - (player.SizeX / 2), player.y - (player.SizeY / 2), player.SizeX, player.SizeY) && colisionObjetos(obs.x, obs.y, al_get_bitmap_width(map), al_get_bitmap_height(map), 200, 90, x - 130, y)) {
			ciclo = false;
		}
	}
	obs.dano = 3;
	obs.cont = 0;
	obs.SizeY = al_get_bitmap_height(map);
	obs.SizeX = al_get_bitmap_width(map);
	obs.estado = true;
	obs.Siguiente = NULL;
}

//Dado la referencia del jugador, los grados de rotacion, y las dimensiones de la pantalla, se mueve el las coordenadas del coche, solo cuando no estan tocando los bordes.
void moverPlayer(coche& player, int grados, int RX, int RY) {
	switch (grados) {
	case 180:
		if (player.y >= 90 + player.velocidad + 1)
			player.y -= player.velocidad;
		break;
	case 135:
		if (player.y >= 90 + player.velocidad + 1)
			player.y -= player.velocidad;
		if (player.x >= 200 + player.velocidad + 1)
			player.x -= player.velocidad;
		break;
	case 90:
		if (player.x >= 200 + player.velocidad + 1)
			player.x -= player.velocidad;
		break;
	case 45:
		if (player.y + player.SizeY <= RY + 30)
			player.y += player.velocidad;
		if (player.x >= 200 + player.velocidad + 1)
			player.x -= player.velocidad;
		break;
	case 0:
		if (player.y + player.SizeY <= RY + 30)
			player.y += player.velocidad;
		break;
	case 360:
		if (player.y + player.SizeY <= RY + 30)
			player.y += player.velocidad;
		break;
	case 315:
		if (player.y + player.SizeY <= RY + 30)
			player.y += player.velocidad;
		if (player.x + player.SizeX <= RX - 130 - player.velocidad - 1)
			player.x += player.velocidad;
		break;
	case 270:
		if (player.x + player.SizeX <= RX - 130 - player.velocidad - 1)
			player.x += player.velocidad;
		break;
	case 225:
		if (player.y >= 90 + player.velocidad + 1)
			player.y -= player.velocidad;
		if (player.x + player.SizeX <= RX - 130 - player.velocidad - 1)
			player.x += player.velocidad;
		break;
	defult:
		cout << "Indefinido player." << grados << endl;
	}

}

//Dado un puntero a una bala y los grados(direccion) se mueven las coordenadas de la bala.
void moverBala(PtrBalaEsp bala, int grados) {

	switch (grados) {
	case 180:
		bala->y -= bala->velocidad;
		break;
	case 135:
		bala->y -= bala->velocidad;
		bala->x -= bala->velocidad;
		break;
	case 90:
		bala->x -= bala->velocidad;
		break;
	case 45:
		bala->y += bala->velocidad;
		bala->x -= bala->velocidad;
		break;
	case 0:
		bala->y += bala->velocidad;
		break;
	case 360:
		bala->y += bala->velocidad;
		break;
	case 315:
		bala->y += bala->velocidad;
		bala->x += bala->velocidad;
		break;
	case 270:
		bala->x += bala->velocidad;
		break;
	case 225:
		bala->y -= bala->velocidad;
		bala->x += bala->velocidad;
		break;
	}

}

//Dado una persona y un barril dependiendo de la direccion del movimiento de la persona y su posicion con respecto al barril, se busca que rebote de manera consistente.
void cambioPerDir(PtrPerso& perso, PtrObsta bars) {
	switch (perso->codigo) {
	case 1:
		if (perso->y >= bars->SizeY + bars->y + 5) {
			if (perso->dir == 225) {
				perso->dir = 315;
			}
			else if (perso->dir == 135) {
				perso->dir = 45;
			}
		}
		else if (bars->y >= perso->y + perso->SizeY + 5) {
			if (perso->dir == 315) {
				perso->dir = 225;
			}
			else if (perso->dir == 45) {
				perso->dir = 135;
			}
		}
		else if (perso->x > bars->x + bars->SizeX || bars->x > perso->x + perso->SizeX) {
			if (perso->dir == 225) {
				perso->dir = 135;
			}
			else if (perso->dir == 315) {
				perso->dir = 45;
			}
			else if (perso->dir == 135) {
				perso->dir = 225;
			}
			else if (perso->dir == 45) {
				perso->dir = 315;
			}
		}
		break;
	case 2:
		if (perso->dir == 90) {
			perso->dir = 270;
		}
		else if (perso->dir == 270) {
			perso->dir = 90;
		}
		break;
	case 3:
		if (perso->dir == 180) {
			perso->dir = 0;
		}
		else if (perso->dir == 0 || perso->dir == 360) {
			perso->dir = 180;
		}
		break;
	}
}

//Se busca el mismo efecto que con la funcion anterior pero esta vez conparando la persona con el jugador, para que asi no se puedan suicidar las personas.
void cambioPerDirCoche(PtrPerso& perso, coche player) {
	switch (perso->codigo) {
	case 1:
		if (perso->y >= player.SizeY + (player.y - (player.SizeY / 2)) + 5) {
			if (perso->dir == 225) {
				perso->dir = 315;
			}
			else if (perso->dir == 135) {
				perso->dir = 45;
			}
		}
		else if (player.y - (player.SizeY / 2) >= perso->y + perso->SizeY + 5) {
			if (perso->dir == 315) {
				perso->dir = 225;
			}
			else if (perso->dir == 45) {
				perso->dir = 135;
			}
		}
		else if (perso->x > (player.x - (player.SizeX / 2)) + player.SizeX || (player.x - (player.SizeX / 2)) > perso->x + perso->SizeX) {
			if (perso->dir == 225) {
				perso->dir = 135;
			}
			else if (perso->dir == 315) {
				perso->dir = 45;
			}
			else if (perso->dir == 135) {
				perso->dir = 225;
			}
			else if (perso->dir == 45) {
				perso->dir = 315;
			}
		}
		break;
	case 2:
		if (perso->dir == 90) {
			perso->dir = 270;
		}
		else if (perso->dir == 270) {
			perso->dir = 90;
		}
		break;
	case 3:
		if (perso->dir == 180) {
			perso->dir = 0;
		}
		else if (perso->dir == 0 || perso->dir == 360) {
			perso->dir = 180;
		}
		break;
	}
}

//Esta funcion recibe una lista de personas, las dimensiones de la pantalla y un bitmap para las personas. Este mueve y pinte a las personas.
void moverPintarPer(PtrPerso& perso, int RX, int RY, ALLEGRO_BITMAP* map) {
	PtrPerso Aux;
	Aux = perso;
	while (Aux != NULL)
	{
		if (Aux->estado) {//Si la persona esta viva.
			//Primero verifica si en caso de choca con uno de los bordes del area transitable, cambie la direccion.
			switch (Aux->codigo) {
			case 1:
				if (Aux->x + Aux->SizeX >= RX) {
					if (Aux->dir == 225) {
						Aux->dir = 135;
					}
					else if (Aux->dir == 315) {
						Aux->dir = 45;
					}
				}
				else if (Aux->x <= 0) {
					if (Aux->dir == 135) {
						Aux->dir = 225;
					}
					else if (Aux->dir == 45) {
						Aux->dir = 315;
					}
				}
				else if (Aux->y + Aux->SizeY >= RY) {
					if (Aux->dir == 45) {
						Aux->dir = 135;
					}
					else if (Aux->dir == 315) {
						Aux->dir = 225;
					}
				}
				else if (Aux->y <= 68) {
					if (Aux->dir == 135) {
						Aux->dir = 45;
					}
					else if (Aux->dir == 225) {
						Aux->dir = 315;
					}
				}
				//Dependiendo de la direccion se mueve las coordenadas de la persona.
				switch (Aux->dir) {
				case 135:
					Aux->y -= Aux->velocidad;
					Aux->x -= Aux->velocidad;
					break;
				case 45:
					Aux->y += Aux->velocidad;
					Aux->x -= Aux->velocidad;
					break;
				case 315:
					Aux->y += Aux->velocidad;
					Aux->x += Aux->velocidad;
					break;
				case 225:
					Aux->y -= Aux->velocidad;
					Aux->x += Aux->velocidad;
					break;
				}
				break;
			case 2:
				if (Aux->dir == 90 && Aux->x <= 0) {
					Aux->dir = 270;
				}
				else if (Aux->dir == 270 && Aux->x + Aux->SizeX >= RX) {
					Aux->dir = 90;
				}
				switch (Aux->dir) {
				case 90:
					Aux->x -= Aux->velocidad;
					break;
				case 270:
					Aux->x += Aux->velocidad;
					break;
				}
				break;
			case 3:
				if ((Aux->dir == 0 || Aux->dir == 360) && Aux->y + Aux->SizeY >= RY)
					Aux->dir = 180;
				if (Aux->dir == 180 && Aux->y <= 90 + Aux->velocidad + 1) {
					Aux->dir = 0;
				}
				switch (Aux->dir) {
				case 0:
					Aux->y += Aux->velocidad;
					break;
				case 360:
					Aux->y += Aux->velocidad;
					break;
				case 180:
					Aux->y -= Aux->velocidad;
					break;
				}
				break;

			}
			//Finalmente se imprime la persona en pantalla.
			al_draw_bitmap(map, Aux->x, Aux->y, NULL);
		}
		else {//Si la persona esta muerta, si el contador de reaparicion cuenta mayor o igual a 120, revivi la personas y le asigna coordenadas nuevas.
			if (Aux->cont >= 120) {
				Aux->cont = 0;
				Aux->x = rand() % (200 - al_get_bitmap_width(map));
				Aux->y = 90 + (rand() % (RY - al_get_bitmap_width(map) - 90));
				Aux->estado = true;
			}
		}
		Aux = Aux->Siguiente;
	}

}

//Dado un barril y el jugador retorna true, si hay una colision.
bool colisionPlayerObst(coche& player, obsta& barril1) {
	return colisionObjetos(player.x - (player.SizeX / 2), player.y - (player.SizeY / 2), player.SizeX, player.SizeY, barril1.x, barril1.y, barril1.SizeX, barril1.SizeY);
}

//Dado una perosona y el jugador retorna true, si hay una colision.
bool colisionPlayerPers(coche& player, Persona& perso) {
	return colisionObjetos(player.x - (player.SizeX / 2), player.y - (player.SizeY / 2), player.SizeX, player.SizeY, perso.x, perso.y, perso.SizeX, perso.SizeY);
}

//Dado una lista de barrils y las coordenadas y demisiones de un barril, retorna la cantidad de colisiones que encuentre del barril con la lista de barriles.
int colisionBarBar(PtrObsta& barril, int x, int y, int Sx, int Sy) {
	PtrObsta  Aux;
	Aux = barril;
	int cont = 0;
	while (Aux != NULL)
	{
		if (Aux->estado == true && colisionObjetos(Aux->x, Aux->y, Aux->SizeX, Aux->SizeY, x, y, Sx, Sy)) {
			cont++;
		}
		Aux = Aux->Siguiente;
	}
	return cont;
}

//Crea un barril con unas coordenas aleatorias, donde esten en el area jugable, y no colisionen con otro barril o el jugador, y al final retorna el barril.
PtrObsta creaBarril(int x, int y, ALLEGRO_BITMAP* map, coche player, PtrObsta bars) {
	bool ciclo = true;
	PtrObsta obs = new(obsta);
	while (ciclo) {
		obs->x = randomX(x, al_get_bitmap_width(map));
		obs->y = randomY(y, al_get_bitmap_height(map));
		if (!colisionObjetos(obs->x, obs->y, al_get_bitmap_width(map), al_get_bitmap_height(map), player.x - (player.SizeX / 2) - 10, player.y - (player.SizeY / 2) - 10, player.SizeX + 10, player.SizeY + 10) && colisionObjetos(obs->x, obs->y, al_get_bitmap_width(map), al_get_bitmap_height(map), 170, 68, x - 340, y) && colisionBarBar(bars, obs->x - 5, obs->y - 5, al_get_bitmap_width(map) + 5, al_get_bitmap_height(map) + 5) == 0) {
			ciclo = false;
		}
	}
	obs->dano = 3;
	obs->cont = 0;
	obs->SizeY = al_get_bitmap_height(map);
	obs->SizeX = al_get_bitmap_width(map);
	obs->estado = true;
	obs->Siguiente = NULL;
	return obs;
}

//Crea una bala con valores por defecto y la retorna.
PtrBalaEsp creaBala(int x, int y, ALLEGRO_BITMAP* map, coche player) {
	PtrBalaEsp bal = new(balaEsp);
	bal->dano = 3;
	bal->x = 100;
	bal->y = 100;
	bal->SizeY = al_get_bitmap_height(map);
	bal->SizeX = al_get_bitmap_width(map);
	bal->estado = false;
	bal->direccion = -1;
	bal->velocidad = 15;
	bal->Siguiente = NULL;
	return bal;
}

//Crea una persona dandoles de manera aleatira una direccion, y una coordenada randon dentro del area transitable.
PtrPerso creaPersona(int x, int y, ALLEGRO_BITMAP* map, coche player, int direc) {
	bool ciclo = true;
	PtrPerso per = new(Persona);
	per->cont = 0;
	per->x = rand() % (200 - al_get_bitmap_width(map));
	per->y = 90 + (rand() % (y - al_get_bitmap_width(map) - 90));
	per->SizeY = al_get_bitmap_height(map);
	per->SizeX = al_get_bitmap_width(map);
	per->estado = true;
	if (direc == 0) {
		per->codigo = 1;
		switch (rand() % 4) {
		case 0:
			per->dir = 225;
			break;
		case 1:
			per->dir = 45;
			break;
		case 2:
			per->dir = 135;
			break;
		case 3:
			per->dir = 315;
			break;
		}
	}
	per->velocidad = 5;
	per->Siguiente = NULL;
	return per;
}

//Crea un elemento mancha dados unas coordenadas
PtrMancha creaMancha(int x, int y) {
	PtrMancha manch = new(mancha);
	manch->x = x;
	manch->y = y;
	manch->Siguiente = NULL;
	return manch;
}

/*Funciones que añaden elementos de un tipo a una lista correspondiente.*/
void anadirBarril(PtrObsta& barril, PtrObsta& nuevo) {
	nuevo->Siguiente = barril;
	barril = nuevo;
}

void anadirBala(PtrBalaEsp& bala, PtrBalaEsp& nuevo) {
	nuevo->Siguiente = bala;
	bala = nuevo;
}

void anadirPersona(PtrPerso& perso, PtrPerso& nuevo) {
	nuevo->Siguiente = perso;
	perso = nuevo;
}

void anadirMancha(PtrMancha& manch, PtrMancha& nuevo) {
	nuevo->Siguiente = manch;
	manch = nuevo;
}

//Imprime los barriles y lleva el conteo del parametro cont, para la reaparaicion del barril y la explosion.
void imprimirBarril(PtrObsta& barril, ALLEGRO_BITMAP* map, int x, int y, coche player, ALLEGRO_BITMAP* Expl[]) {
	PtrObsta Aux = NULL;
	Aux = barril;
	while (Aux != NULL) {
		if (Aux->estado == true) { //Si el barril esta activo, se imprime en pantalla.
			al_draw_bitmap(map, Aux->x, Aux->y, NULL);
		}
		else {
			if (Aux->cont >= 180) { // Si el barril no esta activo y su contador es mayor a 180, se activa,  se le dan coordenadas nuevas y se reinicia el contador.
				Aux->estado = true;
				bool ciclo = true;
				while (ciclo) {
					Aux->x = randomX(x, al_get_bitmap_width(map));
					Aux->y = randomY(y, al_get_bitmap_height(map));
					if (!colisionObjetos(Aux->x, Aux->y, al_get_bitmap_width(map), al_get_bitmap_height(map), player.x - (player.SizeX / 2) - 10, player.y - (player.SizeY / 2) - 10, player.SizeX + 10, player.SizeY + 10) && colisionObjetos(Aux->x, Aux->y, al_get_bitmap_width(map), al_get_bitmap_height(map), 170, 68, x - 340, y) && colisionBarBar(barril, Aux->x - 5, Aux->y - 5, al_get_bitmap_width(map) + 5, al_get_bitmap_height(map) + 5) == 1) {
						ciclo = false;
					}
				}
				Aux->cont = 0;
			}
			else { // Si esta inactiva y no a pasado del unbral de 180, se reproduce la explosion.
				if (Aux->cont >= 0 && Aux->cont < 5) {
					al_draw_bitmap(Expl[0], Aux->x - ((al_get_bitmap_width(Expl[0]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[0]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				else if (Aux->cont >= 5 && Aux->cont < 10) {
					al_draw_bitmap(Expl[1], Aux->x - ((al_get_bitmap_width(Expl[1]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[1]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				else if (Aux->cont >= 10 && Aux->cont < 15) {
					al_draw_bitmap(Expl[2], Aux->x - ((al_get_bitmap_width(Expl[2]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[2]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				else if (Aux->cont >= 15 && Aux->cont < 20) {
					al_draw_bitmap(Expl[3], Aux->x - ((al_get_bitmap_width(Expl[3]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[3]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				else if (Aux->cont >= 20 && Aux->cont < 25) {
					al_draw_bitmap(Expl[4], Aux->x - ((al_get_bitmap_width(Expl[4]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[4]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				else if (Aux->cont >= 25 && Aux->cont < 30) {
					al_draw_bitmap(Expl[5], Aux->x - ((al_get_bitmap_width(Expl[5]) / 2) - (al_get_bitmap_width(map) / 2)), Aux->y - ((al_get_bitmap_height(Expl[5]) / 2) - (al_get_bitmap_height(map) / 2)), NULL);
				}
				Aux->cont++; //Siempre se aumenta el contador.
			}

		}
		Aux = Aux->Siguiente;
	}

}

//Se recorre la lista de barriles y si hay alguna colision con el jugador se desactiva el barril, y se le resta a la vida del jugador el daño del barril.
void ColiBar(PtrObsta& barril, coche& player, ALLEGRO_SAMPLE* boomBarril) {
	PtrObsta Aux = NULL;
	Aux = barril;
	while (Aux != NULL) {
		if (Aux->estado == true) {
			if (colisionPlayerObst(player, *Aux)) {

				al_play_sample(boomBarril, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

				Aux->estado = false;
				player.vida -= Aux->dano;

			}
		}
		Aux = Aux->Siguiente;
	}

}

//Se recorre la lista de personas y si hay alguna colision con el jugador se desactiva la persona, se suman 10 puntos al los puntos y 1 a los homicidios.
void ColiPer(PtrPerso& perso, coche& player, ALLEGRO_SAMPLE* golpePersona) {
	PtrPerso Aux = NULL;
	Aux = perso;
	while (Aux != NULL) {
		if (Aux->estado == true) {
			if (colisionPlayerPers(player, *Aux)) {

				al_play_sample(golpePersona, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

				Aux->estado = false;
				puntos += 10;
				homi++;
			}
		}
		Aux = Aux->Siguiente;
	}
}

//Se recorren la lista de personas y barriles, y si hay una colision de personas con barriles o personas con el jugador se cambia la direccion de la perosna.
void ColiBarPer(PtrObsta& barril, PtrPerso& perso, coche& player) {
	PtrObsta Aux = NULL;
	PtrPerso Aux2 = NULL;
	Aux = barril;
	while (Aux != NULL) {
		Aux2 = perso;
		if (Aux->estado == true) {
			while (Aux2 != NULL) {
				if (colisionObjetos(Aux->x - 15, Aux->y - 15, Aux->SizeX + 15, Aux->SizeY + 15, Aux2->x - 10, Aux2->y - 10, Aux2->SizeX + 10, Aux2->SizeY + 10)) {
					cambioPerDir(Aux2, Aux);
				}
				if (colisionObjetos(Aux2->x - 25, Aux2->y - 25, Aux2->SizeX + 25, Aux2->SizeY + 25, player.x - (player.SizeX / 2) - 5, player.y - (player.SizeY / 2) - 5, player.SizeX + 5, player.SizeY + 5)) {
					cambioPerDirCoche(Aux2, player);
				}
				Aux2 = Aux2->Siguiente;
			}
		}
		Aux = Aux->Siguiente;
	}
}

//Se recorren balas y barriles y se ambos estan activos y hay una colision se desactivan los dos.
void ColiBarBal(PtrObsta& barril, PtrBalaEsp& bala, ALLEGRO_SAMPLE* boomBarril) {
	PtrObsta Aux = NULL;
	PtrBalaEsp Aux2 = NULL;
	Aux = barril;
	while (Aux != NULL) {
		Aux2 = bala;
		if (Aux->estado == true && Aux2->estado == true) {
			while (Aux2 != NULL) {
				if (colisionObjetos(Aux->x, Aux->y, Aux->SizeX, Aux->SizeY, Aux2->x, Aux2->y, 2, 2)) {

					al_play_sample(boomBarril, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

					Aux->estado = false;
					Aux2->estado = false;
				}
				Aux2 = Aux2->Siguiente;
			}
		}
		Aux = Aux->Siguiente;
	}

}

//Se recorren balas y personas y se ambos estan activos se resta el daño de la bala a la vida de la persona, se desactiva la bala y si la vida de la persona es menor igual que 0, se desactiva la persona.
void ColiPerBal(PtrPerso& perso, PtrBalaEsp& bala, ALLEGRO_SAMPLE* golpePersona) {
	PtrPerso Aux = NULL;
	PtrBalaEsp Aux2 = NULL;
	Aux = perso;
	while (Aux != NULL) {
		Aux2 = bala;
		if (Aux->estado == true && Aux2->estado == true) {
			
			while (Aux2 != NULL) {
				if (colisionObjetos(Aux->x, Aux->y, Aux->SizeX, Aux->SizeY, Aux2->x, Aux2->y, 2, 2) && Aux2->estado == true) {
					Aux->vida -= Aux2->dano;
				
					if (Aux->vida <= 0) {
						Aux->estado = false;
						puntos += 5;
						
						al_play_sample(golpePersona, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

						homi++;
						
					}
					Aux2->estado = false;
				}
				Aux2 = Aux2->Siguiente;
			}
		}
		Aux = Aux->Siguiente;
	}

}

void DestruirBarriles(PtrObsta& bar)//Destruye una lista enlazada de elemetos tipo obsta, tomada de ejemplos antiguos.
{
	PtrObsta Aux;
	Aux = bar;
	while (Aux != NULL)
	{
		bar = bar->Siguiente;
		delete(Aux);
		Aux = bar;
	}
}

void DestruirBalas(PtrBalaEsp& bar)//Destruye una lista enlazada de elemetos tipo balaEsp, tomada de ejemplos antiguos.
{
	PtrBalaEsp Aux;
	Aux = bar;
	while (Aux != NULL)
	{
		bar = bar->Siguiente;
		delete(Aux);
		Aux = bar;
	}
}

void DestruirPeros(PtrPerso& bar)//Destruye una lista enlazada de elemetos tipo Persona, tomada de ejemplos antiguos.
{
	PtrPerso Aux;
	Aux = bar;
	while (Aux != NULL)
	{
		bar = bar->Siguiente;
		delete(Aux);
		Aux = bar;
	}
}

void DestruirMancha(PtrMancha& bar)//Destruye una lista enlazada de elemetos tipo Mancha, tomada de ejemplos antiguos.
{
	PtrMancha Aux;
	Aux = bar;
	while (Aux != NULL)
	{
		bar = bar->Siguiente;
		delete(Aux);
		Aux = bar;
	}
}
//Se crea la función CrearArchivo que guarda el nombre, el puntaje y el tiempo en un archivo en memoria secundaria.
void CrearArchivo(char* nombre, char* tiempo, char* puntaje)
{
	FILE* archivo;
	archivo = fopen("resultados.txt", "a");

	if (NULL == archivo) {
		fprintf(stderr, "No se pudo crear archivo %s.\n", "resultados.txt");
		exit(-1);
	}
	else {
		fprintf(archivo, "Nombre:%s\n", nombre);
		fprintf(archivo, "Tiempo: %s\n", tiempo);
		fprintf(archivo, "Puntaje: %s\n", puntaje);
		fprintf(archivo, "\n\n");
	}
	fclose(archivo);
}

//Destruye una lista enlazada de elementos tipo puntaje.
void DestruirInventario(PtrResultados& Lista)
{
	PtrResultados Aux;
	Aux = Lista;
	while (Aux != NULL)
	{
		Lista = Lista->Siguiente;
		delete(Aux);
		Aux = Lista;
	}
}

//Crea un objeto tipo resultado que guarda las estadisticas de una partida.
PtrResultados CrearArticulo(char* nombre, char* tiempo, char* puntaje)
{
	//crea el elemento
	PtrResultados Pieza = new(resultados);//se crea una pieza

	strcpy(Pieza->Nombre, nombre);
	strcpy(Pieza->Tiempo, tiempo);
	strcpy(Pieza->Puntaje, puntaje);

	Pieza->Siguiente = NULL;

	return Pieza;
}

//Agregar al inicio para implementar un push.
void AgregarInicioInventario(PtrResultados& Lista, PtrResultados& Nuevo)
{
	Nuevo->Siguiente = Lista;
	Lista = Nuevo;
}

//Push que agrega un elemento a una lista enlazada en su primera posicion.
void Push(PtrResultados& Lista, PtrResultados Nuevo) { 
	AgregarInicioInventario(Lista, Nuevo);
}

//Retorna sin desligar, el primer valor en el tope de la pila, osea el primero de la lista enlazada.
PtrResultados Top(PtrResultados& Lista) {
	return Lista;
}

//Retorna desligando, el primer valor en el tope de la pila, osea el primero de la lista enlazada.
PtrResultados Pop(PtrResultados& Lista) {
	PtrResultados Aux = Lista;//Apuntar el primer elemento.
	Lista = Lista->Siguiente;//La lista tiene que  apuntar al siguiente o segundo elemento de la lista.
	Aux->Siguiente = NULL;//Olvidese que apuntaba al primer elemento o al elemento que estaba apuntando la lista al inicio.
	return Aux;
}

//Esta funcion carga el archivo de partidas anteriores e imprime en pantalla las ultimas tres.
void CargarArchivo(ALLEGRO_FONT* fuente2, int RX) //Se carga el archivo en memoria secundaria a pantalla con el puntaje y nombre escritos en el display.
{

	PtrResultados lista = NULL;
	PtrResultados Nuevo;

	//Buffer para el texto.
	char nombre[40];
	char tiempo[60];
	char puntaje[50];

	FILE* archivo;

	fopen_s(&archivo, "resultados.txt", "r"); //Se abre al archivo.


	int j = 0;
	if (archivo != NULL)
	{
		while (!feof(archivo)) {
			fscanf(archivo, "Nombre:%s\n", nombre);
			fscanf(archivo, "Tiempo: %s\n", tiempo);
			fscanf(archivo, "Puntaje: %s\n", puntaje);
			fscanf(archivo, "\n\n");

			Nuevo = CrearArticulo(nombre, tiempo, puntaje);//Se crean los valores.
			Push(lista, Nuevo);//Se envia la estrcutura y el elemento nuevo a insertar.
		}

		//Se imprimen las partidas en pantalla.
		for (int x = 1; x <= 3; x++)
		{

			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 200 + 5, NULL, "Nombre:");//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 200, NULL, "Nombre:");

			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 300 + 5, NULL, Top(lista)->Nombre);//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 300, NULL, Top(lista)->Nombre);


			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 400 + 5, NULL, "Tiempo:");//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 400, NULL, "Tiempo:");

			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 500 + 5, NULL, Top(lista)->Tiempo);//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 500, NULL, Top(lista)->Tiempo);


			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 600 + 5, NULL, "Puntaje:");//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 600, NULL, "Puantaje:");

			al_draw_text(fuente2, al_map_rgb(0, 300, 15), (RX / 6 + 5) + j, 700 + 5, NULL, Top(lista)->Puntaje);//para dar el efecto de sombra
			al_draw_text(fuente2, al_map_rgb(250, 300, 15), (RX / 6) + j, 700, NULL, Top(lista)->Puntaje);

			j += 550;

			Pop(lista);//se elimina el top de la lista
		}
		DestruirInventario(lista);

		fclose(archivo);
	}
}