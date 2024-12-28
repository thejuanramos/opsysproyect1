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

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
   
   // Search for the source file in the directory
   int s_file = BuscaFich(directorio, inodos, nombreorigen);
   // Search for the destination file in the directory
   int d_file = BuscaFich(directorio, inodos, nombredestino);

   // If the source file does not exist, return an error
   if (s_file == -1) {
      printf("ERROR: Source file '%s' not found.\n", nombreorigen);
     return -1;
   }

   
   if (d_file != -1) {
      printf("ERROR: A file with the name '%s' already exists.\n", nombredestino);
      return -1;
   }

   // Find a free inode for the new file
   int new_inode = -1;
   for (int i = 0; i < MAX_INODOS; i++) {
      if (ext_bytemaps->bmap_inodos[i] == 0) {
         new_inode = i;
         ext_bytemaps->bmap_inodos[i] = 1; // Mark the inode as used
         ext_superblock->s_free_inodes_count--; // Decrement the free inode count
         break;
      }
   }

   // If no free inode is found, return an error
   if (new_inode == -1) {
      printf("No free inodes available.\n");
      return -1;
   }

   // Find a free directory entry for the new file
   int new_dir_idx = -1;
   for (int i = 0; i < MAX_FICHEROS; i++) {
      if (directorio[i].dir_inodo == NULL_INODO) {
         new_dir_idx = i;
         strcpy(directorio[i].dir_nfich, nombredestino); // Set the new file name
         directorio[i].dir_inodo = new_inode; // Associate the new inode with the directory entry
         break;
      }
   }

   // If no free directory entry is found, return an error
   if (new_dir_idx == -1) {
      printf("No space available in the directory.\n");
      return -1;
   }

   // Copy the inode metadata from the source file to the new file
   EXT_SIMPLE_INODE *source_inode = &inodos->blq_inodos[directorio[s_file].dir_inodo];
   EXT_SIMPLE_INODE *destination_inode = &inodos->blq_inodos[new_inode];
   destination_inode->size_fichero = source_inode->size_fichero;

   // Copy the blocks of data from the source file to the new file
   for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
      if (source_inode->i_nbloque[i] != NULL_BLOQUE) {
         // Find a free block for the new file
         int new_block_idx = -1;
         for (int j = 0; j < MAX_BLOQUES_DATOS; j++) {
            if (ext_bytemaps->bmap_bloques[j] == 0) {
               new_block_idx = j;
               ext_bytemaps->bmap_bloques[j] = 1; // Mark the block as used
               ext_superblock->s_free_blocks_count--; // Decrement the free blocks count
               break;
            }
         }

          // If no free block is available, return an error
         if (new_block_idx == -1) {
            printf("ERROR: No free blocks available to copy file.\n");
            return -1;
         }

         // Assign the new block to the destination inode
         destination_inode->i_nbloque[i] = new_block_idx;
          // Copy the content of the source block to the destination block
         memcpy(memdatos[new_block_idx - PRIM_BLOQUE_DATOS].dato,memdatos[source_inode->i_nbloque[i] - PRIM_BLOQUE_DATOS].dato,SIZE_BLOQUE);
      } else {
      // If the source inode has no block at this position, set the destination block to NULL
      destination_inode->i_nbloque[i] = NULL_BLOQUE;
      }
   }

   printf("File '%s' copied to file '%s'.\n", nombreorigen, nombredestino);

   return 0;
}


int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre){

   int file = BuscaFich(directorio, inodos, nombre);

   // Check if the file exists in the directory
   if(file == -1){
      printf("ERROR: file '%s' not found.\n", nombre);
      return -1;
   }

   // Get the inode associated with the file
   int inodo_file = directorio[file].dir_inodo;
   EXT_SIMPLE_INODE *inode = &inodos->blq_inodos[inodo_file];

   // Iterate through the blocks assigned to the inode and print their contents
   for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++){
      if(inode->i_nbloque[i] != NULL_BLOQUE){
         printf("%s", memdatos[inode->i_nbloque[i] - PRIM_BLOQUE_DATOS].dato);
      }
   }

   // Ensure a newline is printed after the file content
   printf("\n");

   return 0;

}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich){

   int file = BuscaFich(directorio, inodos, nombre);

   // Check if the file exists in the directory
   if(file == -1){
      printf("ERROR: file '%s' not found.\n", nombre);
      return -1;
   }

   // Get the inode associated with the file
   int inodo_file = directorio[file].dir_inodo;
   EXT_SIMPLE_INODE *inode = &inodos->blq_inodos[inodo_file];

   // Free all blocks associated with the file
   for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++){
      if(inode->i_nbloque[i] != NULL_BLOQUE){
         ext_bytemaps->bmap_bloques[inode->i_nbloque[i]] = 0; // Mark the block as free in the block bytemap
         ext_superblock->s_free_blocks_count ++; // Increment the free block count in the superblock
         inode->i_nbloque[i] = NULL_BLOQUE; // Unlink the block from the inode
      }
   }

