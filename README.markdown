<h1 id="titre">Parkayapravesh</h1>
<h2 id="slogan">It really whips the Dalai Lama's ass !</h2>

[Copie d'écran](http://jonathan.aceituno.emi.u-bordeaux1.fr/rvip1.png) •
[Vidéo de démonstration](http://www.youtube.com/watch?v=hp9j6HuhVC4)

# Introduction

Parkayapravesh est le projet de l'UE **Réalité virtuelle, interaction et perception** (RVIP) du Master 2 ISM de l'université Bordeaux 1, réalisé par :

* Jonathan Aceituno
* Laetitia Letoupin
* Yvain Tisserand

## Objectif général du projet

Le logiciel réalisé doit permettre d'aller chercher des objets dans la scène 3D et les positionnner sur l'établi pour reconstruire un assemblage donné en exemple. Ceci implique de définir des techniques d'interaction pour naviguer, sélectionner, manipuler et contrôler le système.

Le logiciel sera exécuté sur une table tactile disposant de deux points de contact. Un seul objet doit être transporté à la fois, et le logiciel ne doit pas déduire de la scène des connaissances particulières qui permettraient de _tricher_.

Les techniques d'interactions proposées doivent être pertinentes, innovantes, leur utilisation doit être facile et rapide.

## Contexte

Les travaux dirigés de l'UE RVIP ont permis de maîtriser la notion de graphe de scène à travers l'utilisation du langage [VRML](http://fr.wikipedia.org/wiki/Virtual_Reality_Markup_Language) et de la bibliothèque logicielle [OpenSG](http://www.opensg.org).

L'environnement 3D décrit par un graphe de scène doit être visité et manipulé ici grâce une surface plane tactile, ce qui présente de nombreux problèmes.

## Données techniques

Le logiciel est écrit en **C++**. Les bibliothèques utilisées sont les suivantes :

* [OpenGL](http://www.opengl.org) et **GLUT** pour l'affichage des graphismes en 3D
* [OpenSG](http://www.opensg.org) 1.8 pour l'interprétation et la manipulation des graphes de scène
* [TUIO](http://www.tuio.org) pour l'acquisition des gestes multitouch indépendamment du périphérique d'entrée

Le logiciel fonctionne sous les environnements *Windows*, *GNU/Linux* et *Mac OS X*.

# Parkayapravesh

## Origine du nom

Les croyances de l'Inde et certaines traditions bouddhistes nous enseignent que certains religieux, comme les sadhus, parviennent à obtenir certains pouvoirs ou *siddhis* comme la lévitation, l'ubiquité ou la capacité de vivre sans manger. Si ces siddhis sont le résultat d'une quête spirituelle intense, il est possible de profiter de quelques-uns d'entre eux sans effort particulier dans un environnement virtuel. Notre projet propose de profiter de quelques siddhis afin de faciliter la réalisation des tâches demandées, en plus des possibilités naturelles d'action auxquelles l'utilisateur, en tant qu'être humain, peut penser.

* **Parkaya pravesh** : la possibilité d'entrer dans un autre corps (_incarnation_)
* **Vayu gaman siddhi** : la possibilité de se déplacer d'un endroit à un autre en quelques secondes (_wayfinding_)
* **Dura-darsanam** : la possibilité de tout voir (_vue de l'objet incarné_)
* **Laghima** : la possibilité de faire léviter quelque chose (_manipulation des objets à distance_) ou de léviter soi-même (_déplacement sans influence de la gravité_)

## Utilisation

En dehors de l'utilisation classique du logiciel avec une table tactile, il est possible de l'essayer avec du matériel plus conventionnel. Il existe des dispositifs qui permettent de simuler l'utilisation d'un périphérique multitouch :

* MTMultiMouse.exe (inclus) est utilisable sous Windows, avec deux souris
* [Tongseng](http://github.com/fajran/tongseng) permet d'utiliser le trackpad multitouch des ordinateurs Apple

## Compilation

### Librairies requises

* OpenGL
* GLUT
* OpenSG 1.8

### Windows

Aller dans le répertoire libTUIO, lancer la solution TUIO_CPP.sln, puis copier Release/libTUIO.lib dans le répertoire libTUIO.

Lancer la solution win32/ProjetVS/OpenSGTUIO.sln sous Visual Studio 2010 (ou transférer la solution à des versions antérieures grâce à un outil quelconque, à vos risques et périls). Générer la solution dans la configuration _Release_, puis lancer OpenSGTUIO.exe depuis le répertoire racine du projet. Testé sur un ordinateur sous Windows 7.

Attention cependant, les images utilisées sont au format PNG, or la version d'OpenSG que vous avez ne supporte peut-être pas ce format d'image, surtout sous Windows. En conséquence, certaines images manqueront (notamment la petite croix pour fermer, en bas à gauche).

### GNU/Linux et Mac OS X

Se placer dans le répertoire du projet. Exécuter les commandes suivantes :

	$ cd libTUIO
	$ make
	$ cd ..
	$ make
	$ ./parkayapravesh

# Techniques d'interaction proposées

Notre proposition s'appuie sur un ensemble de techniques d'interaction choisi pour garantir un temps d'apprentissage court et une certaine rapidité d'utilisation. Nous pensons arriver à ces fins en minimisant le vocabulaire gestuel et en maximisant le champ sémantique des gestes.

Ainsi, nous définissons des concepts qui ont pour vocation de préciser le contexte, des gestes de mouvement, communs à différents éléments choisis en fonction du contexte, des gestes de modification du contexte et un geste avancé.

## Concepts précisant le contexte

Le contexte dans lequel sont effectués les gestes de mouvement est défini par deux concepts : la sélection et l'incarnation. Chacun est lié à zéro ou un objet particulier de la scène.

### Sélection

La sélection permet de préciser l'objet concerné par certaines actions. Ce concept est très fréquemment rencontré dans les interfaces graphiques et ne suppose généralement aucune prédisposition culturelle particulière chez l'utilisateur.

L'objet sélectionné est mis en évidence visuellement grâce à une _bounding-box_.

### Incarnation

L'incarnation est un autre moyen de préciser l'objet concerné par certaines actions, indépendamment de la sélection. Le principe est qu'en s'incarnant en un objet, l'utilisateur voit le monde virtuel du point de vue de cet objet et, en se déplaçant et s'orientant, déplace et oriente l'objet en question. Nous pensons qu'il est possible d'utiliser cette idée dans d'autres contextes d'usage.

Différentes cultures de par le monde présentent souvent des concepts très similaires à l'incarnation dans le folklore, la religion ou la spiritualité, ainsi nous pensons que l'acquisition de cette notion peut se faire très rapidement pour une majorité d'utilisateurs.

Lorsque l'utilisateur est en train d'incarner un objet, une petite vue secondaire sur l'environnement virtuel est affichée en bas à droite de l'écran. Elle suit l'objet actuellement incarné et le présente à la troisième personne. Lors de la désincarnation, cette vue secondaire devient la vue principale et disparaît.

## Gestes de mouvement

Nous définissons les gestes de mouvement comme ceux permettant le contrôle du déplacement et de l'orientation. Nous avons limité notre choix à des gestes permettant le déplacement et l'orientation selon cinq degrés de liberté, ce qui nous paraît suffisant au regard des différentes tâches à réaliser.

Les gestes de mouvement sont initiés à la pose d'un ou deux doigts et terminés lorsque tous les doigts sont retirés de la surface tactile.

Nous avons ajouté aux mouvements la notion d'inertie, similaire à ce qui est fait dans certains smartphones ou interfaces graphiques pour le défilement, afin de permettre un plus grand contrôle et une plus grande étendue de mouvement. Le relâchement d'un geste de mouvement avec une grande vitesse instantanée entraîne la continuation amortie du mouvement pendant un court temps. Si un nouveau geste de mouvement est amorcé avant la fin de l'amortissement, ce dernier est arrêté.


### Orientation

Le mouvement d'un seul doigt sur la surface tactile permet d'effectuer une rotation sur deux degrés de liberté, selon les axes transversal et vertical. L'axe longitudinal n'est jamais utilisé pour simplifier l'utilisation.

### Déplacement

Le mouvement de deux doigts sur la surface tactile permet d'effectuer une translation sur trois degrés de liberté.

Lorsque les deux doigts restent parallèles, la translation est réalisée selon les axes transversal et vertical. Le rapprochement ou l'éloignement des deux doigts permet la translation selon l'axe longitudinal.

Ainsi il est possible d'effectuer des déplacements selon les trois axes en utilisant la même gestuelle.

### Passage de l'orientation au déplacement

Lorsqu'une rotation est en cours, poser un autre doigt permet de continuer le mouvement par une translation. Lorsqu'une translation est en cours, retirer un doigt permet de continuer le mouvement par une rotation.

Nous pensons qu'ainsi il est possible de développer rapidement une certaine aisance dans le contrôle du déplacement et de l'orientation.

### Contextes d'utilisation

Les gestes de mouvement sont utilisables dans deux situations qui ont des sens différents mais triviaux et compréhensibles pour l'utilisateur.

#### Déplacement et orientation de la sélection

Lorsque le(s) doigt(s) initie(nt) le geste sur l'objet actuellement sélectionné, alors les gestes de mouvement concernent cet objet seulement. Le mouvement est effectué _de l'extérieur_ de l'objet concerné.

#### Déplacement et orientation de la vue de l'utilisateur

Dans tous les autres cas, les gestes de mouvement concernent la vue de l'utilisateur. Lorsqu'aucune incarnation n'est en cours, la vue de l'utilisateur peut être considérée comme une camera virtuelle naviguant dans l'environnement. Si l'utilisateur est en train d'incarner un objet, alors la vue de l'utilisateur est liée à la position et à l'orientation de l'objet incarné.

## Gestes de changement de contexte

### Sélection

Le clic d'un doigt sur un objet permet de le sélectionner.

### Incarnation

Un appui long d'un doigt sur la surface tactile permet d'incarner l'objet actuellement sélectionné.

Le temps nécessaire pour compléter le geste est indiqué visuellement par un effet de _motion blur_ et un déplacement automatique et progressif de la vue de l'utilisateur.

Retirer le doigt avant que le temps soit écoulé annule l'opération et l'indique par le même effet avec un retour instantané de la vue de l'utilisateur à son point de départ.

#### Désincarnation

Le clic d'un doigt sur la vue secondaire qui présente l'objet actuellement incarné, lorsqu'une incarnation est en cours, permet d'arrêter l'incarnation.

## Un geste avancé : le _wayfinding_

La rapidité de la navigation étant déterminante, nous avons rendu possible la navigation automatique vers un point donné de l'environnement virtuel. Un double appui court sur un point de la surface permet de naviguer automatiquement jusqu'à arriver devant le point 3D visé, c'est-à-dire le point le plus proche de la vue de l'utilisateur situé dans la direction donnée par le point de la surface touché par le doigt.

Nous pensons que ce geste avancé reste naturel et facile à maîtriser, tout en permettant aux utilisateurs plus expérimentés de parvenir à une plus grande vitesse d'exécution de leurs tâches.