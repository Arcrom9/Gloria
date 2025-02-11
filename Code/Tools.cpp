#include "tools.hpp"

namespace fs = std::filesystem;


void generate_openssl_key(const std::string& filename, size_t length) {
    std::vector<unsigned char> key(length);
    if (RAND_bytes(key.data(), length) != 1) {
        std::cerr << "Erreur : Impossible de générer une clé avec OpenSSL" << std::endl;
        return;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(key.data()), length);
    file.close();
    std::cout << "Clé binaire sauvegardée dans : " << filename << std::endl;
}

void compress(std::string source, std::string destination, std::string index, std::string key, int nbThread, int blockLength){
    std::string command = "../SVC/svc_exec -f "+source+" -o "+destination+" -i "+index+" -k "+key+" -j "+std::to_string(nbThread)+" -b "+std::to_string(blockLength) ;
    system(command.c_str());
}

void decompress(std::string source, std::string destination, std::string nameCol, std::string key){
    std::string command = "../SVC/svc_exec -r -f "+source+" -o "+destination+" -s "+nameCol+" -k "+key;
    system(command.c_str());
}

void randomAcces(std::string source, std::string destination, std::string nameCol, std::string key, std::string Chrom, int pos, int taille){
    std::string command = "../SVC/svc_exec -r -f "+source+" -o "+destination+" -c "+Chrom+":"+Chrom+" -p "+std::to_string(pos)+":"+std::to_string(pos+taille)+" -s "+nameCol+" -k "+key;
    system(command.c_str());
}

void remove(std::string filename){
    std::string command = "rm "+filename;
    system(command.c_str());
}

void removeDos(std::string filename){
    std::string command = "rm -rf "+filename;
    system(command.c_str());
}

void sub_VCF(std::string VCF, std::string subset_patients, std::string dest){
    std::string command = "bcftools view -S " + subset_patients + " --trim-alt-alleles -Ou " + VCF + " | bcftools view -e 'GT=\"0\"' -o " + dest + " -O v";
    system(command.c_str());
}

void fusionner_vcf(const std::vector<std::string>& vcf_files, const std::string& output_vcf) {
    std::stringstream command;
    command << "bcftools concat ";
    // Ajouter tous les fichiers VCF à la commande
    for (const auto& vcf : vcf_files) {
        command << vcf << " ";
    }
    // Spécifier le fichier de sortie
    command << "-o " << output_vcf << " -O v";
    // Exécuter la commande
    int result = system(command.str().c_str());
    if (result == 0) {
        std::cout << "Fusion réussie ! Le fichier fusionné est : " << output_vcf << std::endl;
    } else {
        std::cerr << "Erreur lors de la fusion des fichiers VCF." << std::endl;
    }
}
