#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BLUE   "\x1B[34m"
#define RESET "\x1B[0m"
#define GREEN   "\x1B[32m"
#define RED   "\x1B[31m"


typedef struct Transition{ //Chaque element de cette strcutre contiendrai le mot de transition ainsi que l'etat suivant 
    char trans[10]; //le mot 
    struct Etat *etat; //l'etat suivant
    struct Transition *suivant; //la transition suivante
}Transition;

typedef struct Etat{ //Une etat contient un nom, un booleen et une liste des transition définie ci-dessus 
    char nom[10]; //nom de l'etat
    int final; //final ou non 
    struct Transition *trans; //liste des transition
    struct Etat *suivant; //Tous les etats de l'automate sont regroupés dans une liste
}Etat;


int ajouterTrans(Transition **tete,char *trans,Etat *etat){ //ajouter une transition a la liste des transition en entrée
    Transition *nvTrans = malloc(sizeof(Transition));
    strcpy(nvTrans->trans,trans);
    nvTrans->etat=etat,
    nvTrans->suivant=NULL;
    if(*tete==NULL){
        *tete=nvTrans;
        return 0;
    }
    Transition *ttete=*tete;
    while(ttete->suivant!=NULL) ttete=ttete->suivant;
    ttete->suivant=nvTrans;
}

int ajouterEtat(Etat **tete,char *nom,int final,Transition *trans){ //ajouter un nouvel etat a la liste des etats 
    Etat *nvElt = malloc(sizeof(Etat));
    strcpy(nvElt->nom,nom);
    nvElt->final=final;
    nvElt->trans=trans;
    nvElt->suivant=NULL;
    if(*tete==NULL){
        *tete=nvElt;
        return 0;
    }
    Etat *ttete=*tete;
    while(ttete->suivant!=NULL){
        ttete = ttete->suivant;    
    }
    ttete->suivant=nvElt;
}

int rechNom(Etat *tete,char *nom,Etat **result){ //Rerchercher un etat dont le nom est "nom", et retourner son pointeur s'il existe 
    while(tete!=NULL){
        if(strcmp(tete->nom,nom)==0){
            *result=tete;
            return 1;
        }
        tete=tete->suivant;
    }
    *result=NULL;
    return 0;
}

void afficherTransit(Transition *tete){ //Afficher une transition 
    while(tete!=NULL){
        printf("Transition avec"BLUE " %s " RESET "vers l'etat " BLUE "%s\n" RESET,tete->trans,tete->etat->nom); 
        tete=tete->suivant;
    }
}

void afficherEtat(Etat *tete){ //afficher un etat ainsi que ses transitions 
    while(tete!=NULL){
        printf("%s",tete->nom);
        if(tete->final==1) printf(BLUE " - etat final" RESET);
        printf("\n");
        afficherTransit(tete->trans);
        printf("---------------------------------------\n");
        tete=tete->suivant;
    }
}

int rechTransit(Transition *tete,char *trans,char *nom,int diff){ //Rechercher s'il y a une transition avec le meme mot, si diff=0 on cherche que l'etat suivant de cette etat est le meme en entre, et on cherche que les etats sois differents sinon 
    while(tete!=NULL){
        if(strcmp(tete->trans,trans)==0 && (strcmp(tete->etat->nom,nom)!=0)==diff) return 1;
        tete=tete->suivant; 
    }
    return 0;
}

