## Projet GreenGuard et Aquacheck

### Context

Neusta France souhaite contribuer à la recherche et au développement en proposant son expertise en architecture cloud et transfert/stockage de données pour monter un projet PIE avec l'ISAE. L'objectif du projet GreenGuard est de développer un démonstrateur d'un cas d'usage écologique et académique en mettant en place une infrastructure de surveillance environnementale avec des capteurs connectés et un réseau maillé.

### Objectifs

Le projet GreenGuard vise à développer une infrastructure de surveillance environnementale avancée sur le campus universitaire pour les besoins suivants :

Suivre les conditions météorologiques pour faciliter la planification des activités extérieures.
Contrôler l'éclairage pour optimiser l'utilisation de la lumière naturelle.
Gérer les ressources en eau pour une utilisation optimale.

### Fonctionnnement hors ligne

Lors du setup, la station fait une première connection pour initialiser la bibliothèque *time.h*. Lors de cet étape, une connection réseau 4G est nécessaire. 

Lors du fonctionnement en continue, la station tente de récuperer la date en ligne. En cas d'échec, elle se rabat sur la fonction *time()*. Une connection réseau n'est donc pas forcément nécessaire sur cette partie. 

### Utilisation

Seul trois variables de la bibliothèque *global.h* ont besoin d'être modifié 

La variable **_ATH20_VERSION_** concerne le type de capteur ATH20 embarqué sur la station. Si le capteur est un capteur **ATH20 SEN0528**, mettre la variable à 1, si le capteur est un capteur **Grove 101990644**, mettre la variable à 0.

La variable **_GREENGUARD_** concerne le type de station sur laquelle le code sera embarqué. Si la station est de la flotte **GREENGUARD**, la variable doit être mise à 1. Si la station est de la flotte **AQUACHECK**, la variable doit être mise à 0.

La variable **_PSEUDO_PERIOD_** représente la période des mesures. La valeur de cette variable est le temps (en seconde) que va attendre la carte entre chaque mesures.