"""
Script de visualisation des données environnementales d'une station (fichier txt de la carte SD)'.

Objectif :
- Générer des graphiques pour chaque variable mesurée.
- Afficher des statistiques (min, max, moyenne) sur chaque graphique.

Utilisation :
- Modifier `file_path` pour pointer vers le fichier à analyser.
- Vérifier les noms des colonnes d’intérêt et ajuster les limites d'axe y si besoin

Sortie :
- Un graphique par paramètre mesuré.

"""


import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

# Chargement des données de la carte SD
file_path = r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\01032025\aquacheck5.txt"  # à modifier
#file_path = r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\journee\sensor_data.txt"  # à modifier

data = pd.read_csv(file_path, sep=",", header=0)

# Nettoyage des noms de colonnes
data.columns = data.columns.str.strip()

# Conversion de la colonne Date
data["Date"] = pd.to_datetime(data["Date"], format="%B %d %Y %H:%M:%S", errors="coerce")
data["Time"] = data["Date"]  # copie

# Conversion des colonnes numériques
numeric_columns = [col for col in data.columns if col not in ["Date", "Time"]]
data[numeric_columns] = data[numeric_columns].apply(pd.to_numeric, errors="coerce")

# Suppression des lignes avec des valeurs manquantes
data = data.dropna()

# Extraire la date pour le titre du graphique
date_for_title = data["Date"].dt.strftime("%B %d, %Y").iloc[0] if not data.empty else "Unknown Date"

# Graphique des paramètres environnementaux observés
def plot_column(col):
    plt.figure(figsize=(10, 6))
    plt.scatter(data["Time"], data[col], label=col, color='blue', s=5)
    plt.title(f"Évolution de {col} - {date_for_title}")
    plt.xlabel("Heure")
    plt.ylabel(col)
    
    # Limites de l'axe Y selon la colonne
    y_limits = {
        "Humidity (%)": (0, 100),
        "Light (lux)": (0, 10000),
        "Temperature (°C)": (-10, 35),
        "Soil Moisture (%)": (240, 3590),
        "Humidity Purcentage (%)": (0, 100),
    }
    plt.ylim(y_limits.get(col, (None, None)))

    # Calcul des valeurs min, max et moyenne
    min_value, max_value, mean_value = data[col].min(), data[col].max(), data[col].mean()
    plt.text(0.95, 0.95, f"Min: {min_value:.2f}\nMax: {max_value:.2f}\nMoyenne: {mean_value:.2f}",
             horizontalalignment='right', verticalalignment='top', transform=plt.gca().transAxes,
             fontsize=10, color='black', bbox=dict(facecolor='white', alpha=0.7, edgecolor='black'))

    # Limites de l'axe X et graduation toutes les 2 heures
    ax = plt.gca()
    ax.set_xlim(pd.Timestamp(data["Time"].min().date()) + pd.Timedelta(hours=8),
                pd.Timestamp(data["Time"].min().date()) + pd.Timedelta(hours=20))
    ax.xaxis.set_major_locator(mdates.HourLocator(interval=2))
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    
    # Rotation des labels de l'axe X et ajout de la grille
    plt.xticks(rotation=45)
    plt.grid()
    plt.legend()
    plt.tight_layout()
    
    # Sauvegarde et affichage du graphique
    plt.savefig(f"{col.replace(' ', '_')}.png")
    plt.show()

# Tracer les graphiques pour toutes les colonnes numériques
for col in numeric_columns:
    plot_column(col)


