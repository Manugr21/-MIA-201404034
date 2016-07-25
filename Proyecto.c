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

struct Super_Bloque{
	int		s_filesystem_type;
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
	int 		Journal_tipo_operacion;		//Indica que tipo de operacion se realizo
	int 		Journal_tipo;				//0 Archivos | 1 Carpetas
	char[10]	Journal_nombre;				//Nombre del archivo o directorio
	int 		Journal_contenido; 			//Indica si hay datos contenidos
	time 		Journal_fecha;				//Fecha de la transaccion
	char[10]	Journal_propietario;		//Nombre del usuario propietario del archivo o carpeta
	int 		Journal_Permisos;			//Son los permisos que tenia el archivo o carpeta
};