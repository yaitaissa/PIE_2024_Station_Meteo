import serial
import re
import os


# Configuration du port série
serial_port = "COM5"  
baud_rate = 9600  

# Fichier de sortie
output_file = "data_log_v2.txt"

# Initialisation du fichier avec les en-têtes (ajout unique si nécessaire)
if not os.path.exists(output_file):
    with open(output_file, "w") as file:
        file.write("Date\tHumidity (%)\tTemperature (°C)\tRainfall (session, mm)\tRainfall (24h, mm)\tLux\tSoil moisture\tSoil Humidity (%)\n")

# Initialisation du fichier avec les en-têtes 
# with open(output_file, "w") as file:
#     file.write("Date\tHumidity (%)\tTemperature (°C)\tRainfall (session, mm)\tRainfall (24h, mm)\tLux\tSoil moisture\tSoil Humidity (%)\n")

# Connexion au port série
try:
    ser = serial.Serial(serial_port, baud_rate)
    print(f"Lecture des données sur {serial_port}...")
except serial.SerialException as e:
    print(f"Erreur de connexion au port série : {e}")
    exit()

# Variables pour stocker les données partielles
data_buffer = {}


def process_and_save_data(data_buffer):
    """Enregistre les données si toutes les clés sont présentes dans le buffer."""
    required_keys = {"date_mesure", "humidity", "temperature", "rain_session", "rain_24h", "lux", "soil_moisture", "humidity_purcentage"}
    if required_keys.issubset(data_buffer.keys()):
        print(f"Données recueillies : {data_buffer}")  # Débogage
        with open(output_file, "a") as file:
            file.write(f"{data_buffer.get('date_mesure', 'N/A')}\t{data_buffer.get('humidity', 'N/A')}\t"
                       f"{data_buffer.get('temperature', 'N/A')}\t{data_buffer.get('rain_session', 'N/A')}\t"
                       f"{data_buffer.get('rain_24h', 'N/A')}\t{data_buffer.get('lux', 'N/A')}\t"
                       f"{data_buffer.get('soil_moisture', 'N/A')}\t{data_buffer.get('humidity_purcentage', 'N/A')}\n")
        data_buffer.clear()  # Réinitialise le buffer

# Boucle principale pour lire les données
try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        print(f"Ligne reçue : {line}")  # Affiche la ligne brute pour débogage

        # Extraction des données
        if "humidity:" in line:
            match = re.search(r"humidity:\s*([\d.]+)%", line)
            if match:
                data_buffer["humidity"] = match.group(1)

        elif "temperature:" in line:  
            match = re.search(r"temperature:\s*([\d.]+)", line)
            if match:
                data_buffer["temperature"] = match.group(1)

        elif "Quantité d'eau tombée pendant cette session:" in line:
            match = re.search(r"Quantité d'eau tombée pendant cette session:\s*([\d.]+)", line)
            if match:
                data_buffer["rain_session"] = match.group(1)

        elif "Quantité d'eau tombée dans les dernières 24h:" in line:
            match = re.search(r"Quantité d'eau tombée dans les dernières 24h:\s*([\d.]+)", line)
            if match:
                data_buffer["rain_24h"] = match.group(1)

        elif "Lux:" in line:
            match = re.search(r"Lux:\s*([\d.]+)", line)
            if match:
                data_buffer["lux"] = match.group(1)

        elif "Soil humidity:" in line:
            match = re.search(r"Soil humidity:\s*(\d+)", line)
            if match:
                data_buffer["soil_moisture"] = match.group(1)

        elif "Soil humidity purcentage:" in line:
            match = re.search(r"Soil humidity purcentage:\s*([\d.]+)", line)
            if match:
                data_buffer["humidity_purcentage"] = match.group(1)
                
        elif "2025" in line :
            match = re.search(r"(\w+ \d+ \d+ \d+:\d+:\d+)", line)  # Extrait la date/heure
            if match :
                data_buffer["date_mesure"] = match.group(1)

        elif "[" in line:  # Ligne contenant un timestamp (approximatif)
            match = re.search(r"\[(\d+)\]", line)
            if match:
                data_buffer["timestamp"] = f"Timestamp {match.group(1)}"

            
        
        # Vérifie si des données ont été collectées et les enregistre
        if "date_mesure" in data_buffer:  # Enregistre les données lorsqu'un timestamp est trouvé
            process_and_save_data(data_buffer)

