#ifndef TOOLS_HPP  // Évite les inclusions multiples
#define TOOLS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/rand.h>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <list>
#include <regex>
#include <cmath>

/**
 * Génère un fichier text de longeur "length" avec des caractères aléatoires (sert de clef de chiffrement)
 * @param { filename } chemin vers le fichier qui sera créer
 * @param { length } nombre de caractère du message
 * @return { None }
 */

void generate_openssl_key(const std::string& filename, size_t length);

/**
 * Compresse un fichier VCF
 * @param { source } Chemin vers le VCF brut
 * @param { destination } Chemin vers le nouveau fichier compressé
 * @param { index } Chemin vers le fichier CSV contenant l'index de séparation
 * @param { key } Chemin vers le fichier txt contenant le clef de compression
 * @param { nbThread } Nombre de thread [integer] par default 1
 * @param { blockLength } Taille des blocs de compression [integer] par default 1024
 * @return { None }
 */
void compress(std::string source, std::string destination, std::string index, std::string key, int nbThread, int blockLength);

/**
 * Decompresse un fichier VCF
 * @param { source } Chemin vers le fichier compressé
 * @param { destination } Chemin vers le nouveau fichier décompressé
 * @param { nameCol } Chemin vers le fichier txt contenant les ID des patients qu'on veut décompresser
 * @param { key } Chemin vers le fichier txt contenant le clef de compression
 * @return { None }
 */
void decompress(std::string source, std::string destination, std::string nameCol, std::string key);

/**
 * Decompresse une partie d'un fichier VCF
 * @param { source } Chemin vers le fichier compressé
 * @param { destination } Chemin vers le nouveau fichier décompressé
 * @param { nameCol } Chemin vers le fichier txt contenant les ID des patients qu'on veut décompresser
 * @param { key } Chemin vers le fichier txt contenant le clef de compression
 * @param { Chrom } Le chromosome selectionné
 * @param { pos } Position initial des base à décompresser
 * @param { taille } Taille de la séquence a décompresser
 * @return { None }
 */
void randomAcces(std::string source, std::string destination, std::string nameCol, std::string key, std::string Chrom, int pos, int taille);

/**
 * Supprimer un fichier
 * @param { filename } Chemin vers le fichier qu'on veut supprimer
 * @return { None }
 */
void remove(std::string filename);

/**
 * Supprimer un document
 * @param { filename } Chemin vers le document qu'on veut supprimer
 * @return { None }
 */
void removeDos(std::string filename);

/**
 * Creéer un fichier VCF avec un subset des patients
 * @param { VCF } Chemin vers le fichier VCF avec toutes les données
 * @param { subset_patients } Chemin vers le fichier txt contenant la liste des patients à selectionner
 * @param { dest } Chemin vers le fichier VCF contenant juste les patients sélectionnés
 * @return { None }
 */
void sub_VCF(std::string VCF, std::string subset_patients, std::string dest);

/**
 * Fusionne une list de fichier VCF en un seul
 * @param { vcf_files } Liste des fichiers VCF à fusionner
 * @param { output_vcf } Chemin vers le nouveau fichier VCF
 * @return { None }
 */
void fusionner_vcf(const std::vector<std::string>& vcf_files, const std::string& output_vcf);

#endif 