void transitEtat(Etat *etat,Etat *tete,int *deter,int *simp){ //Associer une liste des transition a un etat et retourner 2 booleen indiquant si avec ces transitions l'automate reste simple ou deterministe ou pas  
    printf("Donner le nombre de transition pour cet etat: ");
    int nbTrans,deterministe=1,simple=1;
    scanf("%d",&nbTrans);
    char trans[10],nomm[10];
    Transition *transit=NULL;
    Etat *ettat=NULL;
    while(nbTrans>0){
        printf("Transition avec le mot (Utilisez le '#' pour une transition spontanne): ");
        scanf("%s",trans);
        if(strlen(trans)>1 || trans[0]=='#') simple=0;
        printf("Vers l'etat: ");
        scanf("%s",nomm);
        if(rechTransit(transit,trans,nomm,0)) printf(RED "Cette transition existe deja!\n" RESET);
        else{
            if(rechNom(tete,nomm,&ettat)) ajouterTrans(&transit,trans,ettat);
            else printf("Cette etat n'existe pas dans votre automate!\n");
            if(rechTransit(transit,trans,nomm,1)) deterministe=0;
            nbTrans--;
        }
    }
    etat->trans=transit;
    *deter=deterministe && (*deter);
    *simp=simple && (*simp);
}

void createTransit(Etat *tete,int *deter,int *simp){ //Creer les transition pour chaque cas dans l'automate 
    Etat *ett=tete;
    int simple=1,deterministe=1;
    while(ett!=NULL){
        printf("Transition de l'etat" BLUE " %s:\n" RESET,ett->nom);
        transitEtat(ett,tete,&deterministe,&simple);
        ett=ett->suivant;
        printf("-------------------------------------------------\n");
    }
    printf(GREEN "----------------- Automate cree! ----------------\n" RESET);
    printf(GREEN "-------------------------------------------------\n" RESET);
    *simp=simple;*deter=deterministe;
}

void createEtats(Etat **tete,int nb){ //Creer nb etat dans l'automate en indiquant le nom et si l'etat est final ou pas 
    int count=0,final;
    char nom[10],rep[3];
    Etat *etatInit = NULL,*ett;
    while(count<nb){
        count++;
        if(count==1) printf("Entrez le nom du premier etat(Etat initial): ");
        else printf("Entrez le nom de l'etat N %d: ",count);
        scanf("%s",nom);
        if(rechNom(etatInit,nom,&ett)) {
            printf(RED "Il existe deja un etat avec ce mot\n" RESET);
            count--;
        }
        else{
            printf("Est ce un etat final? [o/n] ");
            scanf("%s",rep);
            if(rep[0]=='o' || rep[0]=='O') final=1;
            else final=0;
            ajouterEtat(&etatInit,nom,final,NULL);
        }
    }
    *tete=etatInit;
    printf("-------------------------------------------------\n");
}

int contient(Transition *tete,char *trns,Transition ** trans){ //Rechercher dans la liste des transitions s'il y a une transition avec le mot trans et retourner son adresse si c'est le cas  
    int found=0;
    while(!found && tete!=NULL){
        if(strcmp(trns,tete->trans)==0){
            *trans=tete;
            return 1;
        }
        tete=tete->suivant;
    }
    *trans=NULL;
    return 0;
}

int reconnaitreUnMot(Etat *tete,char *mot,int *ffin,Etat **chemin){ //Verifier si le mot en entrée est reconnue par l'automate, et donner le chemin parcouru ainsi que là où on est bloqué et pourquoi on s'est bloqué
    int count=0,verif;
    Transition *transs =NULL;
    Etat *chem =NULL;
    char car[1];
    *ffin=0;
    while(tete!=NULL && count<strlen(mot)){
        car[0]=mot[count];car[1]='\0';
        if(contient(tete->trans,car,&transs)){
            ajouterEtat(&chem,tete->nom,tete->final,tete->trans);
            tete=transs->etat;
        }
        else{
            verif=0;
            ajouterEtat(&chem,tete->nom,tete->final,tete->trans);
            *chemin = chem;
            return 0;
        }
        count++;
    }
    if(tete!=NULL && tete->final==1){
        ajouterEtat(&chem,tete->nom,tete->final,tete->trans);
        verif=1;
        *chemin = chem;
        return 1;
    }
    verif=0;
    *ffin=1;
    ajouterEtat(&chem,tete->nom,tete->final,tete->trans);
    *chemin = chem;
    return 0;
}

