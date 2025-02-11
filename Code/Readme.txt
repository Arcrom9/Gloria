Attention que la structure/architecture des dossiers et fichiers doivent être exactement identique à la mienne pour que cela fonctionne bien.
Sinon il faut changer les chemins vers ceux-ci.

Compilation :
g++ -std=c++17 -lstdc++fs Main.cpp Tools.cpp -o Main_exe -lssl -lcrypto

Execution :
./Main_exe
