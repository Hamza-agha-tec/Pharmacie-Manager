#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition des structures
typedef struct Ordonnance {
    int numero;
    char date[20];
    struct Ordonnance *suivant;
} Ordonnance;

typedef struct Medicament {
    char nom[50];
    float prix;
    int quantite;
    struct Medicament *suivant;
    Ordonnance *ordonnances;
} Medicament;

typedef struct {
    char nom[50];
    char adresse[100];
    int telephone;
    Medicament *medicaments;
} Pharmacie;

// Définition des fonctions
void ajouter_medicament(Pharmacie *pharmacie, Medicament *nouveau_medicament) {
    nouveau_medicament->suivant = pharmacie->medicaments;
    pharmacie->medicaments = nouveau_medicament;
}

void supprimer_medicament(Pharmacie *pharmacie, char *nom_medicament) {
    Medicament *actuel = pharmacie->medicaments;
    Medicament *precedent = NULL;
    while (actuel != NULL && strcmp(actuel->nom, nom_medicament) != 0) {
        precedent = actuel;
        actuel = actuel->suivant;
    }
    if (actuel != NULL) {
        if (precedent == NULL) {
            pharmacie->medicaments = actuel->suivant;
        } else {
            precedent->suivant = actuel->suivant;
        }
        // Libération des ordonnances associées au médicament supprimé
        Ordonnance *ordonnance_actuelle = actuel->ordonnances;
        while (ordonnance_actuelle != NULL) {
            Ordonnance *ordonnance_a_supprimer = ordonnance_actuelle;
            ordonnance_actuelle = ordonnance_actuelle->suivant;
            free(ordonnance_a_supprimer);
        }
        free(actuel);
    }
}

void ajouter_ordonnance(Medicament *medicament, Ordonnance *nouvelle_ordonnance) {
    nouvelle_ordonnance->suivant = medicament->ordonnances;
    medicament->ordonnances = nouvelle_ordonnance;
}

void supprimer_ordonnance(Medicament *medicament, int numero_ordonnance) {
    Ordonnance *actuelle = medicament->ordonnances;
    Ordonnance *precedente = NULL;
    while (actuelle != NULL && actuelle->numero != numero_ordonnance) {
        precedente = actuelle;
        actuelle = actuelle->suivant;
    }
    if (actuelle != NULL) {
        if (precedente == NULL) {
            medicament->ordonnances = actuelle->suivant;
        } else {
            precedente->suivant = actuelle->suivant;
        }
        free(actuelle);
    }
}

void afficher_medicaments(Pharmacie *pharmacie) {
    Medicament *actuel = pharmacie->medicaments;
    while (actuel != NULL) {
        printf("Nom: %s, Prix: %.2f, Quantite: %d\n", actuel->nom, actuel->prix, actuel->quantite);
        actuel = actuel->suivant;
    }
}

void afficher_ordonnances(Medicament *medicament) {
    Ordonnance *actuelle = medicament->ordonnances;
    while (actuelle != NULL) {
        printf("Numero: %d, Date: %s\n", actuelle->numero, actuelle->date);
        actuelle = actuelle->suivant;
    }
}

void sauvegarder_fichier(Pharmacie *pharmacie, char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return;
    }
    Medicament *medicament = pharmacie->medicaments;
    while (medicament != NULL) {
        fprintf(fichier, "%s %.2f %d\n", medicament->nom, medicament->prix, medicament->quantite);
        Ordonnance *ordonnance = medicament->ordonnances;
        while (ordonnance != NULL) {
            fprintf(fichier, "%d %s\n", ordonnance->numero, ordonnance->date);
            ordonnance = ordonnance->suivant;
        }
        // Indiquer la fin des ordonnances pour ce médicament
        fprintf(fichier, "-1\n");
        medicament = medicament->suivant;
    }
    // Indiquer la fin des médicaments
    fprintf(fichier, "-1\n");
    fclose(fichier);
}

void charger_fichier(Pharmacie *pharmacie, char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return;
    }
    pharmacie->medicaments = NULL;
    Medicament *precedent_medicament = NULL;
    while (1) {
        Medicament *nouveau_medicament = (Medicament *)malloc(sizeof(Medicament));
        if (fscanf(fichier, "%49s %f %d", nouveau_medicament->nom, &nouveau_medicament->prix, &nouveau_medicament->quantite) != 3) {
            free(nouveau_medicament);
            break;
        }
        nouveau_medicament->ordonnances = NULL;
        nouveau_medicament->suivant = NULL;
        if (precedent_medicament == NULL) {
            pharmacie->medicaments = nouveau_medicament;
        } else {
            precedent_medicament->suivant = nouveau_medicament;
        }
        precedent_medicament = nouveau_medicament;
        Ordonnance *precedente_ordonnance = NULL;
        while (1) {
            Ordonnance *nouvelle_ordonnance = (Ordonnance *)malloc(sizeof(Ordonnance));
            if (fscanf(fichier, "%d %19s", &nouvelle_ordonnance->numero, nouvelle_ordonnance->date) != 2) {
                free(nouvelle_ordonnance);
                break;
            }
            if (nouvelle_ordonnance->numero == -1) {
                free(nouvelle_ordonnance);
                break;
            }
            nouvelle_ordonnance->suivant = NULL;
            if (precedente_ordonnance == NULL) {
                nouveau_medicament->ordonnances = nouvelle_ordonnance;
            } else {
                precedente_ordonnance->suivant = nouvelle_ordonnance;
            }
            precedente_ordonnance = nouvelle_ordonnance;
        }
    }
    fclose(fichier);
}

