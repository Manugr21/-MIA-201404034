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
	time 	mbr_fecha_creacion;
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
	char 	part_status
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
	time	s_mtime;
	time 	s_umtime;
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
	int 	Journal_tipo;			//0 Archivos | 1 Carpetas
	char	Journal_nombre[10];		//Nombre del archivo o directorio
	int 	Journal_contenido; 		//No estoy seguro que es
	time 	Journal_fecha;			//Fecha de la transaccion
	char	Journal_propietario[10];	//Nombre del usuario propietario del archivo o carpeta
	int 	Journal_Permisos;		//Son los permisos que tenia el archivo o carpeta
};
/*
	Variable Globales
*/
//Cosas genericas pero importantes
int 	TAG;

/*
	Metodos Genericos
*/
void Ingresar_Comando() {
    char Normal[400];
    char Linea_Comparable[400];
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
        //Analizar_Comando(Normal, Lista);
    }
    else
    {
        strcpy(Linea_Comparable, Normal);
        char *Lista = strtok(Normal, " ");
        //Analizar_Comando(Linea_Comparable, Lista);
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
    char ruta[100];
    char fit[2];
    char delete[4];
    int  add;
    char id[5];
    char dfk, dfdm, dfh, dfi;

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
                    strcpy(path, temp);
                    temp = strtok(NULL, "\"");
                    strcat(path, " ");
                    strcat(path, temp);
                }else{
                    strcpy(path, temp);
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

        if((strcasecmp(size,"")!=0)&&(strcasecmp(path,"")!=0)&&(strcasecmp(name,"")!=0)){

        	//Crea los directorios si no existen
            Crear_Directorios_Reales(path);

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
                strcpy(path, temp);
                temp = strtok(NULL, "\"");
                strcat(path, " ");
                strcat(path, temp);
            }else{
                strcpy(path, temp);
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
                    strcpy(path, temp);
                }else{
                    temp++;
                    strcpy(path, temp);
                    temp = strtok(NULL, "\"");
                    strcat(path, " ");
                    strcat(path, temp);
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
        if (((strcasecmp(size,"")!=0)&&(strcasecmp(delet,"")==0))&&(strcasecmp(path,"")!=0)&&(strcasecmp(name,"")!=0)){
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
        }else if((strcasecmp(delet,"")!=0)&&(strcasecmp(path,"")!=0)&&(strcasecmp(name,"")!=0)){
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
                    strcpy(path, temp);
                    temp = strtok(NULL, "\"");
                    strcat(path, " ");
                    strcat(path, temp);
                }else{
                    strcpy(path, temp);
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
    Preparar();
    printf(">>Para apagar el sistema ingrese el comando \"exit\".\n>> Sistema listo, porfavor introduzca un comando...\n");
    //Ingresar_Comando();
    printf("Apagando...\n");
    return 0;
}
