INF4000 - Abnormal behavior detector for SCADA networks
=======

#External Dependancies
* Libpcap
* C++11
* Boost

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
* Différents ACK provenant des RTU ou du MTU

##Topologie des communications
Dans un réseau de contrôle comme celui à l'étude le modèle de client-serveur ne tient pas. En effet, le modèle Maitre-esclave colle mieux à la topologie normal d'un réseau de contrôle. Le maitre demande des mises à jour de ses esclaves et ceux-ci lui répondent. Mis à part des situations extraordinaires les esclaves n'initient jamais une conversation avec le maitre

#Test de Kolmogorov-Smirno
[Test deux échantilons](http://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test#Two-sample_Kolmogorov.E2.80.93Smirnov_test)

#Implémentation
##packet
* int capture_id_; (Numéro de paquet depusi le début de la capture)
* uint64_t timestamp_ (Temps depuis le depuis de la capture)
* int ip_src
* int ip_dst
* int tcp_protocole
* u16 port
* size_t length_

##packet_capture
* 

#Glossaire
* MTU = Master Terminal Unit
* RTU = Remote Terminal Unit
* PLC = Progammable Logic Controller
* ICS = Industrial Control System
* SCADA = Supervisory Control And Data Acquisition
* DNP 3.0 = Distributed Network Protocol,  Its main use is in utilities such as electric and water companies. Usage in other industries is not common. It was developed for communications between various types of data acquisition and control equipment.