void afficherChemin(Etat *chemin){ //afficher un ensemble des etats 
    while(chemin!=NULL){
          printf("%s - ",chemin->nom);
          chemin = chemin->suivant;
    }
}

int existChaine(Etat *etatDepart,Etat *etatArrive,Etat **etatsParcourus){ //Verifier si en allant de l'etat etatDeDepart on pourra arriver a etatArrive, et donner le chemin parcouru 
    Etat *ett=*etatsParcourus,*result = NULL;
    if(etatDepart==etatArrive) return 1;
    if (rechNom(ett,etatDepart->nom,&result)) return 0; 
    ajouterEtat(&ett,etatDepart->nom,etatDepart->final,etatDepart->trans);
    Transition *trans=etatDepart->trans;
    int exist=0;
    while(trans!=NULL && !exist){
        exist = existChaine(trans->etat,etatArrive,&ett);
        trans=trans->suivant;
    }
    *etatsParcourus=ett;
    return exist;
}

int estAccessible(Etat *tete,Etat *etatt){ //Verifier si l'etat est accessible ou pas en exploitant la fonction implémentée ci-dessus 
    Etat *parcours = NULL;
    return existChaine(tete,etatt,&parcours);
}

int estCoaccessible(Etat *tete,Etat *etatt){ //Verifier si l'etat est coaccessible, un etat est coacessible s'il on peut trouver une chaine ammenant a un etat final
    int coaccessible=0;
    Etat *parcours;
    while(tete!=NULL && !coaccessible){
        if(tete->final==1){
            parcours = NULL;
            coaccessible = existChaine(etatt,tete,&parcours); 
        }
        tete=tete->suivant;
    }
    return coaccessible;
}

void supprimerTransitionDans(Etat *etat,Transition **trans){ //Supprimer les transitions ammenant a l'etat 'etat' dans la liste pointée par trans
    if((*trans)->etat==etat) {
        *trans=(*trans)->suivant;
    }
    else{
        Transition *tete=*trans;
        while(tete->suivant!=NULL){
            if(tete->suivant->etat==etat) tete->suivant=tete->suivant->suivant;
            else tete=tete->suivant;
        }
    }
}

void supprimerTransitionVers(Etat *etat,Etat *tete){ //Supprimer les transitions ammenant a letat 'etat' dans tous les etats de l'automate
    Transition *trans; 
    while(tete!=NULL){
        trans=tete->trans;
        if(trans!=NULL) {
            supprimerTransitionDans(etat,&trans);
            tete->trans=trans;
        }
        tete=tete->suivant;
    }
}

void supprimerEtat(Etat **tete,Etat *etat){ //Supprimer une etat de la liste des etats de l'automate
    if((*tete)==etat) *tete=(*tete)->suivant;
    else{
        Etat *tet=*tete;
        int supp=0;
        while(tet->suivant!=NULL && !supp){
            if(tet->suivant==etat){
                tet->suivant=etat->suivant;
                supp=1;
            }
            else tet=tet->suivant;
        }
    }
    supprimerTransitionVers(etat,*tete); //On supprime ainsi toute les transitions qui ammenent a cet etat
}

void reduireAutomate(Etat **tete){ //Reduire un automate
    Etat *etat=*tete;
    if(estCoaccessible(etat,etat)==0){ //Si la tete n'est pas coacessible on la supprime 
        supprimerEtat(&etat,etat);
        *tete=etat;
    }
    else etat=etat->suivant; 
    while(etat!=NULL){ //Pour chaque etat etant non-accessible ou non-coaccessible on le supprime 
        if(estAccessible(*tete,etat)==0 || estCoaccessible(*tete,etat)==0) supprimerEtat(&(*tete),etat);   
        etat=etat->suivant; 
    }
}