void afficher_menu() {
    printf("--------------------------------------\n");
    printf("Menu:\n");
    printf("1. Ajouter un medicament\n");
    printf("2. Supprimer un medicament\n");
    printf("3. Ajouter une ordonnance\n");
    printf("4. Supprimer une ordonnance\n");
    printf("5. Afficher les medicaments\n");
    printf("6. Afficher les ordonnances\n");
    printf("7. Sauvegarder les donnees\n");
    printf("8. Charger les donnees\n");
    printf("9. Quitter\n");
    printf("Choisissez une option: ");
}
int main() {
    Pharmacie pharmacie;
    pharmacie.medicaments = NULL;

    // Charger les données de la pharmacie à partir du fichier
    charger_fichier(&pharmacie, "pharmacie.txt");

    int choix;
    do {
        afficher_menu();
        if (scanf("%d", &choix) != 1) {
            printf("Option invalide. Veuillez réessayer.\n");
            // Clear the invalid input
            while (getchar() != '\n');
            continue;
        }
        switch (choix) {
            case 1: {
                Medicament *nouveau_medicament = (Medicament *)malloc(sizeof(Medicament));
                printf("Nom du medicament: ");
                scanf("%49s", nouveau_medicament->nom);
                printf("Prix: ");
                scanf("%f", &nouveau_medicament->prix);
                printf("Quantite: ");
                scanf("%d", &nouveau_medicament->quantite);
                nouveau_medicament->ordonnances = NULL;
                nouveau_medicament->suivant = NULL;
                ajouter_medicament(&pharmacie, nouveau_medicament);
                break;
            }
            case 2: {
                char nom_medicament[50];
                printf("Nom du medicament a supprimer: ");
                scanf("%49s", nom_medicament);
                supprimer_medicament(&pharmacie, nom_medicament);
                break;
            }
            case 3: {
                char nom_medicament[50];
                printf("Nom du medicament pour ajouter l'ordonnance: ");
                scanf("%49s", nom_medicament);
                Medicament *medicament = pharmacie.medicaments;
                while (medicament != NULL && strcmp(medicament->nom, nom_medicament) != 0) {
                    medicament = medicament->suivant;
                }
                if (medicament != NULL) {
                    Ordonnance *nouvelle_ordonnance = (Ordonnance *)malloc(sizeof(Ordonnance));
                    printf("Numero de l'ordonnance: ");
                    scanf("%d", &nouvelle_ordonnance->numero);
                    printf("Date (YYYY-MM-DD): ");
                    scanf("%19s", nouvelle_ordonnance->date);
                    nouvelle_ordonnance->suivant = NULL;
                    ajouter_ordonnance(medicament, nouvelle_ordonnance);
                } else {
                    printf("Medicament non trouve\n");
                }
                break;
            }
            case 4: {
                char nom_medicament[50];
                int numero_ordonnance;
                printf("Nom du medicament pour supprimer l'ordonnance: ");
                scanf("%49s", nom_medicament);
                Medicament *medicament = pharmacie.medicaments;
                while (medicament != NULL && strcmp(medicament->nom, nom_medicament) != 0) {
                    medicament = medicament->suivant;
                }
                if (medicament != NULL) {
                    printf("Numero de l'ordonnance à supprimer: ");
                    scanf("%d", &numero_ordonnance);
                    supprimer_ordonnance(medicament, numero_ordonnance);
                } else {
                    printf("Medicament non trouve\n");
                }
                break;
            }
            case 5: {
                afficher_medicaments(&pharmacie);
                break;
            }
            case 6: {
                char nom_medicament[50];
                printf("Nom du medicament pour afficher les ordonnances: ");
                scanf("%49s", nom_medicament);
                Medicament *medicament = pharmacie.medicaments;
                while (medicament != NULL && strcmp(medicament->nom, nom_medicament) != 0) {
                    medicament = medicament->suivant;
                }
                if (medicament != NULL) {
                    afficher_ordonnances(medicament);
                } else {
                    printf("Medicament non trouve\n");
                }
                break;
            }
            case 7: {
                sauvegarder_fichier(&pharmacie, "pharmacie.txt");
                break;
            }
            case 8: {
                charger_fichier(&pharmacie, "pharmacie.txt");
                break;
            }
            case 9: {
                printf("Au revoir!\n");
                break;
            }
            default: {
                printf("Option invalide. Veuillez réessayer.\n");
                break;
            }
        }
    } while (choix != 9);

    // Libérer la mémoire allouée
    Medicament *medicament = pharmacie.medicaments;
    while (medicament != NULL) {
        Medicament *medicament_a_supprimer = medicament;
        medicament = medicament->suivant;

        Ordonnance *ordonnance = medicament_a_supprimer->ordonnances;
        while (ordonnance != NULL) {
            Ordonnance *ordonnance_a_supprimer = ordonnance;
            ordonnance = ordonnance->suivant;
            free(ordonnance_a_supprimer);
        }
        free(medicament_a_supprimer);
    }

    return 0;
}
