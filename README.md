## Projet GreenGuard et Aquacheck

### Context

### Objectifs

### Fonctionnnement hors ligne

Lors du setup, la station fait une première connection pour initialiser la bibliothèque *time.h*. Lors de cet étape, une connection réseau 4G est nécessaire. 

Lors du fonctionnement en continue, la station tente de récuperer la date en ligne. En cas d'échec, elle se rabat sur la fonction *time()*. Une connection réseau n'est donc pas forcément nécessaire sur cette partie. 

### Utilisation

Seul trois variables de la bibliothèque *global.h* ont besoin d'être modifié 

La variable **_ATH20_VERSION_** concerne le type de capteur ATH20 embarqué sur la station. Si le capteur est un capteur **ATH20 SEN0528**, mettre la variable à 1, si le capteur est un capteur **Grove 101990644**, mettre la variable à 0.

La variable **_GREENGUARD_** concerne le type de station sur laquelle le code sera embarqué. Si la station est de la flotte **GREENGUARD**, la variable doit être mise à 1. Si la station est de la flotte **AQUACHECK**, la variable doit être mise à 0.

La variable **_PSEUDO_PERIOD_** représente la période des mesures. La valeur de cette variable est le temps (en seconde) que va attendre la carte entre chaque mesures.