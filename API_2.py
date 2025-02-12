# -*- coding: utf-8 -*-
"""
Created on Fri Jan 31 11:36:34 2025

@author: henaute
"""

#
# ligne de commande de lancement 
# uvicorn API_2:app --reload --host 0.0.0.0 --port 8001
#

#
# Ligne de commande de test
# curl -X POST "http://127.0.0.1:8001/compress" \
#        -H "Content-Type: application/json" \
#        -d '{
#             "source": "Data/Decompress/HG00096_chrom1.vcf",
#             "destination": "Data/output(HG00096_chrom1).svc",
#             "index": "Data/Index_mono/Index_HG00096.csv",
#             "key": "Data/Key_mono/Key_HG00096.key",
#             "nbThread": 4,
#             "blockLength": 2048
#        }'
#

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import subprocess

app = FastAPI()
@app.get("/")
def Accueil():
    return{"message" : "Bienvenu sur notre APi"}

# Modèle Pydantic pour la validation des données
class CompressionRequest(BaseModel):
    source: str
    destination: str
    index: str
    key: str
    nbThread: int = 4
    blockLength: int = 1024

@app.post("/compress")
async def compress(request: CompressionRequest):
    try:
        # Construction de la commande
        command = [
            "./SVC/svc_exec",
            "-f", request.source,
            "-o", request.destination,
            "-i", request.index,
            "-k", request.key,
            "-j", str(request.nbThread),
            "-b", str(request.blockLength)
        ]

        # Exécuter la commande
        result = subprocess.run(command, capture_output=True, text=True)

        # Retourner la sortie et les erreurs éventuelles
        return {"output": result.stdout, "error": result.stderr}

    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

"""
# Démarrer l’API avec Uvicorn
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=5000, reload=True)
"""