"""
Script de comparaison des données de température et d'humidité relative entre un capteur ATH20 d'une station et la réanalyse ERA5 sur une journée.

Objectif :
- Comparer les températures et humidités relatives de l'air mesurées par un capteur ATH20 d'une station et celles de la réanalyse ERA5 pour un point géographique donné.
- Valider les données de nos capteurs ATH20

Utilisation :
- Modifier les chemins d'accès `file_path` et `ds` pour pointer vers les fichiers de données du capteur et de la réanalyse ERA5. Les données de réanalyse ERA5 sont à chercher ici : https://cds.climate.copernicus.eu/datasets/reanalysis-era5-single-levels?tab=download&fbclid=IwY2xjawIxomhleHRuA2FlbQIxMAABHQBYEvvMujalE_UqZRlPhcGc11NnMO8_Le-BVfZO0DzdPJQCT9EQkFjoNw_aem_OACl-kxBU0D32IK7xFk6hg

Sortie :
- Graphiques comparant les températures et humidités relatives mesurées et celles de la réanalyse ERA5.
- Un histogramme des écarts de température et d'humidité relative et des statistiques descriptives sur ces écarts.

"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import xarray as xr
import numpy as np

# Charger les données de l'analyse ERA5
ds = xr.open_dataset("C:/Users/lucie/Documents/SUPAERO4/PIE/analyse/journee/analysis_04022025_2.nc")

# Charger les données du capteur
file_path = r"C:\Users\lucie\Documents\SUPAERO4\PIE\analyse\journee\sensor_data.txt"
data = pd.read_csv(file_path, sep=",", header=0)

# Nettoyer et convertir les colonnes
data.columns = data.columns.str.strip()
data["Date"] = pd.to_datetime(data["Date"], format="%B %d %Y %H:%M:%S", errors="coerce")
data.dropna(inplace=True)

# Sélectionner la température et l'humidité du capteur
data_sensor = data[["Date", "Temperature (°C)", "Humidity (%)"]].dropna()

# Sélectionner les données ERA5 pour le point (43.56°N, 1.47°E) et convertir en °C
t2m = ds.t2m.sel(latitude=43.56, longitude=1.47, method="nearest") - 273.15
d2m = ds.d2m.sel(latitude=43.56, longitude=1.47, method="nearest") - 273.15

# Calcul de l'humidité relative ERA5
e_s = 6.109 * np.exp((17.625 * t2m) / (243.04 + t2m))   # Pression de vapeur saturante (hPa)
e = 6.109 * np.exp((17.625 * d2m) / (243.04 + d2m))   # Pression de vapeur réelle (hPa)
RH_era5 = (e / e_s) * 100  # Humidité relative en %
#RH_era5 = 100-5*(T-Td) #formule simplifiée

# Créer un DataFrame avec les données ERA5
df_nc = pd.DataFrame({
    "Date": pd.to_datetime(ds.valid_time.values).tz_localize('UTC').tz_convert('Europe/Paris'),
    "Temperature_NC (°C)": t2m.values,
    "Humidity_NC (%)": RH_era5.values
})

# Convertir les fuseaux horaires pour aligner les deux séries
data_sensor["Date"] = pd.to_datetime(data_sensor["Date"]).dt.tz_localize('Europe/Paris')

# Graphiques - Température
plt.figure(figsize=(10, 6))
plt.scatter(data_sensor["Date"], data_sensor["Temperature (°C)"], label="Capteur", color='blue', s=5)
plt.plot(df_nc["Date"], df_nc["Temperature_NC (°C)"], label="ERA5 t2m", color='red', linestyle='-')
plt.xlabel("Heure")
plt.ylabel("Température (°C)")
plt.title("Évolution de la température (Capteur vs réanalyse ERA5)")
plt.legend()
plt.grid()
plt.xticks(rotation=45)
ax = plt.gca()
ax.xaxis.set_major_locator(mdates.HourLocator(interval=2))
ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
plt.tight_layout()
plt.show()

# Graphiques - Humidité relative
plt.figure(figsize=(10, 6))
plt.scatter(data_sensor["Date"], data_sensor["Humidity (%)"]*100, label="Capteur", color='blue', s=5)
plt.plot(df_nc["Date"], df_nc["Humidity_NC (%)"], label="ERA5 RH", color='red', linestyle='-')
plt.xlabel("Heure")
plt.ylabel("Humidité relative (%)")
plt.title("Évolution de l'humidité relative (Capteur vs réanalyse ERA5)")
plt.legend()
plt.grid()
plt.ylim(0, 100)
plt.xticks(rotation=45)
ax = plt.gca()
ax.xaxis.set_major_locator(mdates.HourLocator(interval=2)) #graduations toutes les 2 heures
ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
plt.tight_layout()
plt.show()

# Fusionner les données capteur et ERA5
df_comparaison = pd.merge_asof(df_nc.sort_values('Date'), 
                               data_sensor.sort_values('Date'), 
                               on="Date", 
                               direction="nearest")

# Calcul des écarts
df_comparaison["Écart Température (°C)"] = df_comparaison["Temperature (°C)"] - df_comparaison["Temperature_NC (°C)"]
df_comparaison["Écart Humidité (%)"] = df_comparaison["Humidity (%)"]*100 - df_comparaison["Humidity_NC (%)"]

# Afficher les statistiques des écarts
print("Statistiques des écarts de température :")
print(df_comparaison["Écart Température (°C)"].describe())

print("\nStatistiques des écarts d'humidité relative :")
print(df_comparaison["Écart Humidité (%)"].describe())

# Histogramme des écarts de température
plt.figure(figsize=(10, 6))
plt.hist(df_comparaison["Écart Température (°C)"], bins=20, color='green', edgecolor='black')
plt.title("Histogramme des écarts de température (Capteur vs ERA5)")
plt.xlabel("Écart (°C)")
plt.ylabel("Fréquence")
plt.grid(True)
plt.tight_layout()
plt.show()

# Histogramme des écarts d'humidité relative
plt.figure(figsize=(10, 6))
plt.hist(df_comparaison["Écart Humidité (%)"], bins=20, color='purple', edgecolor='black')
plt.title("Histogramme des écarts d'humidité relative (Capteur vs ERA5)")
plt.xlabel("Écart (%)")
plt.ylabel("Fréquence")
plt.grid(True)
plt.tight_layout()
plt.show()

