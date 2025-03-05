"""
Script de visualisation des données de plusieurs fichiers de capteurs.

Objectif :
- Superposer les données de différentes stations sur un même graphique pour chaque variable (ex: humidité, température).

Utilisation :
- Modifier la liste `file_paths` pour inclure les fichiers à analyser.
- Vérifier les noms des colonnes d’intérêt dans `columns_to_plot`.
- modifier heure_min et heure_max pour filtrer les données exploitables
- Ajuster les échelles (partie du code commentée l83 à 91) si besoin

Sortie :
- Un graphique par variable mesurée, avec les données superposées de tous les fichiers.

"""



import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import os

# Liste des fichiers txt des cartes SD
file_paths = [
    r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\01032025\aquacheck1.txt",
    r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\01032025\aquacheck2.txt",
    r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\01032025\aquacheck4.txt",
    r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\01032025\aquacheck5.txt"
]

# Colonnes d'intérêt à tracer
columns_to_plot = ["Humidity (%)", "Temperature (°C)", "Soil Moisture (%)", "Humidity Purcentage (%)"] 

# Graphiques pour chaque colonne, une couleur de courbe par fichier
for col in columns_to_plot:  
    plt.figure(figsize=(10, 6))  

    # Parcourir chaque fichier
    for file_path in file_paths:
        # Charger les données du fichier
        data = pd.read_csv(file_path, sep=",", header=0)
        
        data.columns = data.columns.str.strip()

        # Convertir la colonne 'Date' en datetime
        data["Date"] = pd.to_datetime(data["Date"], format="%B %d %Y %H:%M:%S", errors="coerce")
        data["Time"] = data["Date"]  # Garde les valeurs au format datetime
        date_for_title = data["Date"].dt.strftime("%B %d, %Y").iloc[0] if not data.empty else "Unknown Date"

        # Conversion des colonnes numériques
        numeric_columns = [col for col in data.columns if col not in ["Date", "Time"]]
        
        for c in numeric_columns:
            if c in data.columns:
                data[c] = pd.to_numeric(data[c], errors="coerce")
        
        # Suppression des lignes avec des valeurs manquantes
        data = data.dropna()
        
        # Filtrer les données après heure_min et avant heure_max
        heure_min = pd.to_datetime("09:50:00").time()
        heure_max = pd.to_datetime("19:30:00").time()

        data = data[data["Time"].dt.time >= heure_min]
        data = data[data["Time"].dt.time <= heure_max]

        # Vérifier si la colonne d'intérêt existe dans les données
        if col in data.columns:
            # Extraire le nom du fichier pour la légende
            file_name = os.path.basename(file_path)
            
            # Tracer les données du fichier
            plt.plot(data["Time"], data[col], label=file_name, linestyle='-', linewidth=1)  # Ligne pour chaque fichier

    # Titre et étiquettes
    plt.title(f"Évolution de {col}- {date_for_title}")
    plt.xlabel("Heure")
    plt.ylabel(col)

    # Ajuster les échelles de l'axe Y selon la colonne
    # if col == "Humidity (%)":
    #     plt.ylim(0, 100)
    # elif col == "Light (lux)":
    #     plt.ylim(0, 10000)
    # elif col == "Temperature (°C)":
    #     plt.ylim(-10, 35)
    # elif col == 'Soil Moisture (%)':
    #     plt.ylim(240, 3590)

    # Formater les graduations de l'axe des x
    ax = plt.gca()
    ax.xaxis.set_major_locator(mdates.HourLocator(interval=2))
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    plt.xticks(rotation=45)  # Rotation pour lisibilité
    plt.grid()

    # Ajouter la légende
    plt.legend()
    
    # Enregistrer et afficher le graphique
    plt.tight_layout()
    plt.savefig(f"{col.replace(' ', '_')}_superposition.png")  
    plt.show()