int menu(){
    int choix;
    printf("\n");
    printf("\t1)Creer un automate\n");
    printf("\t2)Reduire l'automate\n");
    printf("\t3)Tester la reconnaissance d'un mot\n");
    printf("\t4)Afficher l'automate\n");
    printf("\t5)Quitter\n");
    printf(RED "\n\t    Votre choix: " RESET);
    scanf("%d",&choix);
    printf("-------------------------------------------------------------\n");
    return choix;
}

int main()
{
    int nbEtats=0,simple,deterministe,choix=0;
    char mot[10];
    while(choix!=5){
        choix=menu();
        switch (choix){
            case 1: //Creation d'un automate
                printf(BLUE "--------------------Creation d'un automate--------------------\n" RESET);
                printf("Entrez le nombre d'etat de l'automate: ");
                scanf("%d",&nbEtats);
                if(nbEtats<=0){
                    printf(RED "Ne peut pas etre nul\n" RESET);
                    break;
                }
                Etat *tete=NULL,*chemin=NULL;
                createEtats(&tete,nbEtats);
                createTransit(tete,&deterministe,&simple);
                break;
            case 2: //Reduction de l'automate
                printf(BLUE "--------------------Reduction de l'automate--------------------\n" RESET);
                if(tete!=NULL) reduireAutomate(&tete);
                else printf(RED "Vous devez d'abored introduire l'automate\n" RESET);            
                break;
            case 3: //Tester la reconnaissance d'un mot 
                printf(BLUE "--------------------Reconnaissance d'un mot--------------------\n" RESET);
                if(tete!=NULL){
                    printf("Entrez le mot: ");
                        scanf("%s",mot);
                        if(reconnaitreUnMot(tete,mot,&nbEtats,&chemin)){
                            printf("Le mot est reconnu!\nLe chemin parcouru est: ");
                            afficherChemin(chemin);
                            printf("\nChemin Reussi!");
                        }
                        else{
                            printf("Le mot n'est pas reconnu!\nLe chemin parcouru est: ");
                            afficherChemin(chemin);
                            if(nbEtats) printf("L'etat ou on est arrive n'est pas final");
                            else printf("Bloque! Pas de transition convenable!");
                        }
                        if(!simple) printf("L'automate est generaliste\n");
                        else if(!deterministe) printf("L'automate est simple non-deterministe\n");
                             else printf("L'automate est simple et deterministe\n");
                        printf("\n");
                }
                else printf(RED "Vous devez d'abored introduire l'automate\n" RESET);
                break;
            case 4: //Affichage de l'automate
                printf(BLUE "--------------------Affichage de l'automate--------------------\n" RESET);
                if(tete!=NULL){
                    afficherEtat(tete);
                    if(!simple) printf("L'automate est generalise\n");
                    else if(!deterministe) printf("L'automate est simple non-deterministe\n");
                         else printf("L'automate est simple et deterministe\n");
                }
                else printf(RED "Vous devez d'abored introduire l'automate\n" RESET);
                break;
        }
    }
    /*reduireAutomate(&tete);
    Etat *etat1,*etat2,*etat3;
    etat1=etat2=etat3=NULL;
    memset(mot,0,sizeof(mot));
    if(!simple) printf("L'automate est generaliste\n");
    else if(!deterministe) printf("L'automate est simple non-deterministe\n");
         else printf("L'automate est simple et deterministe\n");
    while(1){
        printf("Tetser l'etat: ");
        scanf("%s",mot);
        if(rechNom(tete,mot,&etat1)) printf("Etat acessible? OUI=%d\tEtat coacessible? OUI=%d\n",estAccessible(tete,etat1),estCoaccessible(tete,etat1));
    }
    //printf("Existe une chaine entre sa et sb? OUI=%d\n",existChaine(etat1,etat2,&etat3));
    while(1){
        
    }    
    afficherEtat(tete);
    printf("\n");*/
    return 0;
}
