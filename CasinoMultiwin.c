#include <stdio.h>                         
#include <stdlib.h>
#include <time.h>
#include <ctype.h>       //https://man7.org/linux/man-pages/man0/ctype.h.0p.html
#include <string.h>
#include <termios.h>     //https://man7.org/linux/man-pages/man0/termios.h.0p.html
#include <unistd.h>      //https://man7.org/linux/man-pages/man0/unistd.h.0p.html

// Definimos el archivo como una constante de preprocesador
#define ARCHIVO "saldo.txt"

// ---FUNCIONES DE GESTION DE SALDO (USAN PUNTEROS)---

// Recibe la DIRECCION de memoria de saldo (int *ptr_saldo) para modificarlo
void cargar_saldo(int *ptr_saldo) {
    FILE *f = fopen(ARCHIVO, "r");
    if (f != NULL) {
        // Leemos del archivo directamente a la direccion de memoria a la que apunta ptr_saldo
        fscanf(f, "%d", ptr_saldo);
        fclose(f);
    }
}

// Aqui solo se usa el valor, no hace falta modificarlo
void guardar_saldo(int saldo_actual) {
    FILE *f = fopen(ARCHIVO, "w");
    if (f != NULL) {
        fprintf(f, "%d", saldo_actual);
        fclose(f);
    }
}