except KeyboardInterrupt:
    print("\nArrêt du script.")
    ser.close()




# import serial
# import re

# # Configuration du port série
# SERIAL_PORT = "COM5"  # Remplacez par votre port série
# BAUD_RATE = 9600      # Définissez le débit en bauds
# OUTPUT_FILE = "donnees_capteurs.txt"

# def parse_data(line):
#     """
#     Parse une ligne de données et extrait les valeurs utiles.
#     Retourne un dictionnaire des données extraites.
#     """
#     try:
#         date_match = re.search(r"(\w+ \d+ \d+ \d+:\d+:\d+)", line)  # Extrait la date/heure
#         humidity_match = re.search(r"humidity:\s*([\d.]+)%", line)
#         temperature_match = re.search(r"temerature:\s*([\d.]+)", line)
#         water_session_match = re.search(r"Quantité d'eau tombée pendant cette session:\s*([\d.]+) mm", line)
#         water_24h_match = re.search(r"Quantité d'eau tombée dans les dernières 24h:\s*([\d.]+) mm", line)
#         lux_match = re.search(r"Lux:\s*([\d.]+)", line)
#         soil_humidity_match = re.search(r"Soil humidity:\s*([\d.]+)", line)
#         soil_humidity_percent_match = re.search(r"Soil humidity purcentage:\s*([\d.]+)", line)

#         # Créer un dictionnaire des données extraites
#         data = {
#             "Date": date_match.group(1) if date_match else None,
#             "Humidity (%)": humidity_match.group(1) if humidity_match else None,
#             "Temperature (°C)": temperature_match.group(1) if temperature_match else None,
#             "Rainfall (session, mm)": water_session_match.group(1) if water_session_match else None,
#             "Rainfall (24h, mm)": water_24h_match.group(1) if water_24h_match else None,
#             "Lux": lux_match.group(1) if lux_match else None,
#             "Soil Humidity": soil_humidity_match.group(1) if soil_humidity_match else None,
#             "Soil Humidity (%)": soil_humidity_percent_match.group(1) if soil_humidity_percent_match else None,
#         }

#         return data
#     except Exception as e:
#         print(f"Erreur lors du parsing : {e}")
#         return None

# def write_data_to_file(data):
#     """
#     Écrit les données formatées dans un fichier texte.
#     """
#     with open(OUTPUT_FILE, "a") as file:
#         # Vérifie si le fichier est vide pour ajouter une en-tête
#         if file.tell() == 0:
#             file.write("Date\tHumidity (%)\tTemperature (°C)\tRainfall (session, mm)\tRainfall (24h, mm)\tLux\tSoil Humidity\tSoil Humidity (%)\n")
#         # Écrit les données sous forme de ligne tabulée
#         file.write(f"{data['Date']}\t{data['Humidity (%)']}\t{data['Temperature (°C)']}\t{data['Rainfall (session, mm)']}\t{data['Rainfall (24h, mm)']}\t{data['Lux']}\t{data['Soil Humidity']}\t{data['Soil Humidity (%)']}\n")

# def main():
#     """
#     Boucle principale pour lire les données depuis le port série.
#     """
#     try:
#         # Ouvre la connexion série
#         with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
#             print("Connexion au port série ouverte.")
#             while True:
#                 # Lit une ligne de données
#                 line = ser.readline().decode("utf-8").strip()
#                 if line:
#                     print(f"Donnée reçue : {line}")
#                     data = parse_data(line)
#                     if data and data["Date"]:
#                         write_data_to_file(data)
#     except serial.SerialException as e:
#         print(f"Erreur série : {e}")
#     except KeyboardInterrupt:
#         print("\nArrêt du script.")
#     except Exception as e:
#         print(f"Erreur inattendue : {e}")

# if __name__ == "__main__":
#     main()
