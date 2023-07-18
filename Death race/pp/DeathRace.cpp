#pragma once

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <algorithm> 

//Incluimos las funciones de allegro.
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//Incluimos la cabezera que contiene el juego.
#include "Juego.h"//cpp con ya el juego

using namespace std;
#pragma warning(disable:4996);


//Variables globales para el manejo de la interfaz del programa.
const int maxFrame = 2;
int frameactual = 0;
int contadorDeFrames = 0;
int Delay = 35;


int main()
{
	//Si no se inicializa Allegro se lanza el error
	if (!al_init()) {
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede inicializar Allegro", NULL, NULL);
		return -1;
	}
	//Se instalan los complementos y periféricos.
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(7);
	al_install_mouse();

	ALLEGRO_MONITOR_INFO monitor; //Se obtiene informacion del monitor del equipo donde e ejecuta el programa.
	//Se obtiene la información del monitor como su resolución mediante diferencia de puntos finales e iniciales
	al_get_monitor_info(0, &monitor);
	const int RX = monitor.x2 - monitor.x1;
	const int RY = monitor.y2 - monitor.y1;
	al_set_new_display_flags(ALLEGRO_FULLSCREEN | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);//Se modifica el display para que sea pantalla completa.

	ALLEGRO_DISPLAY* pantalla = al_create_display(RX, RY); //De esta manera se ajusta a la pantalla de todas las computadoras en base a las dimensiones de la pantalla.

	al_set_window_title(pantalla, "Death Race");//Se le pone un título a la ventana.
	if (!pantalla)
	{
		//Si el display no se ejecuta se lanza el mensaje de error
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede crear la pantalla", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//Se crean todos los punteros propios de Allegro y se carga el multimedia(imagenes, fuentes, musica) y los timers.
	ALLEGRO_EVENT_QUEUE* colaEventos = al_create_event_queue();

	ALLEGRO_TIMER* timer1 = al_create_timer(0.5 / FPS); //TIMER
	ALLEGRO_TIMER* timer2 = al_create_timer(10 / FPS); //TIMER

	ALLEGRO_BITMAP* fondo1Menu = al_load_bitmap("Imagenes/fondov1.png");
	ALLEGRO_BITMAP* fondo2Menu = al_load_bitmap("Imagenes/fondov2.png");
	ALLEGRO_BITMAP* fondo3Menu = al_load_bitmap("Imagenes/fondov3.png");
	ALLEGRO_BITMAP* estadisticas = al_load_bitmap("Imagenes/estadisticas.png");
	ALLEGRO_BITMAP* instrucciones = al_load_bitmap("Imagenes/instrucciones.png");

	ALLEGRO_SAMPLE* musica = al_load_sample("Musica/musica.mp3");

	ALLEGRO_FONT* fuente1 = al_load_font("letraDeath.otf", 100, NULL);
	ALLEGRO_FONT* fuente2 = al_load_font("letraDeath.otf", 80, NULL);
	ALLEGRO_FONT* fuente3 = al_load_font("letraDeath.otf", 30, NULL);
	ALLEGRO_FONT* fuente4 = al_load_font("letraDeath.otf", 60, NULL);


	//Se registran las fuentes de eventos(timers y periféricos) en la cola de eventos
	al_register_event_source(colaEventos, al_get_timer_event_source(timer1));
	al_register_event_source(colaEventos, al_get_timer_event_source(timer2));
	al_register_event_source(colaEventos, al_get_display_event_source(pantalla));
	al_register_event_source(colaEventos, al_get_keyboard_event_source());
	al_register_event_source(colaEventos, al_get_mouse_event_source());

	//Se obtienen las dimensiones de la pantalla en donde se proyecta el juego para que este se ajuste a las dimensiones
	int X = al_get_display_width(pantalla);
	int Y = al_get_display_height(pantalla);

	//Se inician la coordenadas del mouse.
	int mousex = 0;
	int mousey = 0;

	//Se inician los timers.
	al_start_timer(timer1);
	al_start_timer(timer2);

	//Se inicia la musica.
	al_play_sample(musica, 0.05, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);


	bool menu = true; //Booleano para el ciclo while de cerrado del programa.
	while (menu) //Este while es igual que el de la simulacion solo que simplemente espera a que se presiones escape para terminar el programa.
	{
		ALLEGRO_EVENT eventos; //Se genera un elemnto tipo evento.
		al_wait_for_event(colaEventos, &eventos);//Se le indica que espere por algun evento de los fuentes de eventos.

		if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES)//Si se mueve el mouse
		{
			//Se registra la posición en x y y del mouse
			mousex = eventos.mouse.x;
			mousey = eventos.mouse.y;

		}

		//MENU PRINCIPAL
		if (eventos.type == ALLEGRO_EVENT_TIMER) //Si hay un timer.
		{
			if (eventos.timer.source == timer1)
			{

				if (contadorDeFrames++ >= Delay) {//Se declara un contador de frames que logra avanzar la animación del menú principal
					if (frameactual++ >= maxFrame) {//Al alcansa los frames maximos el frame actual vuelve a 0.
						frameactual = 0;
					}
					contadorDeFrames = 0;
				}

				al_clear_to_color(al_map_rgb(0, 0, 0));//Limpia pantalla.

				//Se reproduce la animacion del fondo en base a los frames.
				if (frameactual == 0) {
					al_draw_scaled_bitmap(fondo1Menu, 0, 0, 1920, 1080, 0, 0, RX + 1000, RY + 600, 0);
				}
				if (frameactual == 1) {
					al_draw_scaled_bitmap(fondo2Menu, 0, 0, 1920, 1080, 0, 0, RX + 1000, RY + 600, 0);
				}
				if (frameactual == 2) {
					al_draw_scaled_bitmap(fondo3Menu, 0, 0, 1920, 1080, 0, 0, RX + 1000, RY + 600, 0);
				}
				//Se imprimen los textos del menu.
				al_draw_text(fuente1, al_map_rgb(250, 250, 15), X / 2 + 2, Y / 10 + 5, ALLEGRO_ALIGN_CENTRE, "Death Race");//Da efecto sombra.
				al_draw_text(fuente1, al_map_rgb(250, 300, 15), X / 2, Y / 10, ALLEGRO_ALIGN_CENTRE, "Death Race");
				al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2, (RY * (250.0 / 768.0)), ALLEGRO_ALIGN_CENTRE, "JUGAR");
				al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2, (RY * (390.0 / 768.0)), ALLEGRO_ALIGN_CENTRE, "RESULTADOS");
				al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2, (RY * (460.0 / 768.0)), ALLEGRO_ALIGN_CENTRE, "INSTRUCCIONES");
				al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2, (RY * (530.0 / 768.0)), ALLEGRO_ALIGN_CENTRE, "SALIR");
			}
		}

		//BOTON DE JUGAR
		if ((mousex >= X / 2 - 42 && mousex <= X / 2 + 42) && (mousey >= (RY * 260.0 / 768.0) && mousey <= (RY * 290.0 / 768.0))) {
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)//Si las coordenadas del mouse estan sobre el texto jugar y se clickea el raton.
			{
				if (eventos.mouse.button & 1) {

					//Se destruye la pantalla y la musica, y se llama al juego, en caso de acabar el juego, se llama a la funcion main para volver al menu.
					al_destroy_display(pantalla);
					al_destroy_sample(musica);
					juego();
					main();//Se vuelve a lanzar el menú si se sale del juego.
					menu = false;
				}
			}
		}

		//BOTON DE RESULTADOS
		if ((mousex >= X / 2 - 120 && mousex <= X / 2 + 120) && (mousey >= (RY * (400.0 / 768.0)) && mousey <= (RY * (430.0 / 768.0)))) {
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)//Si las coordenadas del mouse estan sobre el texto resultados y se clickea el raton.
			{
				if (eventos.mouse.button & 1) {
					bool salir = false;
					while (!salir) {//Mientras salir sea false.
						al_wait_for_event(colaEventos, &eventos);//Se esperara por eventos.	
						if (eventos.type == ALLEGRO_EVENT_TIMER) {
							if (eventos.timer.source == timer1)
							{
								//Si hay un evento timer se limpia la pantalla, el fondo, los textos y se cargan los archivos.
								al_clear_to_color(al_map_rgb(0, 0, 0));

								al_draw_scaled_bitmap(estadisticas, 0, 0, 1920, 1080, 0, 0, RX + 1000, RY + 600, 0);//fondo

								al_draw_text(fuente1, al_map_rgb(0, 300, 15), X / 2, Y / 12, ALLEGRO_ALIGN_CENTRE, "Resultados");//para dar el efecto de sombra
								al_draw_text(fuente1, al_map_rgb(250, 300, 15), X / 2, Y / 12, ALLEGRO_ALIGN_CENTRE, "Resultados");

								CargarArchivo(fuente2, RX);

								al_draw_text(fuente2, al_map_rgb(0, 300, 15), X / 2, (RY * (510.0 / 768.0)) + 200, ALLEGRO_ALIGN_CENTRE, "Atras");//para dar el efecto de sombra
								al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2 + 10, (RY * (510.0 / 768.0)) + 210, ALLEGRO_ALIGN_CENTRE, "Atras");

							}
						}
						if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES)
						{
							mousex = eventos.mouse.x;
							mousey = eventos.mouse.y;
						}
						if ((mousex >= X / 2 - 120 && mousex <= X / 2 + 120) && (mousey >= (RY * (540.0 / 768.0) + 150) && mousey <= (RY * (570.0 / 768.0) + 150))) {
							//Si se presiona atras sale al menú principal
							if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
							{
								if (eventos.mouse.button & 1) {
									salir = true;// Si el mouse esta sobre el texto atras y se realiza un click se termina el bucle y se vuelve al menu principal.
								}
							}
						}
						al_flip_display();
					}
				}
			}
		}

		//BOTON DE INSTRUCCIONES
		if ((mousex >= X / 2 - 120 && mousex <= X / 2 + 120) && (mousey >= (RY * (470.0 / 768.0)) && mousey <= (RY * (500.0 / 768.0)))) {
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) //Si las coordenadas del mouse estan sobre el texto resultados y se clickea el raton.
			{
				if (eventos.mouse.button & 1) {

					bool salir = false;
					while (!salir) {
						al_wait_for_event(colaEventos, &eventos);
						if (eventos.type == ALLEGRO_EVENT_TIMER) {
							if (eventos.timer.source == timer1)
							{
								//Se imprime la informacion de las inctrucciones.
								al_clear_to_color(al_map_rgb(0, 0, 0));

								al_draw_scaled_bitmap(instrucciones, 0, 0, 1920, 1080, 0, 0, RX + 700, RY + 600, 0);//fondo

								al_draw_text(fuente2, al_map_rgb(0, 300, 15), X / 2, (RY * (510.0 / 768.0)) + 200, ALLEGRO_ALIGN_CENTRE, "Atras");//para dar el efecto de sombra
								al_draw_text(fuente2, al_map_rgb(250, 300, 15), X / 2 + 10, (RY * (510.0 / 768.0)) + 210, ALLEGRO_ALIGN_CENTRE, "Atras");

							}
						}
						if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES)
						{
							mousex = eventos.mouse.x;
							mousey = eventos.mouse.y;
						}
						if ((mousex >= X / 2 - 120 && mousex <= X / 2 + 120) && (mousey >= (RY * (540.0 / 768.0) + 150) && mousey <= (RY * (570.0 / 768.0) + 150))) {
							//Si se presiona atras sale al menú principal
							if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
							{
								if (eventos.mouse.button & 1) {
									salir = true;
								}
							}
						}
						al_flip_display();
					}

				}
			}
		}

		//BOTON DE SALIR
		if ((mousex >= X / 2 - 120 && mousex <= X / 2 + 120) && (mousey >= (RY * (540.0 / 768.0)) && mousey <= (RY * (570.0 / 768.0)))) {
			//Si se presiona la  opción de salir se sale de la cola de eventos y termina la aplicación.
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (eventos.mouse.button & 1) {
					menu = false;
				}
			}
		}

		if (eventos.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (eventos.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE: //Si se preciona ESCAPE se sale.
				menu = false;
				break;
			}
		}
		else if (eventos.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			menu = false; //Si se cierra la ventalla
		}
		al_flip_display();//se muestran los cambios realizados
	}

	//Se destruyen todos los elementos utilizados en la palicacion.
	al_destroy_sample(musica);
	al_destroy_timer(timer1);
	al_destroy_timer(timer2);
	al_destroy_bitmap(fondo1Menu);
	al_destroy_bitmap(fondo2Menu);
	al_destroy_bitmap(fondo3Menu);
	al_destroy_bitmap(estadisticas);
	al_destroy_bitmap(instrucciones);
	al_destroy_font(fuente1);
	al_destroy_font(fuente2);
	al_destroy_event_queue(colaEventos);
	al_destroy_display(pantalla);

}