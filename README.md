# TP Bus et Réseaux de Jiangbo WANG et KaiXUAN JIANG

## Introduction

Dans ce TP, nous utiliserons un STM32 et un Raspberry Pi pour mettre en place un système de surveillance de la température et de la pression. Le STM32 recueillera les données de température et de pression, puis les enverra au Raspberry Pi via le protocole UART. Le Raspberry Pi utilisera le framework Flask pour mettre en place un serveur web, et finalement, nous utiliserons ce serveur pour visualiser les données.
En même temps, nous utiliserons l'API REST pour effectuer des opérations CRUD (création, lecture, mise à jour, suppression) sur les données de température et de pression.
Enfin, pour apprendre à utiliser le bus CAN, nous enverrons des données d'échelle du serveur web du Raspberry Pi au STM32. Le STM32 convertira ces données en données angulaires, qu'il enverra ensuite à un moteur pas à pas via le bus CAN. Le moteur pas à pas tournera selon cet angle, permettant ainsi de contrôler le mouvement du moteur pas à pas avec les données de température. Voici le schéma du système :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/other/TPobjuctive.png)

## Communication Protocol

Le protocole de communication entre le Raspberry et le STM32 est comme suit :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/other/protocol.png)

## Serveur Web avec Flask

### Page d'accueil

Lorsque vous saisissez http://172.20.10.13:5000/ dans votre navigateur, Flask renvoie le contenu de index.html, comme illustré ci-dessous :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/index.png)

### Affichage des données

En cliquant sur le bouton "show data", vous verrez l'interface illustrée ci-dessous. Cette interface affichera en temps réel les données de température et de pression. La page web enverra automatiquement toutes les secondes des requêtes "GET_T" et "GET_P" au STM32 pour récupérer les dernières données de température et de pression, qui seront ensuite affichées sur la page web.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/showGraph.png)

## Test de l'API REST

Nous utilisons le plugin CHROME RESTED pour tester l'API REST.

### Récupération des dernières données de température

En saisissant http://172.20.10.13:5000/temp/ dans RESTED, en choisissant POST, puis en cliquant sur send, Flask envoie une requête "GET_T" au STM32, récupère les dernières données de température et renvoie les données au format JSON, comme illustré ci-dessous. Ces données sont également stockées dans une liste Python.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getNewTemp.png)

### Récupération de toutes les données de température

En saisissant http://172.20.10.13:5000/temp/ dans RESTED, en choisissant GET, puis en cliquant sur send, Flask renvoie toutes les données de température stockées dans la liste Python au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAllTemp.png)

### Récupération de la température à l'indice x

En saisissant http://172.20.10.13:5000/temp/<index> dans RESTED, en choisissant GET, puis en cliquant sur send, Flask renvoie la donnée de température à l'indice spécifié stockée dans la liste Python au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getTempIndex.png)

### Récupération des dernières données de pression

En saisissant http://172.20.10.13:5000/pres/ dans RESTED, en choisissant POST, puis en cliquant sur send, Flask envoie une requête "GET_P" au STM32, récupère les dernières données de pression et renvoie les données au format JSON, comme illustré ci-dessous. Ces données sont également stockées dans une liste Python.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getNewPres.png)

### Récupération de toutes les données de pression

En saisissant http://172.20.10.13:5000/pres/ dans RESTED, en choisissant GET, puis en cliquant sur send, Flask renvoie toutes les données de pression stockées dans la liste Python au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAllPres.png)

### Récupération de la pression à l'indice x

En saisissant http://172.20.10.13:5000/pres/<index> dans RESTED, en choisissant GET, puis en cliquant sur send, Flask renvoie la donnée de pression à l'indice spécifié stockée dans la liste Python au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getPresIndex.png)

### Réglage de l'échelle

En saisissant http://172.20.10.13:5000/scale/ dans RESTED, en choisissant POST, puis en cliquant sur send, Flask envoie une requête "SET_T" au STM32 pour définir les nouvelles données d'échelle et renvoie les données au format JSON, comme illustré ci-dessous. Ces données sont également stockées dans une liste Python.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/setScale.png)

### Récupération de l'échelle

En saisissant http://172.20.10.13:5000/scale/ dans RESTED, en choisissant GET, puis en cliquant sur send, Flask renvoie les données d'échelle stockées les plus récentes au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getScale.png)

### Récupération de l'angle

En saisissant http://172.20.10.13:5000/angle/ dans RESTED, en choisissant GET, puis en cliquant sur send, Flask envoie une requête "GET_A" au STM32, récupère les dernières données d'angle et renvoie les données au format JSON, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAngle.png)

### Suppression de la température à l'indice

En saisissant http://172.20.10.13:5000/temp/<index> dans RESTED, en choisissant DELETE, puis en cliquant sur send, Flask supprime la donnée de température à l'indice spécifié dans la liste Python et renvoie l'indice supprimé, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/deleteTempIndex.png)

### Suppression de la pression à l'indice

En saisissant http://172.20.10.13:5000/pres/<index> dans RESTED, en choisissant DELETE, puis en cliquant sur send, Flask supprime la donnée de pression à l'indice spécifié dans la liste Python et renvoie l'indice supprimé, comme illustré ci-dessous.
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/deletePresIndex.png)

## STM32 : Résultat du Protocole

### Récupération de la donnée de capteur de bmp280

### GET_T

Lorsque le STM32 reçoit une requête GET_T envoyée par le Raspberry Pi, il envoie une demande au bmp280 pour obtenir les dernières données de température, puis envoie ces données au Raspberry Pi, comme illustré ci-dessous :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/stm32Response/GET_T.png)

### GET_P

Lorsque le STM32 reçoit une requête GET_P envoyée par le Raspberry Pi, il envoie une demande au bmp280 pour obtenir les dernières données de pression, puis envoie ces données au Raspberry Pi, comme illustré ci-dessous :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/stm32Response/GET_P.png)

### Commande du moteur pas à pas

### SET_K

Lorsque le STM32 reçoit une requête SET_K envoyée par le Raspberry Pi, il définit un nouveau coefficient de proportionnalité. Il multiplie ensuite ce coefficient par la différence des dernières variations de température pour obtenir une valeur angulaire, qu'il envoie ensuite au moteur pas à pas. La requête SET_K est illustrée ci-dessous :

![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/stm32Response/SET_K.jpg)

### GET_K

Lorsque le STM32 reçoit une requête GET_K envoyée par le Raspberry Pi, il récupère les dernières données angulaires et les envoie au Raspberry Pi. La requête GET_K est illustrée ci-dessous :

![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/stm32Response/GET_K.jpg)

### GET_A

Lorsque le STM32 reçoit une requête GET_A envoyée par le Raspberry Pi, il récupère les dernières données angulaires stockées et les envoie au Raspberry Pi, comme illustré ci-dessous :
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/stm32Response/GET_A.png)

## Conclusion

Dans ce TP, nous avons appris à utiliser un STM32 et un Raspberry Pi pour mettre en place un système de surveillance de la température et de la pression.
Plus précisément :

- Nous avons appris le protocole I2C, utilisé pour la communication entre le STM32 et le capteur de \* \* \* \* température (bmp280), et comment écrire un pilote de capteur pour I2C.
- Nous avons appris le protocole UART, utilisé pour la communication entre le STM32 et le Raspberry Pi.
- Nous avons appris le protocole CAN, utilisé pour la communication entre le STM32 et le moteur pas à pas.
- Nous avons appris le framework Flask, utilisé pour mettre en place un serveur web.
- Nous avons appris l'API REST, utilisée pour réaliser des opérations CRUD sur les données de température et de pression.
