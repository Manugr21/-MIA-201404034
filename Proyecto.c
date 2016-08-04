/**********************************
*	Manuel Fracisco Galván Reyna  *
*	201404034					  *
***********************************/
/*
	Librerias
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/*
	Estructuras
*/
struct Particion
{
	char 	part_status;
	char 	part_type;
	char 	part_fit;
        int     part_start;
        int     part_size;
        char    part_name[16];
};

struct MBR{
	int 	mbr_tamano;
	time_t*	mbr_fecha_creacion;
	int 	mbr_disk_signature;
	struct 	Particion mbr_partition_1;
	struct 	Particion mbr_partition_2;
	struct 	Particion mbr_partition_3;
	struct 	Particion mbr_partition_4;
	int 	part_start;
	int 	part_size;
	char 	part_name[16];
};

struct EBR
{
	char 	part_status;
	char 	part_fit;
        int     part_start;
	int 	part_size;
	int 	part_next;
        int     part_previous;
	char	part_name[16];
};

struct Super_Bloque{
	int	s_filesystem_type;
	int 	s_inodes_count;
	int 	s_blocks_count;
	int 	s_free_blocks_count;
	int 	s_free_inodes_count;
	time_t*	s_mtime;
	time_t*	s_umtime;
	int 	s_mnt_count;
	int 	s_magic;
	int 	s_inode_size;
	int 	s_block_size;
	int 	s_first_ino;
	int 	s_first_blo;
	int 	s_bm_inode_start;
	int 	s_bm_block_start;
	int 	s_inode_start;
	int 	s_block_start;
};

struct Journal{
	int 	Journal_tipo_operacion;		//Indica que tipo de operacion se realizo
	int 	Journal_tipo;			    //0 Archivos | 1 Carpetas
	char	Journal_nombre[10];		    //Nombre del archivo o directorio
	int 	Journal_contenido; 		    //No estoy seguro que es
	time_t*	Journal_fecha;			    //Fecha de la transaccion
	char	Journal_propietario[10];	//Nombre del usuario propietario del archivo o carpeta
	int 	Journal_Permisos;		    //Son los permisos que tenia el archivo o carpeta
};
/*
	Variable Globales
*/
//Cosas genericas pero importantes
char    buffer[1];
int     Multiplicador;
int     convertido;
char    Montador[100][100][100];
char    Normal[400];
char    Linea_Comparable[400];
char    Abecedario[28];

//Banderas o indicadores
int 	TAG;
int     TAG_Script = 0;
int     aux_int;
int     Particiones_N = 0;
int     Particiones_Ini = 0;
int     Particiones_Fin = 0;
int     EBR_Ini = 0;

//Contadores
int     iWhile;

/*
    Declaracion de metodos 
 */
//Genericos
void    Ingresar_Comando();
char*   montador(char name[20], char path[100]);
void    Crear_Directorios_Reales(char path[100]);

//Fase 1
void    Crear_Disco(char size[10], char name[20], char ruta_Disco[100]);
void    Eliminar_Disco(char path[100]);
void    Crear_Particion(char name[20], char size[10], char unit[2], char fit[2], char type[1], char path[100]);
void    Crear_Particion_Logica(FILE *f, int inicio, int limite, char fit[2], char name[100], int size);
void    Eliminar_Particiones(char delet[4], char name[20], char path[100]);

void    Rep_EBR(FILE *fp, FILE *f, struct EBR ebr);

//Fase 2
//Analizador
void    Analizar_Comando(char *linea, char *palabra);
void    Rep_MBR(char path[100], char ruta_Destino[150]);
void    Rep_EBR_Disco(FILE *fp, FILE *f, struct EBR ebr);
void    Rep_Disco_Interno(FILE *fp, FILE *f, struct MBR mbr, int comparador);
void    Rep_Disco(char path[100], char pSalida[150]);

/*
	El MAIN
*/
int main(){
    system("clear");
    printf("************************************************************\n");
    printf("*        Bienvenido al sistema de archivos Ext             *\n");
    printf("************************************************************\n");
    time_t t;
    srand((unsigned) time(&t));
    strcpy(Abecedario,"abcdefghijklmnopqrstuvwxyz");
    printf(">>Para apagar el sistema ingrese el comando \"exit\".\n>> Sistema listo, porfavor introduzca un comando...\n");
    Ingresar_Comando();
    printf("Apagando...\n");
    return 0;
}

/*
	Metodos Genericos
*/
void Ingresar_Comando() {

    char temp[200];
    TAG = 0;
    scanf(" %[^\n]s", Normal);
    if (Normal[strlen(Normal)-1] == '\\')
    {
    	scanf(" %[^\n]s", temp);
        strncpy(Linea_Comparable, Normal, strlen(Normal)-1);
        strcat(Linea_Comparable, temp);
        strcpy(Normal, Linea_Comparable);
        char *Lista = strtok(Linea_Comparable, " ");
        Analizar_Comando(Normal, Lista);
    }
    else
    {
        strcpy(Linea_Comparable, Normal);
        char *Lista = strtok(Normal, " ");
        Analizar_Comando(Linea_Comparable, Lista);
    }

}

char* montador( char name[20], char path[100]){
    char *id = malloc(sizeof(char) * 128);
    int nRuta = 0,  nPart = 1;
    while(nRuta<100){
        if((strcasecmp(Montador[nRuta][0],"")==0) || (strcasecmp(Montador[nRuta][0], path)==0)){
            strcpy(Montador[nRuta][0], path);
            while ((strcasecmp(Montador[nRuta][nPart], "") != 0)&&(strcasecmp(Montador[nRuta][nPart], name) != 0)){
                nPart++;
            }
            strcpy(Montador[nRuta][nPart], name);
            sprintf(id, "vd%c%d", Abecedario[nRuta], nPart);
            return id;
        }else{
            nRuta++;
        }
    }
    return "";
}

void Crear_Directorios_Reales(char path[100]){
    //const char x = '/';
    //char *fin;
    if(path[0] == '/'){
        //fin = strrchr(path, x);
        char terminal_cmd[300];
        strcpy (terminal_cmd,  "mkdir -p '");
        //strncat(terminal_cmd, path, strlen(path)-strlen(fin));
        strncat(terminal_cmd, path, strlen(path)-1);
        strcat(terminal_cmd, "'");
        system(terminal_cmd);
    }
}

/*
    Metodos y Funciones de la Fase 1
*/
void Crear_Disco(char size[10], char name[20], char ruta_Disco[100]){
    struct MBR mbr;
    //Inicializo los datos del disco
    convertido = (int) strtol(size, (char **)NULL, 10);
    mbr.mbr_tamano = convertido * 1024 * Multiplicador;
    time(&mbr.mbr_fecha_creacion);
    mbr.mbr_disk_signature = (rand() % 26);
    mbr.mbr_partition_1.part_status = '0';
    mbr.mbr_partition_2.part_status = '0';
    mbr.mbr_partition_3.part_status = '0';
    mbr.mbr_partition_4.part_status = '0';

    //Inicio de la escritura del disco
    char ruta_temp[120];
    strcpy(ruta_temp, ruta_Disco);
    strcat(ruta_temp, "/");
    strcat(ruta_temp, name);
    FILE *f_disco = fopen (ruta_temp, "wb+");
    for(int iFor=0; iFor < mbr.mbr_tamano; iFor++){
        fwrite(buffer, sizeof(buffer), 1, f_disco);
    }//Fin del for que escribe el disco

    rewind(f_disco);
    fwrite(&mbr, sizeof(mbr), 1, f_disco);
    fclose(f_disco);

    printf("\t>>Disco creado exitosamente.\n");
}

void Eliminar_Disco(char path[100]){
    char SoN[1];
    printf("¿Deseas eliminar el disco? [S/N]\n");
    scanf("%s", SoN);
    if(strcasecmp(SoN,"S")==0){
        if(fopen (path, "r") == NULL){
            printf("\t>El disco indicado no existe. Intentelo nuevamente...\n");
        }else{
            if(remove(path) == 0){
                printf("\t>>Disco eliminado.\n");
            }else{
                printf("\t>Disco en uso, cierre o desmonte el disco y luego intentelo nuevamente.\n");
            }
        }
    }
}

