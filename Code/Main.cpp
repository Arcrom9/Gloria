#include "tools.hpp"
namespace fs = std::filesystem;

/**
 * Créer un fichier CSV (index_name) et un fichier txt (key_name) pour chaque name dans le fichier 'input_file'
 * @param { input_file } Chemin vers le fichier contenant la liste des patients
 * @param { key_folder } Chemin vers le dossier avec les cles d'accès par patient
 * @param { index_folder } Chemin vers le dossier avec les index de découpage par patient
 * @return { None }
 */
void creat_file(std::string input_file, std::string index_folder, std::string key_folder) {
    // vérifie que les dossiers soient créés
    if (fs::create_directory(index_folder)) {
        std::cout << "Dossier créé : " << index_folder << std::endl;
    } 
    else {
        std::cout << "Échec ou le dossier existe déjà : " << index_folder << std::endl;
    }
     if (fs::create_directory(key_folder)) {
        std::cout << "Dossier créé : " << key_folder << std::endl;
    } 
    else {
        std::cout << "Échec ou le dossier existe déjà : " << key_folder << std::endl;
    }

    std::ifstream infile(input_file);

    if (!infile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << input_file << std::endl;
    }

    std::string name;
    const long long start_pos = 1;
    const long long end_pos = 3 * 100000000; // 3*10^8

    std::string ligne;
    // Lire chaque ligne du fichier
    std::getline(infile, ligne);
    std::stringstream ss(ligne);  // Utiliser un stringstream pour découper la ligne
    std::string IDPatient;
    // Découper chaque entrée séparée par une virgule
    while (std::getline(ss, IDPatient, ',')) {
        std::string new_key = key_folder+"Key_"+ IDPatient + ".key";
        if (fs::exists(new_key)){
            std::cout << "La clef " << new_key <<" existe déjà" << std::endl;
        }
        else{
            generate_openssl_key(new_key, 256);
        }
        // Construire le nom du fichier CSV
        std::string csv_filename = index_folder+"Index_"+ IDPatient + ".csv";

        if (fs::exists(csv_filename)){
            std::cout << "L'index' " << csv_filename <<" existe déjà" << std::endl;
        }
        else{
            // Ouvrir un fichier CSV pour écriture
            std::ofstream csv_file(csv_filename);
            if (!csv_file.is_open()) {
                std::cerr << "Erreur : Impossible de créer le fichier " << csv_filename << std::endl;
                continue;
            }

            // Écrire l'en-tête du CSV
            csv_file << "chrom,start_pos,end_pos\n";

            // Écrire les lignes de chrom 1 à 22
            for (int chrom = 1; chrom <= 22; ++chrom) {
                csv_file << chrom << "," << start_pos << "," << end_pos << "\n";
            }
            csv_file << "X," << start_pos << "," << end_pos << "\n";
            csv_file << "Y," << start_pos << "," << end_pos << "\n";
            std::cout << "Fichier créé : " << csv_filename << std::endl;
        }
    }

    infile.close();
}

/**
 * Converti des fichier VCf compressé par chromosome en fichier compressé par patient
 * [Décompresse les VCF par chromosome pour 1 patient de la liste puis fusionne ces VCF enfin les compresse à nouveau]
 * @param { source } Chemin vers le dossier avec les VCF stocker par chromosome
 * @param { input_file } Chemin vers le fichier contenant la liste des patients
 * @param { dest } Chemin vers le dossier accueillant les nouveau fichier compresser par patient
 * @param { key_folder } Chemin vers le dossier avec les cles d'accès par patient
 * @param { index_folder } Chemin vers le dossier avec les index de découpage par patient
 * @return { None }
 */
