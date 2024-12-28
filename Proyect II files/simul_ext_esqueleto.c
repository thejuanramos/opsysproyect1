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


// Function to parse a command and extract its components
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){

   char *token;
   int num_tokens = 0;

   // Initialize the output strings to empty
   orden[0] = '\0';
   argumento1[0] = '\0';
   argumento2[0] = '\0';

   // Remove the trailing newline character from the input command
   strcomando[strcspn(strcomando, "\n")] = '\0';

   // Tokenize the command using space as a delimiter
   token = strtok(strcomando, " ");
   while(token != NULL){

      if(num_tokens == 0){
         // First token is the command
         strcpy(orden, token);
      }else if(num_tokens == 1){
         // Second token is the first argument
         strcpy(argumento1, token);
      }else if(num_tokens == 2){
         // Third token is the second argument
         strcpy(argumento2, token);
      }else{
         // Too many tokens, invalid command format
         return 1;
      }

      num_tokens ++;
      token = strtok(NULL, " ");
   }

   // Return error if no tokens were found
   if(num_tokens == 0){
      return 1;
   }

   return 0;

}

// Function to search for a file by name in the directory
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){

   // Iterate through the directory entries
   for (int i = 0; i < MAX_FICHEROS; i++){
      if (directorio[i].dir_inodo != NULL_INODO && strcmp(directorio[i].dir_nfich, nombre) == 0){
         // Return the index of the matching entry
         return i;
      }
   }

   // File not found
   return -1;
}

// Function to write the inodes and directory to the file system
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich){

   // Write the inodes to their specific position in the file
   fseek(fich, 2 * SIZE_BLOQUE, SEEK_SET);
   fwrite(inodos, sizeof(EXT_BLQ_INODOS), 1, fich);

   // Write the directory entries to their specific position in the file
   fseek(fich, 3 * SIZE_BLOQUE, SEEK_SET);
   fwrite(directorio, sizeof(EXT_ENTRADA_DIR), 1, fich);

}

// Function to write the bytemaps to the file system
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich){

   fseek(fich, 1 * SIZE_BLOQUE, SEEK_SET);
   fwrite(ext_bytemaps, sizeof(EXT_BYTE_MAPS), 1, fich);

}

// Function to write the superblock to the file system
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich){

   fseek(fich, 0 * SIZE_BLOQUE, SEEK_SET);
   fwrite(ext_superblock, sizeof(EXT_SIMPLE_SUPERBLOCK), 1, fich);

}

// Function to write the data blocks to the file system
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich){

   fseek(fich, 4 * SIZE_BLOQUE, SEEK_SET);
   fwrite(memdatos, sizeof(EXT_DATOS), 1, fich);

}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){
    
   // Search for the file with the old name in the directory
   int file = BuscaFich(directorio, inodos, nombreantiguo);
   // Check if a file with the new name already exists
   int new_name = BuscaFich(directorio, inodos, nombrenuevo);

   // If the file with the old name does not exist, return an error
   if(file == -1){
      printf("ERROR: file '%s' not found.\n", nombreantiguo);
      return -1;
   }

   // If a file with the new name already exists, return an error
   if(new_name != -1){
      printf("ERROR: A file with the name '%s' already exist.\n", nombrenuevo);
      return -1;
   }

   // Rename the file by copying the new name into the directory entry
   strcpy(directorio[file].dir_nfich, nombrenuevo);
   printf("file %s renamed to %s.\n", nombreantiguo, nombrenuevo);

   return 0;
   
}
