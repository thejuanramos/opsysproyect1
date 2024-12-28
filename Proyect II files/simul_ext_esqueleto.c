#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "headers.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre)
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
	 char *comando[LONGITUD_COMANDO];
	 char *orden[LONGITUD_COMANDO];
	 char *argumento1[LONGITUD_COMANDO];
	 char *argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     ...
     
     fent = fopen("particion.bin","r+b");
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Buce de tratamiento de comandos
     for (;;){
	do {
		printf (">> ");
		fflush(stdin);
		fgets(comando, LONGITUD_COMANDO, stdin);
	} while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
		if (strcmp(orden,"dir")==0) {
            		Directorio(directorio,&ext_blq_inodos);
            		continue;
         	}else if (strcmp(orden,"info")==0){
            		LeeSuperBloque(&psup);
           		continue;
		}else if (strcmp(orden, "bytemaps")==0){
            		Printbytemaps(&ext_bytemaps);
            		continue;
         	}else if (strcmp(orden, "rename")==0){
            		Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
            		continue;
         	}else if (strcmp(orden, "print")==0){
            		Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
            		continue;
         	}else if (strcmp(orden, "remove")==0){
            		Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
           		continue;
         	}else if (strcmp(orden, "copy")==0){
            		Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2,  fent);
            		continue;
         	}
         }
         ...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}

// Function to read and display the SuperBlock information
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){

   // Print the total number of inodes
   printf("Inodes: %u\n", psup->s_inodes_count);
   // Print the total number of blocks
   printf("Blocks: %u\n", psup->s_blocks_count);
   // Print the number of free blocks
   printf("Free Blocks: %u\n", psup->s_free_blocks_count);
   // Print the number of free inodes
   printf("Free Inodes: %u\n", psup->s_free_inodes_count);
   // Print the index of the first data block
   printf("First Data Block: %u\n", psup->s_first_data_block);
   // Print the size of a block in bytes
   printf("Block Size: %u\n", psup->s_block_size);

}

// Function to display the inode and block bytemaps
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){

   printf("Inode Bytemap: ");
   
   // Iterate over the inode bytemap and print each entry
   for (int i = 0; i < MAX_INODOS; i++){
      printf("%d ", ext_bytemaps->bmap_inodos[i]);
   }

   printf("\n Block Bytemap: ");
   
   // Iterate over the block bytemap and print each entry
   for (int i = 0; i < 25; i++){
      printf("%d ", ext_bytemaps->bmap_bloques[i]);
   }

   printf("\n");
}


// Function to display the contents of the directory
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {

   printf("Directory Contents:\n");
   printf("Name\t\t\tSize\tInode\tBlocks\n");

      // Iterate over all directory entries
      for (int i = 0; i < MAX_FICHEROS; i++) {

        if (directorio[i].dir_inodo != NULL_INODO) {
            // Fetch the inode associated with the directory entry
            EXT_SIMPLE_INODE *inode = &inodos->blq_inodos[directorio[i].dir_inodo];
            // Print file details: name, size, and inode
            printf("%-16s\t%d\t%d\t", directorio[i].dir_nfich, inode->size_fichero, directorio[i].dir_inodo);

            // Print the block indices associated with the file
            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                if (inode->i_nbloque[j] != NULL_BLOQUE) {
                    printf("%d ", inode->i_nbloque[j]);
                }
            }
            printf("\n");
         }
      }
}