void Crear_Particion(char name[20], char size[10], char unit[2], char fit[2], char type[1], char path[100]){
    struct MBR mbr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL){
        f = fopen (path, "rb+");
    }else{
        printf("\t>¡El archivo no existe!");
    }

    fread (&mbr, sizeof(mbr), 1,f);

    //contar particiones
    TAG = 0;
    aux_int = 0;
    Particiones_N = 0;
    Particiones_Ini = 0;
    Particiones_Fin = 0;
    if(mbr.mbr_partition_1.part_status != 'n'){
        Particiones_N++;
        if(mbr.mbr_partition_1.part_type == 'e'){
            aux_int++;
        }
    }
    if(mbr.mbr_partition_2.part_status != 'n'){
        Particiones_N++;
        if(mbr.mbr_partition_2.part_type == 'e'){
            aux_int++;
        }
    }
    if(mbr.mbr_partition_3.part_status != 'n'){
        Particiones_N++;
        if(mbr.mbr_partition_3.part_type == 'e'){
            aux_int++;
        }
    }
    if(mbr.mbr_partition_4.part_status != 'n'){
        Particiones_N++;
        if(mbr.mbr_partition_4.part_type == 'e'){
            aux_int++;
        }
    }

    if(strcasecmp(unit, "k") == 0){
        Multiplicador = 1024;
    }else if(strcasecmp(unit, "m") == 0){
        Multiplicador = 1024 * 1024;
    }else if(strcasecmp(unit, "") == 0){
        Multiplicador = 1024 * 1024;
    }else if(strcasecmp(unit, "b") == 0){
        Multiplicador = 1;
    }else{
        Multiplicador = 0;
    }
    
    char charFit;
    if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
        charFit = 'w';
    }else if((strcasecmp(fit,"bf") == 0)){
        charFit = 'b';
    }else if((strcasecmp(fit,"ff") == 0)){
        charFit = 'f';
    }else{
        charFit = 'n';
    }

    if(strcasecmp(type,"l") != 0){

        if(Multiplicador != 0){
            if(Particiones_N == 4){
                printf("\t>Solo se pueden crear 4 particiones.");
            }else if(Particiones_N == 0){
                if(mbr.mbr_tamano >= strtol(size, (char**)NULL, 10)*Multiplicador){
                    mbr.mbr_partition_1.part_status = 'd';
                    mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                    mbr.mbr_partition_1.part_start = sizeof(mbr);
                    strcpy(mbr.mbr_partition_1.part_name, name);

                    if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_1.part_fit = 'wf';
                    }else if ((strcasecmp(fit,"bf") == 0)){
                        mbr.mbr_partition_1.part_fit = 'bf';
                    }else{
                        mbr.mbr_partition_1.part_fit = 'ff';
                    }

                    if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                        mbr.mbr_partition_1.part_type = 'p';
                    }else if(strcasecmp(type,"e") == 0){
                        if(aux_int == 0){
                            mbr.mbr_partition_1.part_type = 'e';
                            
                            struct EBR ebr;
                            ebr.part_next= -1;
                            ebr.part_start = sizeof(mbr) + sizeof(ebr);
                            ebr.part_status = 'n';
                            fseek(f,sizeof(mbr),SEEK_SET);
                            fwrite(&ebr, sizeof(ebr), 1, f);
                        }else{
                            TAG = 1;
                            printf("\t>¡Solo puede haber una particion extendida!\n");
                        }
                    }
                    
                    if(TAG == 0){
                        fseek(f, 0, SEEK_SET);
                        fwrite(&mbr, sizeof(mbr), 1, f);
                    }else{
                        printf("\t>No se pudieron guardar los cambios\n");
                    }
                }else{
                    printf("\t>No hay espacio suficiente en el disco.\n");
                }
            }else if(Particiones_N == 1){

                if(mbr.mbr_partition_1.part_status != 'n'){
                    if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                        Particiones_Ini = mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size;
                        Particiones_Fin = mbr.mbr_tamano;
                    }else if(mbr.mbr_partition_1.part_start > sizeof(mbr)){
                        Particiones_Ini = sizeof(mbr);
                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                            Particiones_Ini = mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                    }else{
                        printf("\t>Se leyo mal el archivo.\n");
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_2.part_status = 'd';
                        mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_2.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_2.part_name, name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'wf';
                        }else if ((strcasecmp(fit,"bf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_2.part_type = 'p';
                        }else if(strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_2.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }
                }else if(mbr.mbr_partition_2.part_status != 'n'){
                    if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                        Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                        Particiones_Fin = mbr.mbr_tamano;
                    }else if(mbr.mbr_partition_2.part_start > sizeof(mbr)){
                        Particiones_Ini = sizeof(mbr);
                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                            Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                    }else{
                        printf("\t>Se leyo mal el archivo.\n");
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_1.part_status = 'a';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_1.part_name, name);
                        
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'wf';
                        }else if((strcasecmp(fit,"bf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_1.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_1.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios\n");
                        }
                    }
                }else if(mbr.mbr_partition_3.part_status != 'n'){
                    if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                        Particiones_Ini = mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size;
                        Particiones_Fin = mbr.mbr_tamano;
                    }else if(mbr.mbr_partition_3.part_start > sizeof(mbr)){
                        Particiones_Ini = sizeof(mbr);
                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                            Particiones_Ini = mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                    }else{
                        printf("\t>Se leyo mal el archivo.\n");
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_1.part_status = 'a';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_1.part_name, name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'wf';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_1.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_1.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }
                }else if(mbr.mbr_partition_4.part_status != 'n'){
                    if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                        Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                        Particiones_Fin = mbr.mbr_tamano;
                    }else if(mbr.mbr_partition_2.part_start > sizeof(mbr)){
                        Particiones_Ini = sizeof(mbr);
                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                            Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                    }else{
                        printf("\t>Se leyo mal el archivo.\n");
                    }
                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_1.part_status = 'a';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_1.part_name, name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'wf';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_1.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_1.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }
                }else{
                    printf("\t>Se leyo mal el archivo.\n");
                }
            }else if(Particiones_N == 2){
                if(mbr.mbr_partition_1.part_status == 'n'){
                    if((mbr.mbr_partition_2.part_status != 'n')&&(mbr.mbr_partition_3.part_status != 'n')){
                        if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start)){
                            if((mbr.mbr_partition_2.part_start == sizeof(mbr))){
                                if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_3.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_3.part_start){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                            }else{
                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size == mbr.mbr_partition_2.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_3.part_start){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_3.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                            }else{
                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_2.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n')){
                        if(mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start){
                            if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_2.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_3.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n')){
                        if(mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start){
                            if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_3.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_3.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }//ya termino



                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_1.part_status = 'a';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_1.part_name, name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                            mbr.mbr_partition_1.part_fit = 'wf';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_1.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_1.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }


                }else if(mbr.mbr_partition_2.part_status == 'n'){
                    if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n')){
                        if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_4.part_start){
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_4.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n')){
                        if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_1.part_start){
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n')){
                        if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start){
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }


                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_2.part_status = 'a';
                        mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_2.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_2.part_name,name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_2.part_fit = 'w';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_2.part_fit = 'b';
                        }else{
                            mbr.mbr_partition_2.part_fit = 'f';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_2.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_2.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }
                }else if(mbr.mbr_partition_3.part_status == 'n'){
                    if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n')){
                        if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start){
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_2.part_status != 'n')){
                        if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start){
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_2.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_2.part_start){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                            }else{
                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_4.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_2.part_start){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                            }else{
                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }else if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_2.part_status != 'n')){
                        if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start){
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_2.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_2.part_start){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                            }else{
                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_1.part_start){
                                    if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_2.part_start){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay suficiente espacio para crear la particion.\n");
                                    }
                                }
                            }else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador){
                                Particiones_Ini = sizeof(mbr);
                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                            }else{
                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador)){
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador ){
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                    printf("\t>No hay suficiente espacio para crear la particion.\n");
                                }
                            }
                        }
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("error, no hay espacio suficiente, %d,%d",Particiones_Fin, Particiones_Ini);
                    }else{
                        mbr.mbr_partition_3.part_status = 'd';
                        mbr.mbr_partition_3.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_3.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_3.part_name,name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_3.part_fit = 'w';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_3.part_fit = 'b';
                        }else{
                            mbr.mbr_partition_3.part_fit = 'f';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_3.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_3.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_3.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }


                }else{
                    printf(">Lo siento, no hice algo bien.<\n");
                }
            }else if(Particiones_N == 3){
                if(mbr.mbr_partition_1.part_status == 'n'){
                    if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start)){
                        if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start)){
                            if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start)){
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }

                                }
                            }else{
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }

                                }
                            }
                        }else{
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }else{
                        if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start)){
                            if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start)){
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }

                                }
                            }else{
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }

                            }
                        }
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_1.part_status = 'a';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_1.part_name, name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_1.part_fit = 'wf';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_1.part_fit = 'bf';
                        }else{
                            mbr.mbr_partition_1.part_fit = 'ff';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_1.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_1.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }

                }else if(mbr.mbr_partition_2.part_status == 'n'){
                    if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start)){
                        if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_1.part_start)){
                            if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start)){
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }

                                }
                            }else{
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }else{
                        if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start)){
                            if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_1.part_start)){
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }else{
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_2.part_status = 'a';
                        mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_2.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_2.part_name,name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_2.part_fit = 'w';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_2.part_fit = 'b';
                        }else{
                            mbr.mbr_partition_2.part_fit = 'f';
                        }
                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_2.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_2.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }

                }else if(mbr.mbr_partition_3.part_status == 'n'){
                    if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start)){
                        if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start)){
                            if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start)){
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }else{
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }else{
                        if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start)){
                            if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start)){
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }

                                }
                            }else{
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_3.part_status = 'a';
                        mbr.mbr_partition_3.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_3.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_3.part_name,name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_3.part_fit = 'w';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_3.part_fit = 'b';
                        }else{
                            mbr.mbr_partition_3.part_fit = 'f';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_3.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_3.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_3.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }

                }else if(mbr.mbr_partition_4.part_status == 'n'){
                    if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start)){
                        if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_3.part_start)){
                            if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start)){
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }else{
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }else{
                        if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start)){
                            if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_3.part_start)){
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }else{
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr)){
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else{
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }else{
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }else{
                                                printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                            }
                                        }
                                    }else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mbr.mbr_partition_3.part_start == sizeof(mbr)){
                                if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }else{
                                if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size)){
                                    if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }else{
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }else{
                                            printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                        }
                                    }
                                }else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){// fragmentacion entre 2 y 3
                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)){
                                    if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }else{
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador){
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }else{
                                        printf("\t>No hay espacio suficiente para crear la particion deseada.\n");
                                    }
                                }
                            }
                        }
                    }

                    if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador){
                        printf("\t>No hay espacio suficiente.\n");
                    }else{
                        mbr.mbr_partition_4.part_status = 'a';
                        mbr.mbr_partition_4.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_4.part_start = Particiones_Ini;
                        strcpy(mbr.mbr_partition_4.part_name,name);
                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0)){
                        mbr.mbr_partition_4.part_fit = 'w';
                        }else if(strcasecmp(fit,"bf") == 0){
                            mbr.mbr_partition_4.part_fit = 'b';
                        }else{
                            mbr.mbr_partition_4.part_fit = 'f';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0)){
                            mbr.mbr_partition_4.part_type = 'p';
                        }else if (strcasecmp(type,"e") == 0){
                            if(aux_int == 0){
                                mbr.mbr_partition_4.part_type = 'e';
                                
                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = mbr.mbr_partition_4.part_start+sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }else{
                                TAG = 1;
                                printf("\t>Solo puede haber una particion extendida.\n");
                            }
                        }

                        if(TAG == 0){
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                        }else{
                            printf("\t>No se pudieron guardar los cambios.\n");
                        }
                    }
                }
            }
        }else{
            printf("\t>Unit invalido, tipos disponibles: P, E y L.\n");
        }
    }else{
        
        if(aux_int == 0){
            printf("\t>No existe una particion logica en el disco.\n");
        }else{
            if(mbr.mbr_partition_1.part_type == 'e'){

                Crear_Particion_Logica(f, mbr.mbr_partition_1.part_start,mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

            }else if(mbr.mbr_partition_2.part_type == 'e'){

                Crear_Particion_Logica(f, mbr.mbr_partition_2.part_start,mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

            }else if(mbr.mbr_partition_3.part_type == 'e'){

                Crear_Particion_Logica(f, mbr.mbr_partition_3.part_start,mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

            }else if(mbr.mbr_partition_4.part_type == 'e'){

                Crear_Particion_Logica(f, mbr.mbr_partition_4.part_start,mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

            }else{
                printf("Lo siento hice algo mal.\n");
            }
        }
    }
    fclose(f);
}

void Crear_Particion_Logica(FILE *f, int inicio, int limite, char fit[2], char name[100], int size){
    struct EBR ebr;
    struct EBR ebrNuevo;
    fseek(f, inicio, SEEK_SET);
    fread(&ebr, sizeof(ebr), 1, f);

    if(ebr.part_next == -1){
        if(inicio + sizeof(ebr) + size <= limite){
            ebr.part_fit = fit;
            strcpy(ebr.part_name, name);
            ebr.part_next = inicio + sizeof(ebr) + size;
            ebr.part_size = size;
            ebr.part_start = inicio;
            ebr.part_status = 'd';

            ebrNuevo.part_next= -1;
            ebrNuevo.part_start = ebr.part_next + sizeof(ebr);
            ebrNuevo.part_previous= inicio;
            ebrNuevo.part_status = 'n';
            fseek(f, inicio, SEEK_SET);
            fwrite(&ebr, sizeof(ebr), 1, f);
            fseek(f, ebr.part_next, SEEK_SET);
            fwrite(&ebrNuevo, sizeof(ebr), 1, f);
        }else{
            printf("\t>No hay espacio suficiente en la particion extendida.\n");
        }

    }else{
        if(ebr.part_next == ebr.part_size+ebr.part_start){
            Crear_Particion_Logica(f, ebr.part_next, limite, fit, name, size);
        }else{
            if(ebr.part_next -(ebr.part_size+ebr.part_start) >= size){
                ebrNuevo.part_next= ebr.part_next;
                ebrNuevo.part_start = ebr.part_start + sizeof(ebr) + ebr.part_size + sizeof(ebr);
                ebrNuevo.part_previous= inicio;
                ebrNuevo.part_status = 'd';

                ebr.part_next = ebr.part_start + sizeof(ebr) + ebr.part_size;
                fseek(f, inicio, SEEK_SET);
                fwrite(&ebr, sizeof(ebr), 1, f);
                fseek(f, ebr.part_next, SEEK_SET);
                fwrite(&ebrNuevo, sizeof(ebr), 1, f);
            }else{
                Crear_Particion_Logica(f, ebr.part_next, limite, fit, name, size);
            }
        }
    }
}

void Eliminar_Particiones(char delet[4], char name[20], char path[250]){
    char SoN[1];
    printf("¿Desea borrar la particion? [S/N]\n");
    scanf("%s", SoN);
    if(strcasecmp(SoN,"S")==0){
        struct MBR mbr;
        FILE *f = fopen (path, "rb+");
        fseek(f,0,SEEK_SET);
        fread (&mbr, sizeof(mbr), 1, f);

        //Comparo con la particion 1
        if(strcasecmp(mbr.mbr_partition_1.part_name, name) == 0){
            if(strcasecmp(delet,"fast") == 0){
                mbr.mbr_partition_1.part_status = 'n';
                mbr.mbr_partition_1.part_start = 0;
                mbr.mbr_partition_1.part_size = 0;
                mbr.mbr_partition_1.part_type = 'n';
            }else if(strcasecmp(delet,"full") == 0){
                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                for(int iFor  = 0; iFor  < mbr.mbr_partition_1.part_size;  iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_1.part_status = 'n';
                mbr.mbr_partition_1.part_start = 0;
                mbr.mbr_partition_1.part_size = 0;
                mbr.mbr_partition_1.part_type = 'n';
            }
            printf("Particion %s eliminada.\n", name);
        //Comparo con la particion 2
        }else if(strcasecmp(mbr.mbr_partition_2.part_name,  name) == 0){
            if(strcasecmp(delet,"fast") == 0){
                mbr.mbr_partition_2.part_status = 'n';
                mbr.mbr_partition_2.part_start = 0;
                mbr.mbr_partition_2.part_size = 0;
                mbr.mbr_partition_2.part_type = 'n';
            }else if(strcasecmp(delet,"full") == 0){
                fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_2.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_2.part_status = 'n';
                mbr.mbr_partition_2.part_start = 0;
                mbr.mbr_partition_2.part_size = 0;
                mbr.mbr_partition_2.part_type = 'n';
            }
            printf("Particion %s eliminada.\n", name);
        //Comparo con la particion 3
        }else if(strcasecmp(mbr.mbr_partition_3.part_name, name) == 0){
            if(strcasecmp(delet,"fast") == 0){
                mbr.mbr_partition_3.part_status = 'n';
                mbr.mbr_partition_3.part_start = 0;
                mbr.mbr_partition_3.part_size = 0;
                mbr.mbr_partition_3.part_type = '0';
            }else if(strcasecmp(delet,"full") == 0){
                fseek(f, mbr.mbr_partition_3.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_3.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_3.part_status = 'n';
                mbr.mbr_partition_3.part_start = 0;
                mbr.mbr_partition_3.part_size = 0;
                mbr.mbr_partition_3.part_type = 'n';
            }
            printf("Particion %s eliminada.\n", name);
        //Comparo con la particion 4
        }else if(strcasecmp(mbr.mbr_partition_4.part_name, name) == 0){
            if(strcasecmp(delet,"fast") != 0){
                mbr.mbr_partition_4.part_status = 'n';
                mbr.mbr_partition_4.part_start = 0;
                mbr.mbr_partition_4.part_size = 0;
                mbr.mbr_partition_4.part_type = 'n';
            }else if(strcasecmp(delet,"full") == 0){
                fseek(f, mbr.mbr_partition_4.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_4.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_4.part_status = 'n';
                mbr.mbr_partition_4.part_start = 0;
                mbr.mbr_partition_4.part_size = 0;
                mbr.mbr_partition_4.part_type = 'n';
            }
            printf("Particion %s eliminada.\n", name);
        }else{
            struct EBR ebr;
            struct EBR ebr_prev;
            struct EBR ebr_next;
            ebr_prev.part_next = -1;

            if(mbr.mbr_partition_1.part_type == 'e'){
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_1.part_start;
            }else if(mbr.mbr_partition_2.part_type == 'e'){
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_2.part_start;
            }else if(mbr.mbr_partition_3.part_type == 'e'){
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_3.part_start;
            }else if(mbr.mbr_partition_4.part_type == 'e'){
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_4.part_start;
            }else{
                printf("No se encontro la particion logicas.\n");
            }
            TAG = 0;

            while(ebr.part_status != '0'){
                TAG = 1;
                if(strcasecmp(ebr.part_name, name) == 0){
                    if(ebr_prev.part_next != -1){

                        if(strcasecmp(delet,"fast") != 0){
                            ebr.part_start = 0;
                            ebr.part_status = 'n';
                            fseek(f, ebr.part_next, SEEK_SET);
                            fread (&ebr_next, sizeof(ebr_next), 1, f);
                            if(ebr_next.part_next == -1){
                                ebr.part_next = -1;
                                fseek(f, ebr_prev.part_next, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                                printf("\t>>Particion logica %s eliminada.\n", name);
                            }else{
                                ebr_prev.part_next = ebr.part_next;
                                ebr_next.part_previous = ebr.part_previous;
                                fseek(f, ebr.part_previous, SEEK_SET);
                                fwrite(&ebr_prev, sizeof(ebr_prev), 1, f);
                                fseek(f, ebr.part_next,SEEK_SET);
                                fwrite(&ebr_next, sizeof(ebr_next), 1, f);
                                printf("\t>>Particion %s eliminada.\n", name);
                            }
                        //Fin del borrado rapido
                        }else if(strcasecmp(delet,"full") != 0){
                            ebr.part_start = 0;
                            ebr.part_status = 'n';
                            fseek(f, ebr.part_next, SEEK_SET);
                            fread (&ebr_next, sizeof(ebr_next), 1, f);
                            if(ebr_next.part_next == -1){
                                ebr.part_next = -1;
                                fseek(f, ebr_prev.part_next, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                                fseek(f, ebr.part_start, SEEK_SET);
                                for(int iFor = 0; iFor < ebr.part_size; iFor++){
                                    fwrite (buffer, sizeof(buffer), 1, f);
                                }
                                printf("\t>>Particion %s eliminada.\n", name);
                            }else{
                                int posicionBorrado = ebr_prev.part_next;
                                ebr_prev.part_next = ebr.part_next;
                                ebr_next.part_previous = ebr.part_previous;
                                fseek(f, ebr.part_previous, SEEK_SET);
                                fwrite(&ebr_prev, sizeof(ebr_prev),1, f);
                                fseek(f, ebr.part_next, SEEK_SET);
                                fwrite(&ebr_next, sizeof(ebr_next),1, f);
                                fseek(f, posicionBorrado, SEEK_SET);
                                for(int iFor = 0; iFor < ebr.part_size+sizeof(ebr); iFor++){
                                    fwrite (buffer, sizeof(buffer), 1, f);
                                }
                                printf("\t>>Particion %s eliminada.\n", name);
                            }
                        }//Fin del borrado completo
                    } //Fin del if que compara el nombre con la particion actual
                }else{
                    ebr_prev = ebr;
                    fseek(f, ebr.part_next, SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1, f);
                }//Fin del cambio de particion
            }//Fin del ciclo de busqueda
        }//Fin del borrado de una particion
        fseek(f, 0, SEEK_SET);
        fwrite(&mbr, sizeof(mbr), 1, f);
        fclose(f);
    }//Fin del borrado
}

void Montar(char name[20], char path[250]){
    char id[10];
    struct MBR mbr;
    FILE *f_disco;
    if((fopen(path, "rb+")) == NULL){
        printf("\t>No se encontro el archivo en la ruta especificada.\n");
    }else{
        f_disco = fopen(path, "rb+");
        fseek(f_disco,0,SEEK_SET);
        fread (&mbr, sizeof(mbr), 1,f_disco);

        if(strcasecmp(mbr.mbr_partition_1.part_name, name) == 0){
            mbr.mbr_partition_1.part_status = 'a';
            strcpy(id, montador( name, path));
            printf("\t>>La particion %s se monto exitosamento con el id: %s\n", name, id);
        }else if(strcasecmp(mbr.mbr_partition_2.part_name, name) == 0){
            mbr.mbr_partition_2.part_status = 'a';
            strcpy(id, montador( name, path));
            printf("\t>>La particion %s se monto exitosamento con el id: %s\n", name, id);
        }else if(strcasecmp(mbr.mbr_partition_3.part_name, name) == 0){
            mbr.mbr_partition_3.part_status = 'a';
            strcpy(id, montador( name, path));
            printf("\t>>La particion %s se monto exitosamento con el id: %s\n", name, id);
        }else if(strcasecmp(mbr.mbr_partition_4.part_name, name) == 0){
            mbr.mbr_partition_4.part_status = 'a';
            strcpy(id, montador( name, path));
            printf("\t>>La particion %s se monto exitosamento con el id: %s\n", name, id);
        }else{
            struct EBR ebr;
            TAG = 0;

            if(mbr.mbr_partition_1.part_type == 'e'){
                fseek(f_disco, mbr.mbr_partition_1.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }else if(mbr.mbr_partition_2.part_type == 'e'){
                fseek(f_disco, mbr.mbr_partition_2.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }else if(mbr.mbr_partition_3.part_type == 'e'){
                fseek(f_disco, mbr.mbr_partition_3.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }else if(mbr.mbr_partition_4.part_type == 'e'){
                fseek(f_disco, mbr.mbr_partition_4.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }else{
                printf("\t>Solo se pueden montar particiones existentes, intentelo nuevamente :(\n");
                TAG =1;
            }
            while((ebr.part_status != '0')&&(TAG == 0)){
                if(strcasecmp(ebr.part_name, name) == 0){
                    ebr.part_status = 'a';
                    fseek(f_disco, ebr.part_start-sizeof(ebr), SEEK_SET);
                    fwrite(&ebr, sizeof(ebr), 1, f_disco);
                    strcpy(id, montador( name, path));
                    printf("\t>>La particion %s se monto exitosamento con el id: %s\n", name, id);
                    TAG = 1;
                }else{
                    fseek(f_disco,ebr.part_next,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f_disco);
                }
            }
        }
        fseek(f_disco, 0, SEEK_SET);
        fwrite(&mbr, sizeof(mbr), 1, f_disco);
        fclose(f_disco);
    }
}

void Desmontar(char id[4]){
        struct MBR mbr;
        char name[20], path[250];
        int Algo = 0;
        char Letra_Disco = id[2];
        char nP[3];
        FILE *f;
        sprintf(nP, "%c%c", id[3], id[4]);
        aux_int = 0;
        while(Letra_Disco != Abecedario[aux_int]){
            aux_int++;
        }
        strcpy(name, Montador[aux_int][atoi(nP)]);
        strcpy(path, Montador[aux_int][0]);
        strcpy(Montador[aux_int][atoi(nP)], "");
        TAG = 0;
        while(Algo < 100){
            if(strcasecmp(Montador[aux_int][Algo], "") != 0){
                TAG = 1;
            }
            Algo++;
        }
        if(TAG == 0){
            strcpy(Montador[aux_int][0], "");
        }

        if((fopen (path, "rb+")) == NULL){
            printf("\t>No se encontro el archivo en la ruta indicada\n");
        }else{
            f = fopen (path, "rb+");
            fseek(f,0,SEEK_SET);
            fread (&mbr, sizeof(mbr), 1,f);
            if(strcasecmp(mbr.mbr_partition_1.part_name,name) == 0){
                mbr.mbr_partition_1.part_status = 'd';
                printf("\t>>Particion %s desmontada.\n",name);
            }else if(strcasecmp(mbr.mbr_partition_2.part_name,name) == 0){
                mbr.mbr_partition_2.part_status = 'd';
                printf("\t>>Particion %s desmontada.\n",name);
            }else if(strcasecmp(mbr.mbr_partition_3.part_name,name) == 0){
                mbr.mbr_partition_3.part_status = 'd';
                printf("\t>>Particion %s desmontada.\n",name);
            }else if(strcasecmp(mbr.mbr_partition_4.part_name,name) == 0){
                mbr.mbr_partition_4.part_status = 'd';
                printf("\t>>Particion %s desmontada.\n",name);
            }else{
                struct EBR ebr;
                TAG = 0;
                //con esto ubico en donde esta el primer ebr a leer
                if(mbr.mbr_partition_1.part_type == 'e'){
                    fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f);
                }else if(mbr.mbr_partition_2.part_type == 'e'){
                    fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f);
                }else if(mbr.mbr_partition_3.part_type == 'e'){
                    fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f);
                }else if(mbr.mbr_partition_4.part_type == 'e'){
                    fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f);
                }else{
                    printf("\t>Solo se pueden desmontar particiones montadas... lo se, que mala indicacion :P\n");
                    TAG =1;
                }
                while((ebr.part_status != '0')&&(TAG == 0)){
                    if(strcasecmp(ebr.part_name,name) == 0){
                        ebr.part_status = 'd';
                        fseek(f,ebr.part_start-sizeof(ebr),SEEK_SET);
                        fwrite(&ebr,sizeof(ebr),1,f);
                        TAG = 1;
                        printf("\t>>Particion %s desmontada.\n",name);
                    }else{
                        fseek(f,ebr.part_next,SEEK_SET);
                        fread (&ebr, sizeof(ebr), 1,f);
                    }
                }
            }
            fseek(f, 0, SEEK_SET);
            fwrite(&mbr, sizeof(mbr), 1, f);
            fclose(f);
        }
}

/*
    Reportes
 */
void Rep_EBR(FILE *fp, FILE *f, struct EBR ebr){
    if(ebr.part_status != 'n'){
        fprintf ( fp, "%s [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n", ebr.part_name);
        fprintf ( fp, "<tr><td><b>%s</b></td><td>Valor</td></tr>\n",ebr.part_name);
        fprintf ( fp, "<tr><td><b>part_status</b></td><td><b>%c</b></td></tr>\n",ebr.part_status);
        fprintf ( fp, "<tr><td><b>part_fit</b></td><td><b>%c</b></td></tr>\n",ebr.part_fit);
        fprintf ( fp, "<tr><td><b>part_start</b></td><td><b>%d</b></td></tr>\n",ebr.part_start);
        fprintf ( fp, "<tr><td><b>part_size</b></td><td><b>%d</b></td></tr>\n",ebr.part_size);
        fprintf ( fp, "<tr><td><b>part_next</b></td><td><b>%d</b></td></tr>\n",ebr.part_next);
        fprintf ( fp, "<tr><td><b>part_name</b></td><td><b>%d</b></td></tr>\n",ebr.part_name);
        fprintf ( fp, "</table>>];\n");
        if(ebr.part_next != -1){
            fseek(f,ebr.part_next,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            Rep_EBR(fp, f, ebr);
        }
    }
}

void Rep_MBR(char path[100], char ruta_Destino[150]){
    struct MBR mbr;
    struct EBR ebr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL){

        f = fopen (path, "rb+");
        FILE *fp = fopen ( "/home/manugr/Desktop/reporteMbr.dot", "w" );
        fread (&mbr, sizeof(mbr), 1,f);
        fprintf ( fp, "digraph \"mbr\" {\n");
        fprintf ( fp, "node [shape=plaintext]\n");
        fprintf ( fp, "nodoMbr [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        fprintf ( fp, "<tr><td><b>%s</b></td></tr>\n", path);
        fprintf ( fp, "<tr><td><b>mbr_tamano(bytes)</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_tamano);
        fprintf ( fp, "<tr><td><b>mbr_fecha_creacion</b></td><td><b>%s</b></td></tr>\n",mbr.mbr_fecha_creacion);
        fprintf ( fp, "<tr><td><b>mbr_disk_signature</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_disk_signature);
        TAG = 0;

        if(mbr.mbr_partition_1.part_status != 'n'){
        fprintf ( fp, "<tr><td><b>part_status_1</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_1.part_status);
        fprintf ( fp, "<tr><td><b>part_type_1</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_1.part_type);
        fprintf ( fp, "<tr><td><b>part_fit_1</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_1.part_fit);
        fprintf ( fp, "<tr><td><b>part_start_1</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_1.part_start);
        fprintf ( fp, "<tr><td><b>part_size_1</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_1.part_size);
        fprintf ( fp, "<tr><td><b>part_name_1</b></td><td><b>%s</b></td></tr>\n",mbr.mbr_partition_1.part_name);
        if(mbr.mbr_partition_1.part_type == 'e'){
            fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            TAG = 10;
        }
        }
        if(mbr.mbr_partition_2.part_status != 'n'){
        fprintf ( fp, "<tr><td><b>part_status_2</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_2.part_status);
        fprintf ( fp, "<tr><td><b>part_type_2</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_2.part_type);
        fprintf ( fp, "<tr><td><b>part_fit_2</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_2.part_fit);
        fprintf ( fp, "<tr><td><b>part_start_2</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_2.part_start);
        fprintf ( fp, "<tr><td><b>part_size_2</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_2.part_size);
        fprintf ( fp, "<tr><td><b>part_name_2</b></td><td><b>%s</b></td></tr>\n",mbr.mbr_partition_2.part_name);
        if(mbr.mbr_partition_2.part_type == 'e'){
            fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            TAG = 10;
        }
        }
        if(mbr.mbr_partition_3.part_status != 'n'){
        fprintf ( fp, "<tr><td><b>part_status_3</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_3.part_status);
        fprintf ( fp, "<tr><td><b>part_type_3</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_3.part_type);
        fprintf ( fp, "<tr><td><b>part_fit_3</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_3.part_fit);
        fprintf ( fp, "<tr><td><b>part_start_3</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_3.part_start);
        fprintf ( fp, "<tr><td><b>part_size_3</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_3.part_size);
        fprintf ( fp, "<tr><td><b>part_name_3</b></td><td><b>%s</b></td></tr>\n",mbr.mbr_partition_3.part_name);
        if(mbr.mbr_partition_3.part_type == 'e'){
            fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            TAG = 10;
        }
        }
        if(mbr.mbr_partition_4.part_status != 'n'){
        fprintf ( fp, "<tr><td><b>part_status_4</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_4.part_status);
        fprintf ( fp, "<tr><td><b>part_type_4</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_4.part_type);
        fprintf ( fp, "<tr><td><b>part_fit_4</b></td><td><b>%c</b></td></tr>\n",mbr.mbr_partition_4.part_fit);
        fprintf ( fp, "<tr><td><b>part_start_4</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_4.part_start);
        fprintf ( fp, "<tr><td><b>part_size_4</b></td><td><b>%d</b></td></tr>\n",mbr.mbr_partition_4.part_size);
        fprintf ( fp, "<tr><td><b>part_name_4</b></td><td><b>%s</b></td></tr>\n",mbr.mbr_partition_4.part_name);
        if(mbr.mbr_partition_4.part_type == 'e'){
            fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            TAG = 10;
        }
        }
        fprintf ( fp, "</table>>];\n");

        if(TAG == 10){
        ebrInicial = ebr.part_next;
        Rep_EBR(fp,f,ebr);
        }

        fprintf ( fp, "}\n");
        fclose(f);
        fclose(fp);
        char temp[200];
        sprintf(temp, "dot -Tpng \"/home/manugr/Desktop/reporteMbr.dot\" -o \"%s\"", ruta_Destino);
        system(temp);
        strcpy(temp,"");
        sprintf(temp, "run-mailcap \"%s\" &", ruta_Destino);
        system(temp);
    }else{
        printf("\t>El archivo no existe!\n");
    }

}

void Rep_EBR_Disco(FILE *fp, FILE *f, struct EBR ebr){
    if(ebr.part_status != 'n'){
        fprintf ( fp, "extendida%s [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n",ebr.part_name);
        fprintf ( fp, "<tr><td><b>EBR</b></td><td><b>logica(%s)</b></td></tr>\n",ebr.part_name);
        fprintf ( fp, "</table>>];\n");
        if(ebr.part_next != -1){
            fseek(f,ebr.part_next,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            Rep_EBR_Disco(fp, f, ebr);
        }
    }
}

void Rep_Disco_Interno(FILE *fp, FILE *f, struct MBR mbr, int comparador){
    TAG = 0;
    struct EBR ebr;
    if(mbr.mbr_partition_1.part_status != 'n'){
        if(mbr.mbr_partition_1.part_start == comparador){
            comparador += mbr.mbr_partition_1.part_size;
            TAG = 10;
            if(mbr.mbr_partition_1.part_type == 'e'){
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                ebrInicial = ebr.part_next;
                if(ebr.part_status == 'n'){
                    fprintf ( fp, "extendida [label=\"Extendida(vacia)\"];");
                }else{
                    fprintf ( fp, "subgraph sub { \n");
                    Rep_EBR_Disco(fp,f,ebr);
                    fprintf ( fp, "}");
                }
            }else{
                fprintf ( fp, "primaria%s [label=\"primaria(%s)\"];", mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_name);
            }
        }
    }
    if((mbr.mbr_partition_2.part_status != 'n')&&(TAG != 10)){
        if(mbr.mbr_partition_2.part_start == comparador){
            comparador += mbr.mbr_partition_2.part_size;
            TAG = 10;
            if(mbr.mbr_partition_2.part_type == 'e'){
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                ebrInicial = ebr.part_next;
                if(ebr.part_status == 'n'){
                    fprintf ( fp, "extendida [label=\"Extendida(vacia)\"];");
                }else{
                    fprintf ( fp, "subgraph sub { \n");
                    Rep_EBR_Disco(fp,f,ebr);
                    fprintf ( fp, "}");
                }

            }else{
                fprintf ( fp, "primaria%s [label=\"primaria(%s)\"];", mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_name);
            }
        }
    }
    if((mbr.mbr_partition_3.part_status != 'n')&&(TAG != 10)){
        if(mbr.mbr_partition_3.part_start == comparador){
            comparador += mbr.mbr_partition_3.part_size;
            TAG = 10;
            if(mbr.mbr_partition_3.part_type == 'e'){
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                ebrInicial = ebr.part_next;
                if(ebr.part_status == 'n'){
                    fprintf ( fp, "extendida [label=\"Extendida(vacia)\"];");
                }else{
                    fprintf ( fp, "subgraph sub1 { \n");
                    Rep_EBR_Disco(fp,f,ebr);
                    fprintf ( fp, "}");
                }
            }else{
                fprintf ( fp, "primaria%s [label=\"primaria(%s)\"];", mbr.mbr_partition_3.part_name, mbr.mbr_partition_3.part_name);
            }
        }
    }
    if((mbr.mbr_partition_4.part_status != 'n')&&(TAG != 10)){
        if(mbr.mbr_partition_4.part_start == comparador){
            comparador += mbr.mbr_partition_4.part_size;
            TAG = 10;
            if(mbr.mbr_partition_4.part_type == 'e'){
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                ebrInicial = ebr.part_next;
                if(ebr.part_status == 'n'){
                    fprintf ( fp, "extendida [label=\"Extendida(vacia)\"];");
                }else{
                    fprintf ( fp, "subgraph sub1 { \n");
                    Rep_EBR_Disco(fp,f,ebr);
                    fprintf ( fp, "}");
                }
            }else{
                fprintf ( fp, "primaria%s [label=\"primaria(%s)\"];", mbr.mbr_partition_4.part_name, mbr.mbr_partition_4.part_name);
            }
        }
    }

    int respaldo = 984315;
    if(TAG == 0){
        fprintf ( fp, "libre [label=\"libre\"];");
        respaldo = comparador;

        if(comparador < mbr.mbr_partition_1.part_start){
            respaldo = mbr.mbr_partition_1.part_start;
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_2.part_start)&&((respaldo > mbr.mbr_partition_2.part_start)||(TAG == 0))){
            respaldo = mbr.mbr_partition_2.part_start;
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_3.part_start)&&((respaldo > mbr.mbr_partition_3.part_start)||(TAG == 0))){
            respaldo = mbr.mbr_partition_3.part_start;
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_4.part_start)&&((respaldo > mbr.mbr_partition_4.part_start)||(TAG == 0))){
            respaldo = mbr.mbr_partition_4.part_start;
            TAG = 11;
        }
    }

    if(TAG == 10){
        Rep_Disco_Interno(fp,f,mbr,comparador);
    }else if(comparador < respaldo){
        Rep_Disco_Interno(fp,f,mbr,respaldo);
    }
}

void Rep_Disco(char path[100], char pSalida[150]){
    struct MBR mbr;
    struct EBR ebr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL){
        f = fopen (path, "rb+");
            FILE *fp = fopen ( "/home/manugr/Desktop/reporteDisk.dot", "w" );
        fread (&mbr, sizeof(mbr), 1,f);
        fprintf ( fp, "digraph disk { \n subgraph sub { \n node [shape=rectangle style=filled color=black fillcolor=white];\n");
        fprintf ( fp, "mbr [label=\"MBR\"];");
        TAG = 0;

        Rep_Disco_Interno(fp,f,mbr,sizeof(mbr));

        fprintf ( fp, "}\n }\n");

        fclose(f);
        fclose(fp);
        char temp[200];
        sprintf(temp, "dot -Tpng \"/home/manugr/Desktop/reporteDisk.dot\" -o \"%s\"", pSalida);
        system(temp);
        strcpy(temp,"");
        sprintf(temp, "run-mailcap \"%s\" &", pSalida);
        system(temp);
    }else{
        printf("\t>El archivo no existe!\n");
    }
}

/*
    Analizador de comandos
 */
void Analizar_Comando(char *linea, char *palabra) {

    char aux_String[100] = "";
    char size[10];
    char unit[2];
    char type[2];
    char name[20];
    char path[100];
    char ruta_Disco[100];
    char ruta_Destino[100];
    char fit[2];
    char delete[4];
    int  add, dsm;
    char id[5];
    char dfk, dfm, dfh, dfi;
    char fs[3];

    //Area de creacion de discos
    if(strcasecmp(palabra, "mkdisk") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        while(temp != NULL){
            if(strcasecmp(temp, "-size")==0){
                temp = strtok(NULL, " ");
                if (temp[0] == ':'){
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(size, temp);
            }else if(strcasecmp(temp, "+unit")==0){
                temp = strtok(NULL, " ");
                if (temp[0] == ':'){
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(unit, temp);
            }else if (strcasecmp(temp,"-path")==0){
                temp = strtok(NULL, " ");
                if (temp[0] == ':'){
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(aux_String, temp);
                if(aux_String[0] == '\"'){
                    temp++;
                    strcpy(ruta_Disco, temp);
                    temp = strtok(NULL, "\"");
                    strcat(ruta_Disco, " ");
                    strcat(ruta_Disco, temp);
                }else{
                    strcpy(ruta_Disco, temp);
                }
            }else if(strcasecmp(temp,"-name")==0){
            	temp = strtok(NULL, " ");
            	if (temp[0] == ':'){
                    memmove(temp, temp+1, strlen(temp));
                }
            	strcpy(name, temp);
        	}else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>%s no es un modificador valido para la instruccion Mkdisk.\n",temp);
            }//Fin del if que verifica que modificador es
            temp = strtok(NULL, "::");
        }//Fin del while que obtiene las propiedades

        if((strcasecmp(size,"")!=0)&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0)){

        	//Crea los directorios si no existen
            Crear_Directorios_Reales(ruta_Disco);

            //Asgino el valor de Kilos o Megas
            if(strcasecmp(unit,"k")==0){
                Multiplicador = 1;
            }else if(strcasecmp(unit,"m")==0){
                Multiplicador = 1024;
            }else if(strcasecmp(unit,"")==0){
                Multiplicador = 1024;
            }else{
                Multiplicador = -1;
            }

            //Valido el multiplicador para crear el disco
            if(Multiplicador == -1){
                printf("\t>Unidad invalida, porfavor intentelo nuevamente.\n\t>Las unidades permitidas son \"K\" y \"M\".\n");
            }else{
                printf("\t>Creando disco... \n");
                Crear_Disco(size, name, ruta_Disco);
            }//Fin del If donde se crea el disco.
        }else{
            printf("\t>Faltan modificadores obligatorios.\n \t>Por favor intentelo nuevamente e ingrese el tamaño y la ruta por lo menos.\n");
        }//Fin del If que revisa los parametros obligatorios

    //Area de eliminacion de disco
    }else if (strcasecmp(palabra, "rmdisk") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        if (strcasecmp(temp,"-path")==0){
            temp = strtok(NULL, " ");
            strcpy(aux_String, temp);
            if(aux_String[0] == '\"'){
                temp++;
                strcpy(ruta_Disco, temp);
                temp = strtok(NULL, "\"");
                strcat(ruta_Disco, " ");
                strcat(ruta_Disco, temp);
            }else{
                strcpy(ruta_Disco, temp);
            }
            printf("Eliminaria disco");
        }else if(strcasecmp(temp, "\n") == 0){

        }else if(strcasecmp(temp, "\r\n") == 0){

        }else{
            printf("\t>Comando invalido, para eliminar un disco por favor ingrese su ruta con el comando -path.\n");
        }

    //Area de creacion de particiones primarias, logicas y extendidas
    }else if(strcasecmp(palabra, "fdisk") == 0){
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");

        while(temp != NULL){
            if(strcasecmp(temp,"-size") == 0){
                temp = strtok(NULL, " ");
                strcpy(size, temp);
            }else if(strcasecmp(temp,"+unit") == 0){
                temp = strtok(NULL, " ");
                strcpy(unit, temp);
            }else if(strcasecmp(temp,"-path") == 0){
                temp = strtok(NULL, " ");
                strcpy(aux_String, temp);
                if(aux_String[0] != '\"'){
                    strcpy(ruta_Disco, temp);
                }else{
                    temp++;
                    strcpy(ruta_Disco, temp);
                    temp = strtok(NULL, "\"");
                    strcat(ruta_Disco, " ");
                    strcat(ruta_Disco, temp);
                }
            }else if(strcasecmp(temp,"+type") == 0){
                temp = strtok(NULL, " ");
                strcpy(type, temp);
            }else if(strcasecmp(temp,"+fit") == 0){
                temp = strtok(NULL, " ");
                strcpy(fit, temp);
            }else if(strcasecmp(temp,"+delete") == 0){
                temp = strtok(NULL, " ");
                strcpy(delete, temp);
            }else if(strcasecmp(temp,"-name") == 0){
                temp = strtok(NULL, " ");
                strcpy(name, temp);
            }else if(strcasecmp(temp,"+add") == 0){
                temp = strtok(NULL, " ");
                strcpy(add, temp);
            }else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Modificador invalido %s\n", temp);
            }
            temp = strtok(NULL, "::");
        }

        //Validacion de los datos
        if (((strcasecmp(size,"")!=0)&&(strcasecmp(delete,"")==0))&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0)){
            //Validacion de que el fit este correcto
            if((strcasecmp(fit,"bf") == 0)||(strcasecmp(fit,"ff") == 0)||(strcasecmp(fit,"wf") == 0)||(strcasecmp(fit,"") == 0)){
                //Validacion de que el type este correcto
                if((strcasecmp(type,"p") == 0)||(strcasecmp(type,"e") == 0)||(strcasecmp(type,"l") == 0)||(strcasecmp(type,"") == 0)){
                    Crear_Particion(name[20], size[10], unit[2], fit[2], type[1], path[100]);
                }else{
                    printf("\t>Caracter no reconocido.\n\t>Los types validos son: P, E y L.\n");
                }
            }else{
                printf("\t>Caracter no reconocido.\n\t>Los tipos de fit disponibles son: BF, FF y WF.\n");
            }
        }else if((strcasecmp(delete,"")!=0)&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0)){
            Eliminar_Particiones(delete[4], name[20], ruta_Disco[100]);
        }else{
            printf("\t>No se han escrito todos los datos esenciales, por favor intentelo nuevamente...\n");
        }

    //Area para montar particiones
    }else if (strcasecmp(palabra, "mount") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        while (temp != NULL) {
            if (strcasecmp(temp,"-name")==0){
                temp = strtok(NULL, " ");
                strcpy(name, temp);
            }else if (strcasecmp(temp,"-path")==0){
                temp = strtok(NULL, " ");
                strcpy(aux_String, temp);
                if(aux_String[0] == '\"'){
                    temp++;
                    strcpy(ruta_Disco, temp);
                    temp = strtok(NULL, "\"");
                    strcat(ruta_Disco, " ");
                    strcat(ruta_Disco, temp);
                }else{
                    strcpy(ruta_Disco, temp);
                }
            }else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Comando invalido, para montar una particion debe ingresar el nombre y la ruta.\n");
            }
            temp = strtok(NULL, "::");
        }
        Montar(name[20], path[100]);

    //Area para desmontar particiones
    }else if(strcasecmp(palabra, "unmount") == 0){
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        if (strcasecmp(temp,"-id")==0){
                temp = strtok(NULL, " ");
                strcpy(id, temp);
        }else{
                printf("\t>Comando invalido, para desmontar debe ingresar el id asignado a la particion.\n");
        }
        Desmontar(id[4]);
    }else if(strcasecmp(palabra, "rep") == 0){
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");

        while(temp != NULL){
            if(strcasecmp(temp,"-name") == 0){
                temp = strtok(NULL, " ");
                strcpy(name, temp);
            }else if(strcasecmp(temp,"-path") == 0){
                temp = strtok(NULL, " ");
                strcpy(aux_String, temp);
                if(aux_String[0] == '\"'){
                    temp++;
                    strcpy(ruta_Destino, temp);
                    temp = strtok(NULL, "\"");
                    strcat(ruta_Destino, " ");
                    strcat(ruta_Destino, temp);
                }else{
                    strcpy(ruta_Destino, temp);
                }
            }else if(strcasecmp(temp,"-id") == 0){
                temp = strtok(NULL, " ");
                strcpy(id, temp);
            }else if(strcasecmp(temp,"+ruta") == 0){
                temp = strtok(NULL, " ");
                strcpy(path, temp);
            }else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("Comando invalido %s, para desplegar un reporte necesita el nombre, la ruta y el id.\n", temp);
            }
            temp = strtok(NULL, "::");
        }
        Crear_Directorios_Reales(ruta_Destino);
        printf("Haria un reporte");

    //Area de lectura de scripts
    }else if (strcasecmp(palabra, "exec") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        if (strcasecmp(temp,"-path")==0){
            temp = strtok(NULL, " ");
            strcpy(aux_String, temp);
            if(aux_String[0] == '\"'){
                temp++;
                strcpy(ruta_Disco, temp);
                temp = strtok(NULL, "\"");
                strcat(ruta_Disco, " ");
                strcat(ruta_Disco, temp);
            }else{
                strcpy(ruta_Disco, temp);
            }

            FILE *f = fopen (ruta_Disco, "r");

            if(f != NULL){
                while(feof(f)==0){
                    strcpy(Normal,"");
                    fgets(Normal,100,f);
                    iWhile = 0;
                    strcpy(aux_String, Normal);
                    if(aux_String[0] != '#'){
                        if((strcasecmp(Normal,"") != 0) && (strlen(Normal) > 2)){
                            printf("Script: %s\n", Normal);
                            strcpy(Linea_Comparable, Normal);
                            char *lista = strtok(Normal, " ");
                            TAG_Script = 1;
                            Analizar_Comando(Linea_Comparable, lista);
                        }

                    }else{
                        printf(">> %s\n", aux_String);
                    }
                }
            }else{
                printf("\t>No se encontro el archivo en la ruta especificada.\n");
            }
        }else if(strcasecmp(temp, "\n") == 0){

        }else{
                printf("\t>Para ejecutar un script unicamente debe ingresar la ruta del archivo con el comando -path.\n");
        }
    }else if(strcasecmp(palabra, "df") == 0){
    	char *temp = strtok(linea, " ");
    	temp = strtok(NULL, " ");

    	while(temp != NULL){
    		if(strcasecmp(temp, "+k") == 0){
    			dfk = 'k';
    		}else if(strcasecmp(temp, "+m") == 0){
    			dfm = 'm';
    		}else if(strcasecmp(temp, "+h") == 0){
    			dfh = 'h';
    		}else if(strcasecmp(temp, "+i") == 0){
    			dfi = 'i';
    		}else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Comando invalido, para disk free.\n");
            }
    		temp = strtok(NULL, " ");
    	}

    	if((dfk == NULL) || (dfm == NULL) || (dfh == NULL) || (dfi == NULL)){
            dfk = 'k';
    	}

    	printf("Hace lo del disk free.\n");

    }else if(strcasecmp(palabra, "du") == 0){
    	char *temp = strtok(linea, " ");
    	temp = strtok(NULL, "::");

    	while(temp != NULL){
    		if(strcasecmp(temp, "+n") == 0){
    			temp = strtok(NULL, " ");
    			dsm = (int)strtol(temp, (char **)NULL, 10);
    		}else if(strcasecmp(temp, "-h") == 0){
                temp = strtok(NULL, " ");
    			dfh = 'h';
    		}else if(strcasecmp(temp, "-path") == 0){
                temp = strtok(NULL, " ");
                strcpy(aux_String, temp);
                if(aux_String[0] == '\"'){
                    temp++;
                    strcpy(ruta_Disco, temp);
                    temp = strtok(NULL, "\"");
                    strcat(ruta_Disco, " ");
                    strcat(ruta_Disco, temp);
                }else{
                    strcpy(ruta_Disco, temp);
                }
    		}else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Comando invalido, para disk used.\n");
            }
    		temp = strtok(NULL, "::");
    	}
    	if((ruta_Disco != "") && (dfh == 'h')){
            printf("Hace lo del disk used.\n");
    	}

    }else if (strcasecmp(palabra, "mkfs") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");

        while(temp != NULL){
            if(strcasecmp(temp,"-id") == 0){
                temp = strtok(NULL, " ");
                strcpy(id, temp);
            }else if(strcasecmp(temp,"+type") == 0){
                temp = strtok(NULL, " ");
                strcpy(type, temp);
            }else if(strcasecmp(temp,"+add") == 0){
                temp = strtok(NULL, " ");
                add = (int)strtol(temp, (char**)NULL, 10);
            }else if(strcasecmp(temp,"+unit") == 0){
                temp = strtok(NULL, " ");
                strcpy(unit, temp);
            }else if(strcasecmp(temp,"+fs") == 0){
                temp = strtok(NULL, " ");
                strcpy(fs, temp);
            }else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Comando invalido para mkfs.\n");
            }
            temp = strtok(NULL, ":");
        }

        if((strcasecmp(type, "fast") == 0) || (strcasecmp(type, "full") == 0)){

        }else if(strcasecmp(type, "")){
            strcpy(type, "full");
        }else{
            printf("El tipo de formateo no es valido.\n");
        }

        if(strcasecmp(id, "") == 0){
            printf("Se necesita el modificador id para realizar formateos.\n");
        }else{
            char c = id[2];
            char ns[3];
            sprintf(ns, "%c%c", id[3],id[4]);
            aux_int = 0;
            while(c != Abecedario[aux_int]){
                aux_int++;
            }
            convertido = (int)strtol(ns, (char **)NULL, 10);
            strcpy(name, Montador[aux_int][convertido]);
            strcpy(ruta_Disco, Montador[aux_int][0]);
            //Formatear(name, ruta_Disco);
            printf("Formatearia la particion");
        }

    }else if(strcasecmp(palabra, "exit") == 0) {
        TAG_Script = -7;
    }else if(strcasecmp(palabra, "clear") == 0) {
        system("clear");
    }else{
        printf("\t>Comando invalido. Por favor intentelo nuevamente...\n");
    }

    //Valido si seguir leyendo comandos o realizar el script
    if(TAG_Script == 0){
        Ingresar_Comando();
    }else if(TAG_Script == -7){

    }else{
        TAG_Script = 0;
    }
}