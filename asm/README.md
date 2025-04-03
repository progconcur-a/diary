# Micro architecture

On s'intéresse à une micro architecture pédagogique. Le but est de comprendre les bases de l'architecture d'un processeur. On va donc définir un jeu d'instruction minimal et une architecture minimaliste.

## Set d'instruction minimal

| Instruction | Description                                 | Exemple          |
| ----------- | ------------------------------------------- | ---------------- |
| `MOV`       | Copie la valeur d'un registre dans un autre | `MOV R1, R2`     |
| `MOV`       | Copie immédiate dans un registre            | `MOV R1, 42`     |
| `ADD`       | Additionne deux registres                   | `ADD R1, R2`     |
| `SUB`       | Soustrait deux registres                    | `SUB R1, R2`     |
| `AND`       | ET bit à bit entre deux resitres            | `AND R1, R2`     |
| `OR`        | OU bit à bit entre deux registres           | `OR R1, R2`      |
| `XOR`       | OU exclusif bit à bit entre deux registres  | `XOR R1, R2`     |
| `NOT`       | Inverse les bits d'un registre              | `NOT R1`         |
| `MUL`       | Multiplie deux registres                    | `MUL R1, R2`     |
| `JMP`       | Saut inconditionnel                         | `JMP 0x00`       |
| `JZ`        | Saut si zéro                                | `JZ 0x00`        |
| `JNZ`       | Saut si non zéro                            | `JNZ 0x00`       |
| `LOAD`      | Charge une valeur dans un registre          | `LOAD R1, 0x00`  |
| `STORE`     | Stocke une valeur dans la mémoire           | `STORE 0x00, R1` |
| `PUSH`      | Empile une valeur dans la pile              | `PUSH R1`        |
| `POP`       | Dépile une valeur de la pile                | `POP R1`         |
| `CALL`      | Appelle une fonction                        | `CALL 0x00`      |

## Registres

Ce processeur minimal à 8 registres de 32-bits chacun `R0` à `R7`.

## Appel de fonction

Les paramètres utilisent `R2`, `R4` et `R6`. La valeur de retour est `R7`.

## Entrées sorties

La mémoire est sur 16-bits de 0x0000 à 0xFFFF. L'adresse 0xFFF0 est l'entrée standard et l'adresse 0xFFF1 est la sortie standard. Pour afficher `hello` sur la sortie standard on écrit :

```asm
MOV R1, 104
STORE 0xFFF1, R1
MOV R1, 101
STORE 0xFFF1, R1
STORE 0xFFF1, R1
MOV R1, 111
STORE 0xFFF1, R1
MOV R1, 0
STORE 0xFFF1, R1
```

## Exercice 1 : Division Euclidienne

Quel serait l'algorithme pour faire une division euclidienne en assembleur ? Essayez d'imaginer un pseudo-code en C et tester votre algorithme. Ensuite essayez de le traduire en assembleur.

## Exercice 2 : PGCD

Implémenter en assembleur la fonction `int pgcd(int a, int b)`, utiliser la division euclidienne précédente.

## Notes

Contrat: si je rentre dans une section de code "macro"
R1..R3 sous la responsabilité de l'appelant de sauver les registres
R4..R6 sous la responsabilité de l'appelé de sauver les registres

% Unused registers R1, R3, R7
% R6, value of fuel in engine
% R5, remaining oxygen tank level
LOAD R1, 0xff32

% /!\ Used registed R1

### POP PUSH

```asm

    push R3
    call algorithm
    pop R3

algorithm:
    push R6
    % J'ai besoin de R3, R6

    pop R6
    ret
```