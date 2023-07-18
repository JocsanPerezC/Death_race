#pragma once

#include "Funciones.h"
using namespace std;
#pragma warning(disable:4996);
#define FPS 60.0

int juego() { 
	if (!al_init()) {
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede inicializar Allegro", NULL, NULL);
		return -1;
	}
	//Se instalan los complementos y periféricos y se hace el la preparacion y inicializacion como en el cpp principal.
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(7);
	ALLEGRO_MONITOR_INFO monitor;
	al_get_monitor_info(0, &monitor);
	const int RX = monitor.x2 - monitor.x1;
	const int RY = monitor.y2 - monitor.y1;

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_FULLSCREEN);
	ALLEGRO_DISPLAY* pantalla = al_create_display(RX, RY);

	al_set_window_title(pantalla, "Death Race");

	if (!pantalla)
	{
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede crear la pantalla", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	//Elementos de allegro.
	ALLEGRO_FONT* fuente1;
	ALLEGRO_FONT* fuente2;
	ALLEGRO_KEYBOARD_STATE teclado;
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS); //Velocidad en tiempo real

	ALLEGRO_TIMER* timerD = al_create_timer(1); //Para el contador.

	ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();

	//Imagenes
	ALLEGRO_BITMAP* cochePlayer = al_load_bitmap("Imagenes/Coche.png");
	ALLEGRO_BITMAP* persona = al_load_bitmap("Imagenes/Persona2.png");
	ALLEGRO_BITMAP* barril = al_load_bitmap("Imagenes/Barril1.png");
	ALLEGRO_BITMAP* bullet = al_load_bitmap("Imagenes/Bala.png");
	ALLEGRO_BITMAP* fondo2 = al_load_bitmap("Imagenes/Fondo2.png");
	ALLEGRO_BITMAP* blood = al_load_bitmap("Imagenes/Blood.png");
	ALLEGRO_BITMAP* cora = al_load_bitmap("Imagenes/Corazon.png");
	//Array para la explosion.
	ALLEGRO_BITMAP* Expl[6];
	Expl[0] = al_load_bitmap("Imagenes/Ex1.png");
	Expl[1] = al_load_bitmap("Imagenes/Ex2.png");
	Expl[2] = al_load_bitmap("Imagenes/Ex3.png");
	Expl[3] = al_load_bitmap("Imagenes/Ex4.png");
	Expl[4] = al_load_bitmap("Imagenes/Ex5.png");
	Expl[5] = al_load_bitmap("Imagenes/Ex6.png");

	//Sonido
	ALLEGRO_SAMPLE* carro = al_load_sample("Musica/carroMotor.mp3");

	ALLEGRO_SAMPLE* explosion = al_load_sample("Musica/explosion.mp3");

	ALLEGRO_SAMPLE* disparo = al_load_sample("Musica/disparo.mp3");

	ALLEGRO_SAMPLE* boomBarril = al_load_sample("Musica/explosionBarril.mp3");

	ALLEGRO_SAMPLE* golpePersona = al_load_sample("Musica/golpePersona.mp3");


	fuente1 = al_load_font("letraDeath.otf", 60, NULL);
	fuente2 = al_load_font("letraDeath.otf", 50, NULL);
	//Se registran los fuentes de eventos.
	al_register_event_source(cola_eventos, al_get_timer_event_source(timerD));
	al_register_event_source(cola_eventos, al_get_timer_event_source(timer));
	al_register_event_source(cola_eventos, al_get_keyboard_event_source());

	//Se declaran las variables a utilizar
	//Para el ciclo.
	bool hecho = true;
	//Para el texto.
	char buffer4[5];
	char buffer3[5];
	char buffer2[5];
	char buffer1[10];

	//Posicion del jugador
	int cochex = 200;
	int cochey = 500;

	//Numero de balas disparadas.
	int contBull = 0;

	//Dimensiones del coche.
	int cocheLargo = al_get_bitmap_height(cochePlayer);
	int cocheAncho = al_get_bitmap_width(cochePlayer);

	//Para el movimiento del jugador.
	int grados = 180;
	int mover = 0;
	int desp = 5;

	//Tiempo de la partida.
	int tiempo = 60;

	//Para cuando se termina la partida.
	int salida = 3;

	//Las lista enlazadas de objetos
	PtrObsta Barriles = NULL;
	PtrBalaEsp Balas = NULL;
	PtrPerso Personas = NULL;
	PtrMancha Manchas = NULL;

	PtrMancha nuevo5; // Para generar manchas

	//Jugador.
	coche player;

	//Inicializa el jugador
	inicializarPlayer(player, RX, RY, cochePlayer);

	//Se crean y añaden los obstaculos.
	for (int i = 0; i < 3; i++) {
		PtrObsta nuevo;
		nuevo = creaBarril(RX, RY, barril, player, Barriles);
		anadirBarril(Barriles, nuevo);
	}

	//Se crean y añaden las balas.
	for (int i = 0; i < 2; i++) {
		PtrBalaEsp nuevo2;
		nuevo2 = creaBala(RX, RY, barril, player);
		anadirBala(Balas, nuevo2);
	}

	//Se crean y añaden las personas.
	for (int i = 0; i < 3; i++) {
		PtrPerso nuevo;
		nuevo = creaPersona(RX, RY, persona, player, 0);
		anadirPersona(Personas, nuevo);
	}

	//Se impia la pantalla.
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_rest(1);

	al_play_sample(carro, 0.1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);//sonido del carro

	//Se inician los timers.
	al_start_timer(timerD);
	al_start_timer(timer);

	while (hecho)
	{
		ALLEGRO_EVENT eventos;
		al_wait_for_event(cola_eventos, &eventos);

		if (eventos.type == ALLEGRO_EVENT_KEY_DOWN)
		{//Para las teclas presionadas:
			switch (eventos.keyboard.keycode) {
				//Si se preciona ESCAPE se sale del juego
			case ALLEGRO_KEY_ESCAPE:
				hecho = false;
				salida = 0;
				break;
				//Si se preciona la flecha izquierda rota en sentido antihorario.
			case ALLEGRO_KEY_LEFT:
				grados -= 45;
				if (grados <= 0) {
					grados = 360;
				}
				break;
				//Si se preciona la flecha derecha se rota en sentido horario.
			case ALLEGRO_KEY_RIGHT:
				grados += 45;
				if (grados >= 360) {
					grados = 0;
				}
				break;
				//Si se presiona arriba se cambia el estado de mover a 1, que significa que esta en movimiento.
			case ALLEGRO_KEY_UP:
				mover = 1;
				break;
				//Se se preciona espacio y la bala se encuentra inactiva y el jugador está activo, se llama a CrearBala y se reproduce el sonido de disparo del jugador.
			case ALLEGRO_KEY_SPACE:
				PtrBalaEsp Aux;
				Aux = Balas;
				bool ciclo = true;
				while (Aux != NULL && ciclo)
				{
					if (Aux->estado == false && contBull >= 10) {
						Aux->estado = true;
						Aux->direccion = grados;
						Aux->x = player.x;
						Aux->y = player.y;
						contBull = 0;
						ciclo = true;
						
						al_play_sample(disparo, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
					else {
						Aux = Aux->Siguiente;
					}
				}
				break;
			}
		}
		//Para las teclas que  o estan pulsadas (Las flechas) se asigna a la direccion un valor que no coincida con Derecha o Izquierda
		if (eventos.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (eventos.keyboard.keycode) {
				//Si se levanta la tacla de arria el coche se deja de mover ya que mover  es igaula 0.
			case ALLEGRO_KEY_UP:
				mover = 0;
				break;
			}
		}
		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			//Eventos de timerD. Este solo nos permitira llevar el conteo del timeo de juego.
			if (eventos.timer.source == timerD) {
				tiempo--;
			}
			//Eventos de timer
			if (eventos.timer.source == timer) {
				contBull++;
				//Si mover es 1 y el jugador esta activo, llama a la funcion que cambia las coordenadas del jugador.
				if (mover == 1 && player.estado) {
					moverPlayer(player, grados, RX, RY);
					
				}

				//Se limpia la pantalla
				al_clear_to_color(al_map_rgb(0, 0, 0));
				//Se imprime el fondo que se va a desplazar hacia arriba por cada vez llamada
				al_draw_scaled_bitmap(fondo2, 0, 0, al_get_bitmap_width(fondo2), al_get_bitmap_height(fondo2), 0, 0, RX, RY, NULL);


				imprimirBarril(Barriles, barril, RX, RY, player, Expl);//Se imprimen los barriles.

				//Colisiones.
				ColiBarBal(Barriles, Balas, boomBarril);// BALAS VS BARRILES


				ColiBar(Barriles, player, boomBarril); // BARRILES VS CARRO

				ColiPer(Personas, player, golpePersona);// PERSONAS VS CARRO

				moverPintarPer(Personas, RX, RY, persona); // Se pueven y pintan a las personas.

				//Se recorren las balas, si estan activas se imprimen, luego se mueven, y si se han salido del area del juego se desactivan.
				PtrBalaEsp Aux;
				Aux = Balas;
				while (Aux != NULL)
				{
					if (Aux->estado == true) {
						al_draw_bitmap(bullet, Aux->x, Aux->y, NULL);
						moverBala(Aux, Aux->direccion);
						if (!colisionObjetos(Aux->x, Aux->y, 10, 10, 170, 68, RX - 340, RY)) {
							Aux->estado = false;
						}
					}
					Aux = Aux->Siguiente;
				}

				//Recorren las manchas y las pintan.
				PtrMancha Aux3;
				Aux3 = Manchas;
				while (Aux3 != NULL)
				{
					al_draw_bitmap_region(blood, 150, 150, 75, 75, Aux3->x, Aux3->y, NULL);
					Aux3 = Aux3->Siguiente;
				}
				//Se recorren las personas y se estas estan desactivas se genera la animacion de sangre en base al atributo cont.
				//Ademas de que al llegar al ultimo fragmento, se genera una mancha para que se mantanga la mancha.
				PtrPerso Aux2;
				Aux2 = Personas;
				while (Aux2 != NULL)
				{
					if (Aux2->estado == false) 
					{
						if (Aux2->cont >= 0 && Aux2->cont < 1) {
							al_draw_bitmap_region(blood, 0, 0, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);												
						}
						else if (Aux2->cont >= 2 && Aux2->cont < 4) {
							al_draw_bitmap_region(blood, 75, 0, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 4 && Aux2->cont < 6) {
							al_draw_bitmap_region(blood, 150, 0, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 6 && Aux2->cont < 8) {
							al_draw_bitmap_region(blood, 0, 75, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 8 && Aux2->cont < 10) {
							al_draw_bitmap_region(blood, 75, 75, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 10 && Aux2->cont < 12) {
							al_draw_bitmap_region(blood, 150, 75, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 12 && Aux2->cont < 14) {
							al_draw_bitmap_region(blood, 0, 150, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont >= 14 && Aux2->cont < 16) {
							al_draw_bitmap_region(blood, 75, 150, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
						}
						else if (Aux2->cont == 16) {
							al_draw_bitmap_region(blood, 150, 150, 75, 75, Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)), NULL);
							nuevo5 = creaMancha(Aux2->x - (((75 / 2) - (al_get_bitmap_width(persona) / 2))), Aux2->y - (((75) / 2) - (al_get_bitmap_height(persona) / 2)));
							anadirMancha(Manchas, nuevo5);
						}
						Aux2->cont++;
					}
					Aux2 = Aux2->Siguiente;
				}

				ColiPerBal(Personas, Balas, golpePersona);// PERSONAS VS BALAS
				
				ColiBarBal(Barriles, Balas, boomBarril);// BARRILES VS BALAS

				ColiBarPer(Barriles, Personas, player);// BARRILES VS PERSONAS


				//Si la vida es menor igual a 0, se desactiva el jugador.
				if (player.vida <= 0)
				{
					player.estado = false;
				}

				//Si esta activo el coche se imprime con la rotacion necesaria
				if (player.estado) {
					al_draw_rotated_bitmap(cochePlayer, player.SizeX / 2, player.SizeY / 2, player.x, player.y, grados * 3.14159 / 180, 0);
				}
				else { //Si esta desactivado en base al contador re reproduce la animacino de explosion y luego se termina la partida.
					if (player.cont >= 0 && player.cont < 5) {
						al_draw_bitmap(Expl[0], player.x - ((al_get_bitmap_width(Expl[0]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[0]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);											
					}
					else if (player.cont >= 5 && player.cont < 10) {
						al_draw_bitmap(Expl[1], player.x - ((al_get_bitmap_width(Expl[1]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[1]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);
					}
					else if (player.cont >= 10 && player.cont < 15) {
						al_draw_bitmap(Expl[2], player.x - ((al_get_bitmap_width(Expl[2]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[2]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);
					}
					else if (player.cont >= 15 && player.cont < 20) {
						al_draw_bitmap(Expl[3], player.x - ((al_get_bitmap_width(Expl[3]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[3]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);
					}
					else if (player.cont >= 20 && player.cont < 25) {
						al_draw_bitmap(Expl[4], player.x - ((al_get_bitmap_width(Expl[4]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[4]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);
					}
					else if (player.cont >= 25 && player.cont < 30) {
						al_draw_bitmap(Expl[5], player.x - ((al_get_bitmap_width(Expl[5]) / 2) - (al_get_bitmap_width(cochePlayer) / 2)), player.y - ((al_get_bitmap_height(Expl[5]) / 2) - (al_get_bitmap_height(cochePlayer) / 2)), NULL);
						
					}
					else {
						salida = 0;

						hecho = false;
					}
					player.cont++;
				}

				//********************Estadisticas***************************
				//Para verlas en pantalla constantemente
				_itoa_s(tiempo, buffer3, 10);//para escribir el tiempo
				if (tiempo < 0) {
					al_draw_text(fuente1, al_map_rgb(255, 300, 15), RX / 2, 40, ALLEGRO_ALIGN_CENTRE, "0");
					hecho = false;
					salida = 0;
				}
				else {
					al_draw_text(fuente1, al_map_rgb(0, 300, 15), RX / 2 + 3, 43, ALLEGRO_ALIGN_CENTRE, buffer3);//efecto sombra
					al_draw_text(fuente1, al_map_rgb(255, 300, 15), RX / 2, 40, ALLEGRO_ALIGN_CENTRE, buffer3);
				}
				al_draw_text(fuente1, al_map_rgb(0, 300, 15), RX / 2 + 3, 8, ALLEGRO_ALIGN_CENTRE, "Death Race");//efecto sombra
				al_draw_text(fuente1, al_map_rgb(255, 300, 15), RX / 2, 5, ALLEGRO_ALIGN_CENTRE, "Death Race");

				_itoa_s(puntos, buffer1, 10);
				
				al_draw_text(fuente2, al_map_rgb(0, 300, 15), RX / 24 + 3, 13, ALLEGRO_ALIGN_LEFT, "Puntuacion:");//efecto sombra
				al_draw_text(fuente2, al_map_rgb(255, 300, 15), RX / 24 , 10, ALLEGRO_ALIGN_LEFT, "Puntuacion:");

				al_draw_text(fuente2, al_map_rgb(0, 300, 15), RX / 24 + 3, 53, ALLEGRO_ALIGN_LEFT, buffer1);//efecto sombra
				al_draw_text(fuente2, al_map_rgb(255, 300, 15), RX / 24, 50, ALLEGRO_ALIGN_LEFT, buffer1);


				al_draw_text(fuente2, al_map_rgb(0, 300, 15), RX / 6 + 3, 13, ALLEGRO_ALIGN_LEFT, "Homicidio en primer grado:");//efecto sombra
				al_draw_text(fuente2, al_map_rgb(255, 300, 15), RX / 6, 10, ALLEGRO_ALIGN_LEFT, "Homicidio en primer grado:");

				_itoa_s(homi, buffer4, 10);
				al_draw_text(fuente2, al_map_rgb(0, 300, 15), RX / 4 + 3, 53, ALLEGRO_ALIGN_LEFT, buffer4);//efecto sombra
				al_draw_text(fuente2, al_map_rgb(255, 300, 15), RX / 4, 50, ALLEGRO_ALIGN_LEFT, buffer4);

				//Se imprimen los corazones.
				if (player.vida == 10)
				{
					al_draw_bitmap(cora, RX - 60, 10, NULL);
					al_draw_bitmap(cora, RX - 120, 10, NULL);
					al_draw_bitmap(cora, RX - 180, 10, NULL);
					al_draw_bitmap(cora, RX - 240, 10, NULL);
				}
				else if (player.vida == 7) {
					al_draw_bitmap(cora, RX - 60, 10, NULL);
					al_draw_bitmap(cora, RX - 120, 10, NULL);
					al_draw_bitmap(cora, RX - 180, 10, NULL);
				}
				else if (player.vida == 4) {
					al_draw_bitmap(cora, RX - 60, 10, NULL);
					al_draw_bitmap(cora, RX - 120, 10, NULL);
				}
				else if (player.vida == 1) {
					al_draw_bitmap(cora, RX - 60, 10, NULL);
				}
			}
		}
		al_flip_display();
	}
	//Si la salida es igual a cero (El jugador perdio la partida)
	if (salida == 0) {
		al_play_sample(explosion, 0.1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		al_destroy_sample(carro);
		//Si es el juego completo se procede con el guardado de los resultados
				
		char puntaje[20] = "";
		strcat_s(puntaje, 20, buffer1);

		char tiempoJ[20] = "";
		strcat_s(tiempoJ, 20, buffer3);

		char nombre[40];
		nombre[0] = char(95);
		char letra;

		int pos = 0;

		int seguir = true;
		while (seguir) {
			ALLEGRO_EVENT eventoos;
			al_wait_for_event(cola_eventos, &eventoos);
			al_clear_to_color(al_map_rgb(0, 0, 0));
			if (eventoos.type == ALLEGRO_EVENT_KEY_CHAR) {
				//Para los eventos de teclado si pos, la ubiciacion del caracter es menor a 40, se procede a escribir el nombre
				if (pos != 40) {
					//Si se preciona la tecla BACKSPACE (Borrar) se porra el caracter de la posicion actual y se disminuye en 1 la posicion
					if (eventoos.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
						if (pos != 0) {
							nombre[pos] = NULL;
							pos = pos - 1;

						}
					}
					//Se se preciona ENTER se procede termina el ciclo
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_ENTER) {
						seguir = false;
					}
					//Se inhabilitan las teclas LEFT, RIGHT, TAB, ESCAPE
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_LEFT) {
					}
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
					}
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_TAB) {
					}
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					}
					//Si se preciona espacio se procede a imprimer "_"
					else if (eventoos.keyboard.keycode == ALLEGRO_KEY_SPACE) {
						nombre[pos] = char(95);
						pos = pos + 1;
					}
					else {
						//Se no se preciona alguna de las teclas anteriores (se preciona un caracter unicode) se procede a escribirlo en el nombre
						if (eventoos.keyboard.unichar != ALLEGRO_KEY_BACKSPACE) {
							nombre[pos] = eventoos.keyboard.unichar;
							pos = pos + 1;
						}
					}
				}
			}
			//Para señalar la ubicacion de escritura se coloca un caracter al final del nombre
			nombre[pos] = '<';
			nombre[pos + 1] = NULL;
			//Muestra en pantalla el puntaje y el nombre escrito
			al_draw_text(fuente1, al_map_rgb(255, 255, 20), RX / 2, RY / 2 - 250, ALLEGRO_ALIGN_CENTRE, "FIN DEL JUEGO");
			al_draw_text(fuente2, al_map_rgb(255, 255, 255), RX / 2, RY / 2 - 150, ALLEGRO_ALIGN_CENTRE, buffer1);
			al_draw_text(fuente2, al_map_rgb(255, 255, 20), RX / 2, RY / 2, ALLEGRO_ALIGN_CENTRE, nombre);
			al_draw_text(fuente2, al_map_rgb(255, 255, 255), RX / 2, RY / 2 + 200, ALLEGRO_ALIGN_CENTRE, "Presione Enter para finalizar");
			al_draw_text(fuente2, al_map_rgb(255, 255, 255), RX / 2, RY / 2 - 50, ALLEGRO_ALIGN_CENTRE, "Ingrese su nombre");
			if (nombre[0] == char(95)) {
				al_draw_text(fuente2, al_map_rgb(255, 255, 255), RX / 2, RY / 2, ALLEGRO_ALIGN_CENTRE, "Escriba su nombre");
			}
			al_flip_display();
		}
		//Se el nombre no es vacio se borra lo que no haya escrito el usuario (como el caracter para ubicar)
		if (pos != 0) {
			nombre[pos] = NULL;
		}
		else {//Si el usuario no escribe el nombre so coloca un signo de interrogacion
			nombre[0] = '?';
		}
		//Se llama a CrearArchivo con los datos obtenidos
		CrearArchivo(nombre, tiempoJ, puntaje);

	}



	//Se destruyen las variables incializadas de Allgero
	al_destroy_timer(timer);
	al_destroy_timer(timerD);

	
	al_destroy_sample(disparo);
	al_destroy_sample(boomBarril);
	al_destroy_sample(golpePersona);
	al_destroy_sample(explosion);

	al_destroy_font(fuente1);
	al_destroy_font(fuente2);
	DestruirBarriles(Barriles);
	DestruirBalas(Balas);
	DestruirPeros(Personas);
	DestruirMancha(Manchas);

	al_destroy_display(pantalla);

	al_destroy_bitmap(cochePlayer);
	al_destroy_bitmap(persona);
	al_destroy_bitmap(barril);
	al_destroy_bitmap(bullet);
	al_destroy_bitmap(fondo2);

	al_destroy_event_queue(cola_eventos);
	//Se termina el juego.
	return salida;
}