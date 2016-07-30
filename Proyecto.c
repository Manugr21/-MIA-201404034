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
	int 	part_size;
	int 	part_next;
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
int     Multiplicador;
int     convertido;
char    Normal[400];
char    Linea_Comparable[400];
char    Abecedario[28];

//Banderas o indicadores
int 	TAG;
int     TAG_Script = 0;

//Contadores
int     iWhile;

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

void Crear_Directorios_Reales(char path[200]){
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
    int  add, dsn;
    char id[5];
    char dfk, dfdm, dfh, dfi;
    char fs[3];

    //Area de creacion de discos
    if(strcasecmp(palabra, "mkdisk") == 0) {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "::");
        while(temp != NULL){
            if(strcasecmp(temp, "-size")==0){
                temp = strtok(NULL, " ");
                strcpy(size, temp);
            }else if(strcasecmp(temp, "+unit")==0){
                temp = strtok(NULL, " ");
                strcpy(unit, temp);
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
            }else if(strcasecmp(temp,"-name")==0){
            	temp = strtok(NULL, " ");
            	strcpy(name, temp);
        	}else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>%s no es un modificador valido para la instruccion Mkdisk.\n",temp);
            }//Fin del if que verifica que modificador es
            temp= strtok(NULL, "::");
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
                printf("\t>Unidad invalida, porfavor intentelo nuevamente.\nLas unidades permitidas son \"K\" y \"M\".\n");
            }else{
                printf("Crearia disco");
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
                    printf("Crearia una particion.\n");
                }else{
                    printf("\t>Caracter no reconocido.\n\t>Los types validos son: P, E y L.\n");
                }
            }else{
                printf("\t>Caracter no reconocido.\n\t>Los tipos de fit disponibles son: BF, FF y WF.\n");
            }
        }else if((strcasecmp(delete,"")!=0)&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0)){
            printf("Eliminaria una particion\n");
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
        printf("Montaria una particion.\n");

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
        printf("Desmonta una particion.\n");
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
    			dsk = 'k';
    		}else if(strcasecmp(temp, "+m") == 0){
    			dsm = 'm';
    		}else if(strcasecmp(temp, "+h") == 0){
    			dsh = 'h';
    		}else if(strcasecmp(temp, "+i") == 0){
    			dsi = 'i';
    		}else if(strcasecmp(temp, "\n") == 0){

            }else if(strcasecmp(temp, "\r\n") == 0){

            }else{
                printf("\t>Comando invalido, para disk free.\n");
            }
    		temp = strtok(NULL, " ");
    	}

    	if((dsk == NULL) || (dsm == NULL) || (dsh == NULL) || (dsi == NULL)){
            dsk = 'k'
    	}

    	printf("Hace lo del disk free.\n");

    }else if(strcasecmp(palabra, "du") == 0){
    	char *temp = strtok(linea, " ");
    	temp = strtok(NULL, "::");

    	while(temp != NULL){
    		if(strcasecmp(temp, "+n") == 0){
    			temp = strtok(NULL, " ");
    			dsn = (int)strtol(temp, (char **)NULL, 10);
    		}else if(strcasecmp(temp, "-h") == 0){
                temp = strtok(NULL, " ");
    			dsh = 'h';
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
    	if((ruta_Disco != "") && (dsh == 'h')){
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
            char c = Jarvis.id[2];
            char ns[3];
            sprintf(ns, "%c%c", Jarvis.id[3],Jarvis.id[4]);
            aux_int = 0;
            while(c != Abecedario[aux_int]){
                aux_int++;
            }
            convertido = (int)strtol(ns, (char **)NULL, 10);
            strcpy(name, Montador[aux_int][convertido]);
            strcpy(ruta_Disco, Montador[aux_int][0]);
            //Formatear(name, ruta_Disco);
            printf("Formatearia la particion")ñ
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



/*
	El MAIN
*/
int main()
{
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
