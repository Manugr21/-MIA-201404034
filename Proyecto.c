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
char 	Normal[400];

/*
	Metodos Genericos
*/
void Ingresar_Comando() {
    contador_While = 0;
    TAG = 0;

    scanf(" %[^\n]s", Normal);
    if (Normal[strlen(Normal)-1] == '\\')  
    {
    	char temp[200];
    	scanf(" %[^\n]s", temp);
    	strcat(Normal, temp);
    }
    strcpy(Linea_Comparable, Normal);
    char *Lista = strtok(Normal, " ");
    Analizar_Comando(Linea_Comparable, Lista);
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