void chrom_patient(std::string source, std::string input_file, std::string dest, std::string key_folder, std::string index_folder)
{
    // Initialise les variables de temps et print l'heure du lancement
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&now_time);
    std::cout << "Initiation : " 
              << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S") 
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    if (fs::create_directory(dest)) {
        std::cout << "Dossier créé : " << dest << std::endl;
    } 
    else {
        std::cout << "Échec ou le dossier existe déjà : " << dest << std::endl;
    }

    std::string folder_name = "Temp_Decompress";
    if (fs::create_directory(folder_name)) {
        std::cout << "Dossier créé : " << folder_name << std::endl;
    } 
    else {
        std::cout << "Échec ou le dossier existe déjà : " << folder_name << std::endl;
    }

    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << input_file << std::endl;
    }
    std::string ligne;
    std::getline(file, ligne);
    std::stringstream ss(ligne);  // Utiliser un stringstream pour découper la ligne
    std::string IDPatient;
    // Découper chaque entrée séparée par une virgule
    while (std::getline(ss, IDPatient, ',')) {
        std::cout << "IDPatient : " << IDPatient << std::endl;
        std::string outputTemp = source+"_temp";
        std::ofstream output(outputTemp);
        output << IDPatient;
        output.close();

        std::string destination = dest+IDPatient+".svc";
        if (!fs::exists(destination)){

            std::vector<std::string> vcf_files = {};
            for (const auto& entry : fs::directory_iterator(source)) {
                std::string filename =  entry.path().filename().string();
                std::cout << " " << std::endl;
                std::cout << filename << std::endl;
                std::cout << " " << std::endl;
                
                std::string C_raw = source +"/"+filename;
                std::string NWE = std::filesystem::path(filename).stem().string(); //NameWithoutExtension
                std::string D_raw = folder_name+"/"+IDPatient+"_"+NWE+".vcf";
                std::string k = "../Data/Key/key_"+NWE+".key";
                if (!fs::exists(D_raw)){
                    start = std::chrono::high_resolution_clock::now();
                    decompress(C_raw, D_raw, outputTemp, k); // !!!
                    end = std::chrono::high_resolution_clock::now();
                    duration = end - start;
                    std::cout << "Temps de decompression : " << duration.count() << " secondes" << std::endl;
                    std::cout << " " << std::endl;
                }
                if (fs::file_size(D_raw) == 0){
                    std::cout << D_raw << " est vide" << std::endl;
                }
                else{
                    vcf_files.push_back(D_raw);
                    std::cout << D_raw << " est ajouté" << std::endl;
                }
            }

            std::string VCF_merge = dest + IDPatient;
            start = std::chrono::high_resolution_clock::now();
            fusionner_vcf(vcf_files, VCF_merge); // !!!
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "Temps de merge : " << duration.count() << " secondes" << std::endl;
            std::cout << " " << std::endl;


            std::string key = key_folder+"Key_"+IDPatient+".key";
            std::string index = index_folder+"Index_"+IDPatient+".csv";
            start = std::chrono::high_resolution_clock::now();
            compress(VCF_merge, destination , index, key, 4, 1024); // !!!
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            std::cout << "Temps de compression : " << duration.count() << " secondes" << std::endl;
            std::cout << " " << std::endl;

            if (fs::exists(VCF_merge)) remove(VCF_merge);
        }
        if (fs::exists(outputTemp)) remove(outputTemp);
    }
    if (fs::exists(folder_name)) removeDos(folder_name);

    // Fermer le fichier après traitement
    file.close();
}

int main() {

    // Nom du fichier texte contenant les noms
    std::string input_file = "../Data/list_ID(réduite)";
    std::string key_folder = "../Data/Key_mono/";
    std::string index_folder = "../Data/Index_mono/";
    creat_file(input_file, index_folder, key_folder);

    // Dossier où se trouvent les VCF compressé par chromosome
    std::string source = "../Data/Compressed/";
    // Dossier où se trouveront les VCF compressé par patient
    std::string dest = "../Data/Compressed_mono/";
    chrom_patient(source, input_file, dest, key_folder, index_folder);

    return 0;
}