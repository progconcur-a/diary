# Semaine 15/16

- [ ] GPU

## Processeur GPU

Un processeur GPU (Graphics Processing Unit) est un processeur spécialisé dans le traitement des données graphiques. Il est conçu pour effectuer des calculs parallèles massifs, ce qui le rend particulièrement adapté pour les tâches de rendu graphique, mais aussi pour d'autres applications nécessitant une grande puissance de calcul, comme l'apprentissage automatique et le traitement de données.

Un GPU NVidia est composé de :

- GPC (Graphics Processing Cluster) : Unité de traitement graphique
  - TPC (Texture Processing Cluster) : Unité de traitement de texture
    - SM (Streaming Multiprocessor) : Multiprocesseur de flux
      - Cores : Cœurs de calcul (typiquement 64 à 128 coeurs)
      - L1 Cache : Mémoire cache de niveau 1
      - Register File : Fichier de registres
      - Instruction Cache : Mémoire cache d'instructions
      - Shared Memory : Mémoire partagée
      - Tensor Cores (Cœurs Tensor) : Cœurs spécialisés pour les opérations de tenseurs
      - RT Cores (Cœurs de Ray Tracing) : Cœurs spécialisés pour le ray tracing
  - Raster Engine : Moteur de rastrisation
    - ROP (Raster Operations Pipeline) : Pipeline d'opérations de rastrisation
    - ROP Cache : Mémoire cache des opérations de rastrisation
  - Polymorph Engine : Moteur polymorphe
    - Texture Unit : Unité de texture

Voici la version **corrigée et structurée sous forme de liste à puces**, respectant votre format initial tout en intégrant les précisions nécessaires :

- GPC (Graphics Processing Cluster) : Groupe de traitement graphique
  - Raster Engine : Moteur de rastérisation
    - ROP (Raster Operations Pipeline) : Pipeline de traitement final des pixels (écriture framebuffer, blending, z-test…)
    - ROP Cache : Mémoire cache dédiée aux ROP
  - Polymorph Engine : Moteur géométrique programmable (vertex fetch, tessellation, viewport transform, etc.)
  - Plusieurs :
    - TPC (Texture Processing Cluster) : Groupe de traitement
      - Texture Units : Unités de traitement de textures (sampling, filtrage, etc.)
      - Deux :
        - SM (Streaming Multiprocessor) : Multiprocesseur de flux (unité d’exécution parallèle)
          - CUDA Cores : 64 à 128 Cœurs de calcul généralistes (SIMD, scalaires)
          - Tensor Cores : Cœurs spécialisés pour les produits de matrices (Deep Learning, FP16, TF32…)
          - RT Cores *(présents sur certaines architectures)-: Cœurs dédiés au ray tracing matériel (BVH traversal, intersection)
          - Register File : Banque de registres (privés par thread)
          - Shared Memory / L1 Cache : Mémoire partagée entre threads d’un même bloc (peut servir de cache L1 selon config)
          - Instruction Cache : Cache des instructions dispatchées aux warps
          - Warp Scheduler(s) : Planificateurs de warps (groupes de 32 threads)
          - Load/Store Units : Pour l’accès à la mémoire globale

### Tensor Cores

Un Tensor Core est un type de cœur de calcul spécialisé dans les opérations de tenseurs.

Pour mieux comprendre un tenseur c'est un tableau à N dimensions. En d'autre termes c'est une matrice multidimensionnelle. Par exemple :

$$ 128x3x64x64 $$

Sans accélération graphique le calcul de : $D = A \times B + C$ se calculerait :

```python
import random

N = 8  # Taille des matrices

A = [[random.random() for _ in range(N)] for _ in range(N)]
B = [[random.random() for _ in range(N)] for _ in range(N)]
C = [[random.random() for _ in range(N)] for _ in range(N)]

# Matrice résultat D (initialement vide)
D = [[0.0 for _ in range(N)] for _ in range(N)]

# Calcul de D = A × B + C
for i in range(N):             # ligne de A
    for j in range(N):         # colonne de B
        sum_product = 0.0
        for k in range(N):     # produit scalaire (A[i] et colonne B[k])
            sum_product += A[i][k] * B[k][j]
        D[i][j] = sum_product + C[i][j]

print("Résultat D = A × B + C :")
for row in D:
    print(["{:.2f}".format(x) for x in row])
```

Ici pour une matrice 8x8, on a 512 multiplications + 448 additions pour le produit et 64 additions finales.

Dans un Tensor Core 8x8, cela peut être fait en une seule opération. Le gain peut être d'un facteur de 100'000 par rapport à une exécution classique sur CPU.

Les tenseurs sont particulièrement utiles pour les opérations de Deep Learning, où les matrices sont souvent de grande taille et les opérations de multiplication matricielle sont fréquentes. C'est également utilisé en traitement d'image, en vision par ordinateur et en simulation physique (par exemple, pour les simulations de fluides ou de particules).

### RT Cores

Un RT Core est un cœur de calcul spécialisé dans le ray tracing matériel. Il est conçu pour accélérer les calculs liés au ray tracing, tels que la traversée de structures de données BVH (Bounding Volume Hierarchy) et les intersections de rayons avec des surfaces.

https://youtu.be/iOlehM5kNSk?si=6GoR4RM-5xbJ6WYk&t=1068

Il gère la descente dans l'arbre BVH, le test du rayon contre les triangles, et le calcul des intersections. Il est optimisé pour traiter de nombreux rayons en parallèle, ce qui permet d'accélérer considérablement le rendu d'images en ray tracing.

## Pipeline graphique

Le pipeline graphique est un ensemble d'étapes de traitement des données graphiques, depuis la création des modèles 3D jusqu'à l'affichage final sur l'écran. Il est divisé en plusieurs étapes principales :

1. Vertex Shader : Transforme les sommets des modèles 3D en coordonnées d'écran.
2. Tessellation : Divise les surfaces en polygones plus petits pour un rendu plus détaillé.
3. Geometry Shader : Génère de nouveaux sommets ou modifie les sommets existants.
4. Rasterization : Convertit les primitives géométriques en fragments (pixels).
5. Fragment Shader : Calcule la couleur de chaque fragment en fonction des textures, des lumières et des matériaux.
6. Output Merger : Combine les fragments pour produire l'image finale, en appliquant des opérations comme le blending et le z-test.
7. Display : Envoie l'image finale au framebuffer pour l'affichage à l'écran.

Exemple :

```bash
g++ -std=c++11 main.cpp -lglfw -lGLEW -lGL
```
