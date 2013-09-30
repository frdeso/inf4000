inf4000
=======

# Facteurs discriminants
3 caractéristiques pouvant discriminer du trafic malveillant.
* Temps entre deux envois
* Taille des paquets
* Topologie des communications


##Temps entre deux envois
Les intervalles entre deux envois sur un réseau SCADA sont très réguliers. Les MTU utilise une méthode de pooling pour questionner les sondes un peu partout dans le réseau. Les MTU font des requêtes à des intervalles réguliers.

##Taille des paquets
Il y a 2 tailles de paquets où se retrouvent 80% des paquets sur le réseau. On pense que les deux tailles de paquets ont les fonctions suivantes:
* Requêtes du MTU et réponses des RTU
##Topologie des communications


#Glossaire
* MTU = Master Terminal Unit
* RTU = Remote Terminal Unit
* PLC = Progammable Logic Controller
* ICS = Industrial Control System
* SCADA = Supervisory Control And Data Acquisition
* DNP 3.0 = Distributed Network Protocol,  Its main use is in utilities such as electric and water companies. Usage in other industries is not common. It was developed for communications between various types of data acquisition and control equipment.