// Limpia la consola detectando si es Windows o Linux/Mac
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Captura una pulsacion de tecla sin esperar a que el usuario presione Enter
char getch_custom(void) {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// --- ESTRUCTURAS ---
typedef struct {
    int numero;
    int color;
    int fila;
    int docena;
    int mitad;
} NumeroRuleta;

typedef struct {
    int tipo;
    int opcion;
    int cantidad;
} ApuestaRuleta;

// --- INTERFAZ GRAFICA ---

void logo() {
    printf("╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                    ║\n");
    printf("║         ██████╗ █████╗ ███████╗██╗███╗   ██╗ ██████╗               ║\n");
    printf("║        ██╔════╝██╔══██╗██╔════╝██║████╗  ██║██╔═══██╗              ║\n");
    printf("║        ██║     ███████║███████╗██║██╔██╗ ██║██║   ██║              ║\n");
    printf("║        ██║     ██╔══██║╚════██║██║██║╚██╗██║██║   ██║              ║\n");
    printf("║        ╚██████╗██║  ██║███████║██║██║ ╚████║╚██████╔╝              ║\n");
    printf("║         ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝               ║\n");
    printf("║                                                                    ║\n");
    printf("║    ███╗   ███╗██╗   ██╗██╗  ████████╗██╗██╗    ██╗██╗███╗   ██╗    ║\n");
    printf("║    ████╗ ████║██║   ██║██║  ╚══██╔══╝██║██║    ██║██║████╗  ██║    ║\n");
    printf("║    ██╔████╔██║██║   ██║██║     ██║   ██║██║ █╗ ██║██║██╔██╗ ██║    ║\n");
    printf("║    ██║╚██╔╝██║██║   ██║██║     ██║   ██║██║███╗██║██║██║╚██╗██║    ║\n");
    printf("║    ██║ ╚═╝ ██║╚██████╔╝███████╗██║   ██║╚███╔███╔╝██║██║ ╚████║    ║\n");
    printf("║    ╚═╝     ╚═╝ ╚═════╝ ╚══════╝╚═╝   ╚═╝╚══╝╚══╝ ╚═╝╚═╝   ╚═══╝    ║\n");
    printf("║                                                                    ║\n");
    printf("║                                                                    ║\n");
    printf("║                                                                    ║\n");
}

// Muestra la pantalla de bienvenida y espera una tecla
void menuinicial(int saldo) {
    limpiarPantalla();
    logo();
    printf("╠════════════════════════════════════════════════════════════════════╣\n");
    if (saldo > 0) {
        printf("║             BIENVENIDO DE NUEVO - SALDO: %-6d                    ║\n", saldo);
    } else {
        printf("║                PRESIONA ENTER PARA EMPEZAR A GANAR                 ║\n");
    }
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    getchar();
}

// Muestra las opciones principales, valida la entrada y devuelve la eleccion
int menu_principal_opciones(int saldo) {
    int n;
    limpiarPantalla();
    logo();
    printf("╠════════════════════════════════════════════════════════════════════╣\n");
    printf("                           Saldo actual: %d                           \n", saldo);
    printf("╠════════════════════════════════════════════════════════════════════╣\n");
    printf("║ 1: Ruleta                                                          ║\n");
    printf("║ 2: Blackjack                                                       ║\n");
    printf("║ 3: Slots                                                           ║\n");
    printf("║ 4: Ingresar saldo                                                  ║\n");
    printf("║ 5: Retirar saldo                                                   ║\n");
    printf("║ 0: Salir                                                           ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");

    do {
        printf("Elige una opción: ");
        if(scanf("%d", &n) != 1) {
            while(getchar() != '\n');
            n = -1;
        }
    } while(n < 0 || n > 5);

    return n;
}

// --- GESTION DE DINERO (REQUIERE PUNTERO) ---

// Solicita al usuario una cantidad y la suma al saldo global
void ingresar_saldo(int *saldo) {
    int x;
    printf("¿Cuánto deseas ingresar?: ");
    scanf("%d", &x);
    if(x > 0) {
        *saldo += x; // Modificamos el valor apuntado
    } else {
        printf("Cantidad inválida.\n");
    }
}

// Solicita una cantidad y la resta del saldo si hay fondos suficientes
void retirar_saldo(int *saldo) {
    int x;
    printf("¿Cuánto deseas retirar?: ");
    scanf("%d", &x);
    if(x <= *saldo && x > 0) {
        *saldo -= x; // Modificamos el valor apuntado
    } else {
        printf("Cantidad inválida.\n");
    }
}

// Guarda el progreso y muestra el mensaje de despedida
void salir(int saldo) {
    guardar_saldo(saldo); 
    printf("\nGracias por jugar. Tu saldo final es: %d\n", saldo);
    printf("Progreso guardado en 'saldo.txt'.\n");
}

// --- LOGICA DE LA RULETA ---

// Configura las propiedades (color, fila, docena) de cada numero de la ruleta
void inicializarRuleta(NumeroRuleta ruleta[]) {
    int colores[] = {0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 
                    2, 2, 1, 2, 1, 2, 1, 2, 1, 1,
                    2, 1, 2, 1, 2, 1, 2, 1, 2, 2,
                    1, 2, 1, 2, 1, 2, 1};
    
    for(int i = 0; i <= 36; i++) {
        ruleta[i].numero = i;
        ruleta[i].color = colores[i];
        ruleta[i].fila = (i == 0) ? 0 : (i % 3 == 0 ? 3 : i % 3);
        ruleta[i].docena = (i == 0) ? 0 : (i <= 12 ? 1 : (i <= 24 ? 2 : 3));
        ruleta[i].mitad = (i == 0) ? 0 : (i <= 18 ? 1 : 2);
    }
}

// Dibuja el tablero de apuestas de la ruleta en consola
void dibujarRuleta(NumeroRuleta ruleta[]) {
    printf("\n===========================================\n");
    printf("             RULETA MULTIWIN\n");
    printf("===========================================\n\n");
    
    printf("     0  |   (\033[1;32mVERDE\033[0m)\n");
    printf("    --------------------\n");

    printf("   ");
    for(int i = 3; i <= 36; i += 3)
        printf(" %s%2d\033[0m", ruleta[i].color == 1 ? "\033[1;31m" : "\033[1;30m", i);
    printf("\n");

    printf("   ");
    for(int i = 2; i <= 35; i += 3)
        printf(" %s%2d\033[0m", ruleta[i].color == 1 ? "\033[1;31m" : "\033[1;30m", i);
    printf("\n");

    printf("   ");
    for(int i = 1; i <= 34; i += 3)
        printf(" %s%2d\033[0m", ruleta[i].color == 1 ? "\033[1;31m" : "\033[1;30m", i);
    printf("\n");

    printf("\n    ROJO: \033[1;31m■\033[0m  NEGRO: \033[1;30m■\033[0m  VERDE: \033[1;32m■\033[0m\n");
    printf("===========================================\n");
}

// Imprime las opciones de apuestas disponibles
void mostrarMenuApuestas() {
    printf("\n===========================================\n");
    printf("           MENÚ DE APUESTAS\n");
    printf("===========================================\n");
    printf("1. Número específico (x36) [0 al 36]\n");
    printf("2. Color (x2)              [Rojo / Negro]\n");
    printf("3. Par/Impar (x2)          [Par / Impar]\n");
    printf("4. Fila (x3)               [Fila 1, 2 o 3]\n");
    printf("5. Docena (x3)             [1-12, 13-24, 25-36]\n");
    printf("6. Mitad (x2)              [1-18, 19-36]\n");
    printf("7. Girar ruleta\n");
    printf("8. Ver apuestas\n");
    printf("9. Volver al menú principal\n");
    printf("===========================================\n");
}

// Genera un numero aleatorio entre 0 y 36
int girarRuleta() {
    return rand() % 37;
}

// Comprueba si una apuesta especifica ha ganado segun el resultado
int verificarApuesta(ApuestaRuleta ap, NumeroRuleta r) {
    int resultado = 0;
    switch(ap.tipo) {
        case 1: resultado = (ap.opcion == r.numero); break;
        case 2: resultado = ((ap.opcion == 1 && r.color == 1) || (ap.opcion == 2 && r.color == 2)); break;
        case 3:
            if(r.numero == 0) resultado = 0;
            else resultado = ((ap.opcion == 1 && r.numero % 2 == 0) || (ap.opcion == 2 && r.numero % 2 == 1));
            break;
        case 4: resultado = (ap.opcion == r.fila); break;
        case 5: resultado = (ap.opcion == r.docena); break;
        case 6: resultado = (ap.opcion == r.mitad); break;
    }
    return resultado;
}

// Devuelve la cantidad ganada multiplicando la apuesta por su cuota
int calcularGanancias(ApuestaRuleta ap) {
    int ganancia = 0;
    switch(ap.tipo) {
        case 1: ganancia = ap.cantidad * 36; break;
        case 2:
        case 3:
        case 6: ganancia = ap.cantidad * 2; break;
        case 4:
        case 5: ganancia = ap.cantidad * 3; break;
    }
    return ganancia;
}

// Lista todas las apuestas que el usuario ha realizado en la ronda actual
void mostrarApuestas(ApuestaRuleta apuestas[], int num) {
    limpiarPantalla();
    printf("\n===========================================\n");
    printf("           APUESTAS REALIZADAS\n");
    printf("===========================================\n");

    if(num == 0) {
        printf("No hay apuestas.\n");
    } else {
        for(int i = 0; i < num; i++) {
            char desc[50];
            switch(apuestas[i].tipo) {
                case 1: sprintf(desc, "Numero %d", apuestas[i].opcion); break;
                case 2: sprintf(desc, "Color %s", apuestas[i].opcion == 1 ? "ROJO" : "NEGRO"); break;
                case 3: sprintf(desc, "%s", apuestas[i].opcion == 1 ? "PAR" : "IMPAR"); break;
                case 4: sprintf(desc, "Fila %d", apuestas[i].opcion); break;
                case 5: sprintf(desc, "Docena %d", apuestas[i].opcion); break;
                case 6: sprintf(desc, "Mitad %d", apuestas[i].opcion); break;
                default: sprintf(desc, "?");
            }
            printf("Apuesta %d: %-15s | Cant: %d\n", i+1, desc, apuestas[i].cantidad);
        }
    }
    printf("Presiona enter para volver: ");
    while(getchar() != '\n');
    getchar();
}

// Funcion principal que gestiona el flujo del juego de la ruleta (RECIBE PUNTERO)
void ejecutar_ruleta(int *saldo) {
    NumeroRuleta ruleta[37];
    ApuestaRuleta apuestas[100];
    int numApuestas = 0;
    int opcion, subopcion, cantidad;
    int continuar = 1;

    inicializarRuleta(ruleta);

    while(continuar) {
        limpiarPantalla();
        dibujarRuleta(ruleta);
        mostrarMenuApuestas();

        // USAMOS *saldo PARA VER EL VALOR
        printf("Tu saldo: %d créditos\n", *saldo);
        printf("Selecciona una opción: ");
        if(scanf("%d", &opcion) != 1) {
            while(getchar() != '\n');
            opcion = -1;
        }

        switch(opcion) {
            case 1:
                printf("--> Ingresa el número (0-36): ");
                scanf("%d", &subopcion);
                break;
            case 2:
                printf("--> Elige color (1 = ROJO, 2 = NEGRO): ");
                scanf("%d", &subopcion);
                break;
            case 3:
                printf("--> Elige (1 = PAR, 2 = IMPAR): ");
                scanf("%d", &subopcion);
                break;
            case 4:
                printf("--> Elige fila:\n");
                printf("    1: Fila inferior (1, 4, 7...)\n");
                printf("    2: Fila media    (2, 5, 8...)\n");
                printf("    3: Fila superior (3, 6, 9...)\n");
                printf("    Opción: ");
                scanf("%d", &subopcion);
                break;
            case 5:
                printf("--> Elige docena:\n");
                printf("    1: 1ª Docena (1-12)\n");
                printf("    2: 2ª Docena (13-24)\n");
                printf("    3: 3ª Docena (25-36)\n");
                printf("    Opción: ");
                scanf("%d", &subopcion);
                break;
            case 6:
                printf("--> Elige mitad (1 = 1-18, 2 = 19-36): ");
                scanf("%d", &subopcion);
                break;
            case 7: case 8: case 9: 
                subopcion = 0; 
                break;
            default:
                subopcion = -1;
        }

        if (opcion >= 1 && opcion <= 6) {
             int valido = 1;
             if (opcion == 1 && (subopcion < 0 || subopcion > 36)) valido = 0;
             if ((opcion == 2 || opcion == 3 || opcion == 6) && (subopcion < 1 || subopcion > 2)) valido = 0;
             if ((opcion == 4 || opcion == 5) && (subopcion < 1 || subopcion > 3)) valido = 0;

             if (!valido) {
                 printf("Opción inválida seleccionada.\n");
                 while(getchar() != '\n'); getchar();
             } else {
                 printf("Cantidad a apostar (máximo %d): ", *saldo);
                 scanf("%d", &cantidad);

                 if(cantidad <= 0 || cantidad > *saldo) {
                     printf("Cantidad no válida.\n");
                     while(getchar() != '\n'); getchar();
                 } else {
                     apuestas[numApuestas].tipo = opcion;
                     apuestas[numApuestas].opcion = subopcion;
                     apuestas[numApuestas].cantidad = cantidad;
                     numApuestas++;
                     *saldo -= cantidad; // RESTAMOS DEL PUNTERO
                     printf("Apuesta registrada.\n");
                 }
             }
        } 
        else if (opcion == 7) {
            if(numApuestas == 0) {
                printf("No has realizado apuestas.\n");
                getchar(); getchar();
            } else {
                printf("\nLa ruleta está girando...\n");
                int res = girarRuleta();

                printf("\nRESULTADO: %d ", res);
                if(res == 0) printf("(\033[1;32mVERDE\033[0m)\n");
                else if(ruleta[res].color == 1) printf("(\033[1;31mROJO\033[0m)\n");
                else printf("(\033[1;30mNEGRO\033[0m)\n");

                int gananciasTotales = 0;
                for(int i = 0; i < numApuestas; i++) {
                    if(verificarApuesta(apuestas[i], ruleta[res])) {
                        int g = calcularGanancias(apuestas[i]);
                        gananciasTotales += g;
                        printf("Apuesta %d GANADORA +%d\n", i+1, g);
                    } else {
                        printf("Apuesta %d perdida.\n", i+1);
                    }
                }

                *saldo += gananciasTotales; // SUMAMOS AL PUNTERO
                printf("Ganancias totales: %d\n", gananciasTotales);
                printf("Nuevo saldo: %d\n", *saldo);
                numApuestas = 0;

                if(*saldo <= 0) {
                    printf("\n¡Te has quedado sin créditos!\n");
                    continuar = 0;
                } else {
                    printf("¿Seguir apostando? (1=Sí, 0=No): ");
                    scanf("%d", &continuar);
                }
            }
        }
        else if (opcion == 8) {
            mostrarApuestas(apuestas, numApuestas);
        }
        else if (opcion == 9) {
            continuar = 0;
        }
        else {
            printf("Opción no válida.\n");
            getchar(); getchar();
        }
    }
}

// --- LOGICA DE BLACKJACK ---

// Devuelve la representacion en cadena del valor de una carta (A, J, Q, K o numero)
char *nombre_valor(int carta) {
    int v = carta % 13;
    switch (v) {
        case 0: return "A";
        case 10: return "J";
        case 11: return "Q";
        case 12: return "K";
        default: {
            static char num[3];
            num[0] = '0' + ((v + 1) / 10);
            num[1] = '0' + ((v + 1) % 10);
            num[2] = '\0';
            if (num[0] == '0') {
                num[0] = num[1];
                num[1] = '\0';
            }
            return num;
        }
    }
}

// Devuelve el simbolo del palo de la carta
char *nombre_palo(int carta) {
    int p = carta / 13;
    switch (p) {
        case 0: return "♠";
        case 1: return "♥";
        case 2: return "♦";
        case 3: return "♣";
    }
    return "?";
}

// Calcula el valor numerico de la carta para el Blackjack
int valor_carta(int carta) {
    int r = carta % 13;
    if (r == 0) return 11;
    if (r >= 10) return 10;
    return r + 1;
}

// Dibuja una carta individual en arte ASCII
void imprimir_carta(int carta) {
    char *v = nombre_valor(carta);
    char *p = nombre_palo(carta);
    char v2[3];  
    v2[0] = v[0];
    v2[1] = (v[1] != '\0') ? v[1] : ' ';
    v2[2] = '\0';

    printf("┌─────────┐\n");
    printf("│ %-2s      │\n", v2);
    printf("│         │\n");
    printf("│    %s    │\n", p);
    printf("│         │\n");
    printf("│      %-2s │\n", v2);
    printf("└─────────┘\n");
}

// Dibuja el dorso de una carta oculta
void imprimir_carta_oculta() {
    printf("┌─────────┐\n");
    printf("│░░░░░░░░░│\n");
    printf("│░░░░░░░░░│\n");
    printf("│░░  ?  ░░│\n");
    printf("│░░░░░░░░░│\n");
    printf("│░░░░░░░░░│\n");
    printf("└─────────┘\n");
}

// Recorre e imprime todas las cartas de una mano
void imprimir_mano(int mano[], int n) {
    for (int i = 0; i < n; i++)
        imprimir_carta(mano[i]);
}

// Suma el valor total de una mano ajustando los Ases si es necesario
int valor_mano(int mano[], int n) {
    int total = 0, ases = 0;
    for (int i = 0; i < n; i++) {
        int v = valor_carta(mano[i]);
        total += v;
        if (mano[i] % 13 == 0) ases++;
    }
    while (total > 21 && ases > 0) {
        total -= 10;
        ases--;
    }
    return total;
}

// Mezcla el mazo de cartas aleatoriamente
void barajar(int mazo[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = mazo[i];
        mazo[i] = mazo[j];
        mazo[j] = tmp;
    }
}

// Gestiona una ronda individual de Blackjack (apuesta, reparto y turnos) (RECIBE PUNTERO)
int jugar_ronda_blackjack(int *saldo) {
    limpiarPantalla();
    int apuesta = 0;
    
    printf("=== BLACKJACK ===\n");
    printf("Tienes %d creditos.\n", *saldo);
    
    do {
        printf("Introduce tu apuesta (0 para volver): ");
        if (scanf("%d", &apuesta) != 1) {
            while(getchar() != '\n');
            apuesta = -1;
        }
    } while (apuesta < 0 || apuesta > *saldo);

    if (apuesta == 0) return 0;

    int mazo[52];
    for (int i = 0; i < 52; i++) mazo[i] = i;
    barajar(mazo, 52);

    int jugador[10], dealer[10];
    int pj = 0, pd = 0, tope = 0;

    jugador[pj++] = mazo[tope++];
    dealer[pd++] = mazo[tope++];
    jugador[pj++] = mazo[tope++];
    dealer[pd++] = mazo[tope++];

    int jugando = 1;
    char opcion;

    while (jugando) {
        limpiarPantalla();
        printf("--- TURNO JUGADOR ---\n");
        printf("Apuesta: %d\n\n", apuesta);

        printf("DEALER:\n");
        imprimir_carta(dealer[0]); 
        imprimir_carta_oculta(); 

        printf("\nTU MANO (%d puntos):\n", valor_mano(jugador, pj));
        imprimir_mano(jugador, pj);

        if (valor_mano(jugador, pj) > 21) {
            printf("\n¡TE PASASTE! (Bust)\n");
            jugando = 0;
        } else {
            printf("\n[s] Pedir carta  |  [n] Plantarse: ");
            scanf(" %c", &opcion);
            opcion = tolower(opcion);

            if (opcion == 's') {
                jugador[pj++] = mazo[tope++];
            } else if (opcion == 'n') {
                jugando = 0;
            }
        }
    }

    int v_jugador = valor_mano(jugador, pj);
    if (v_jugador <= 21) {
        printf("\n--- TURNO DEALER ---\n");
        printf("El dealer revela su carta oculta...\n");
        while (valor_mano(dealer, pd) < 17) {
            dealer[pd++] = mazo[tope++];
        }
    }

    int v_dealer = valor_mano(dealer, pd);

    limpiarPantalla();
    printf("--- RESULTADO FINAL ---\n\n");
    printf("DEALER (%d puntos):\n", v_dealer);
    imprimir_mano(dealer, pd);
    printf("\nTU MANO (%d puntos):\n", v_jugador);
    imprimir_mano(jugador, pj);
    printf("\n");

    if (v_jugador > 21) {
        printf("PERDISTE %d creditos. Te pasaste de 21.\n", apuesta);
        *saldo -= apuesta; // MODIFICAR PUNTERO
    } else if (v_dealer > 21) {
        printf("¡GANAS %d creditos! El dealer se paso.\n", apuesta);
        *saldo += apuesta;
    } else if (v_jugador > v_dealer) {
        printf("¡GANAS %d creditos! Tu mano es mejor.\n", apuesta);
        *saldo += apuesta;
    } else if (v_dealer > v_jugador) {
        printf("PERDISTE %d creditos. El dealer tiene mejor mano.\n", apuesta);
        *saldo -= apuesta;
    } else {
        printf("EMPATE (Push). Recuperas tu apuesta.\n");
    }

    printf("\nPresiona ENTER para continuar...");
    while(getchar() != '\n'); 
    getchar();

    return 1;
}

// Bucle principal para el juego de Blackjack
void ejecutar_blackjack(int *saldo) {
    int quiere_jugar = 1;
    while (*saldo > 0 && quiere_jugar) {
        quiere_jugar = jugar_ronda_blackjack(saldo);
    }
    if (*saldo <= 0) printf("\nTe has quedado sin créditos para Blackjack.\n");
}

// --- LOGICA DE SLOTS ---

// Selecciona un icono para los slots basandose en probabilidades predefinidas
int generarIndicePonderado() {
    const int PESOS[] = {60, 20, 10, 8, 2};
    const int PESO_TOTAL = 100;
    int r = rand() % PESO_TOTAL;
    int acc = 0;
    for (int i = 0; i < 5; i++) {
        acc += PESOS[i];
        if (r < acc) return i;
    }
    return 0;
}

// Imprime la maquina tragaperras y los resultados en pantalla
void dibujarInterfazSlots(int saldo, int apuesta_actual, const char* s1, const char* s2, const char* s3, char* msg, int ganancia) {
    limpiarPantalla();
    printf("==============================\n");
    printf("      🎰MultiWin SLOTS🎰\n");
    printf("==============================\n\n");

    printf("SALDO: %d€   APUESTA: %d€\n\n", saldo, apuesta_actual);

    printf("  | %s | %s | %s |\n\n", s1, s2, s3); // ARREGLAR ESTOs

    printf("------------------------------\n");
    printf("%s\n", msg);
    if (ganancia > 0)
        printf("GANANCIA: %d€\n", ganancia);
    printf("------------------------------\n\n");

    printf("     PAREJA        TRIO\n");
    printf("----------------------------\n");
    printf("🍒    x0,1    |     x3     |\n");
    printf("🍋    X0,3    |     X10    |\n");
    printf("🍇    X1      |     X15    |\n");
    printf("🔔    X1,5    |     X20    |\n");
    printf("💎    X3      |     X50    |\n");
    printf("____________________________\n\n");

    printf("[TECLA] GIRAR    [0] CAMBIAR APUESTA/SALIR\n");
}

// Funcion principal que gestiona el juego de Slots (apuestas y tiradas) (RECIBE PUNTERO)
void ejecutar_slots(int *saldo) {
    const char* EMOJIS[] = {"🍒", "🍋", "🍇", "🔔", "💎"};
    const int MULTIPLICADORES[] = {3, 10, 15, 20, 50};
    int apuesta;
    const char* slot1 = "?";
    const char* slot2 = "?";
    const char* slot3 = "?";
    char mensaje_resultado[100] = "PULSA TECLA PARA GIRAR";
    int ultima_ganancia = 0;
    
    int en_juego = 1;

    while (*saldo >= 0 && en_juego) {
        limpiarPantalla();
        printf("SALDO: %d\n", *saldo);

        do {
            printf("APUESTA (0 PARA VOLVER AL MENU): ");
            if (scanf("%d", &apuesta) != 1) {
                while (getchar() != '\n');
                apuesta = -1;
            }
        } while (apuesta < 0 || apuesta > *saldo);

        if (apuesta == 0) {
            en_juego = 0; 
        } else {
            ultima_ganancia = 0;
            sprintf(mensaje_resultado, "PULSA TECLA PARA GIRAR");
            
            int tirando = 1;

            while (*saldo >= apuesta && tirando) {
                // Pasamos el valor desreferenciado (*saldo) para dibujarlo
                dibujarInterfazSlots(*saldo, apuesta, slot1, slot2, slot3, mensaje_resultado, ultima_ganancia);
                char tecla = getch_custom();

                if (tecla == '0') {
                    tirando = 0;
                } else {
                    *saldo -= apuesta; // RESTAR APUESTA

                    int i1 = generarIndicePonderado();
                    int i2 = generarIndicePonderado();
                    int i3 = generarIndicePonderado();

                    slot1 = EMOJIS[i1];
                    slot2 = EMOJIS[i2];
                    slot3 = EMOJIS[i3];

                    if (i1 == i2 && i2 == i3) {
                        ultima_ganancia = apuesta * MULTIPLICADORES[i1];
                        *saldo += ultima_ganancia; // SUMAR PREMIO
                        sprintf(mensaje_resultado, "PREMIO TRIO");
                    } else if (i1 == i2 || i2 == i3 || i1 == i3) {
                        int p;
                        if (i1 == i2) p = i1;
                        else if (i2 == i3) p = i2;
                        else p = i1;

                        switch (p) {
                            case 0: ultima_ganancia = apuesta / 10; break;
                            case 1: ultima_ganancia = apuesta / 3; break;
                            case 2: ultima_ganancia = apuesta; break;
                            case 3: ultima_ganancia = apuesta + apuesta / 2; break;
                            case 4: ultima_ganancia = apuesta * 3; break;
                        }

                        *saldo += ultima_ganancia; // SUMAR PREMIO
                        sprintf(mensaje_resultado, "PREMIO PAREJA");
                    } else {
                        ultima_ganancia = 0;
                        sprintf(mensaje_resultado, "SIN PREMIO");
                    }
                    
                    if(*saldo < apuesta) {
                         dibujarInterfazSlots(*saldo, apuesta, slot1, slot2, slot3, mensaje_resultado, ultima_ganancia);
                         printf("\nSaldo insuficiente para continuar con esta apuesta.\n");
                         getchar(); 
                         tirando = 0;
                    }
                }
            }
        }
    }
}

// --- MAIN (PUNTO DE ENTRADA) ---

int main() {
    int opcion;
    int saldo = 0; // <--- AQUI ESTA LA VARIABLE LOCAL AHORA
    srand(time(NULL));
    
    // Pasamos la direccion (&saldo) para que se modifique
    cargar_saldo(&saldo); 
    
    // Pasamos el valor (saldo) porque solo lee
    menuinicial(saldo);

    do {
        // Pasamos el valor (saldo) para mostrarlo en el menu
        opcion = menu_principal_opciones(saldo);

        switch(opcion) {
            // Pasamos la direccion (&saldo) porque el juego modifica el dinero
            case 1: ejecutar_ruleta(&saldo);    break;
            case 2: ejecutar_blackjack(&saldo); break;
            case 3: ejecutar_slots(&saldo);     break;
            case 4: ingresar_saldo(&saldo);     break;
            case 5: retirar_saldo(&saldo);      break;
            case 0: salir(saldo);               break;
        }
        
        // Guardar progreso despues de cada accion importante (usamos valor)
        guardar_saldo(saldo); 

        if(opcion != 0) {
            printf("\nPresiona ENTER para volver al menú...");
            while(getchar() != '\n');
            getchar();
        }

    } while(opcion != 0);

    return 0;
}