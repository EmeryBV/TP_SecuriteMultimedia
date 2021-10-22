// test_couleur.cpp : Seuille une image en niveau de gris

#include <stdio.h>
#include "image_ppm.h"

//Genere une sequence aleatoire
void randormGenerator(int value, int nTaille, int * result){
    srand(value);
    for (int i = 0; i <nTaille ; ++i) {
        result[i] = rand()%256;
//        printf("\n %d", result[i]);
    }
}

//Chiffre l'image avec une séquence aléatoire
void encode(OCTET * ImgIn, OCTET * ImgOut, int nTaille, int SID){

    int result[nTaille];
    randormGenerator(SID, nTaille,result);

    for (int i = 0; i <nTaille ; ++i) {

        int  bit1 = ImgIn[i] ;
        int  bit2 = result[i] ;
        ImgOut[i] = bit1 ^ bit2  ;

    }
}

void decode(OCTET * ImgIn, OCTET * ImgOut, int nTaille, int SID){

    int result[nTaille];
    randormGenerator(SID, nTaille,result);

    for (int i = 0; i <nTaille ; ++i) {

        int  bit1 = ImgIn[i] ;
        int  bit2 = result[i] ;
        ImgOut[i] = bit1 ^ bit2  ;

    }
}

void PSNR(OCTET  * Img1, OCTET *Img2,int nTaille){

    float grandtotal = 0;
    int v1, v2;
    for (int i=0; i < nTaille; i++)
    {
        v1 = Img1[i];
        v2 = Img2[i];
        grandtotal += ((v1-v2)*(v1-v2));
    }
    grandtotal /= (float)(nTaille);

    float result = 10 * log10((256*256)/((float)(grandtotal)));
    printf("Le PSNR est %f\n", result);

}

void histogramme_Function(OCTET * Img, int nTaille,int * histogramme ){

    for (int i = 0; i < nTaille ; ++i) {
        histogramme[Img[i]]++;
    }

}

void entropie(OCTET  * Img, int nTaille){

    int histogramme[256] = {0};
    histogramme_Function(Img,nTaille,histogramme);
    float somme = 0 ;
    for (int i = 0; i <256 ; ++i) {
        if(histogramme[i]>0) {
            float pi = (float) histogramme[i] / (float) nTaille;
//            printf("La somme est %d\n", histogramme[i] );
            somme -= pi *log2(pi);
        }
    }
    printf("La somme est %f\n", somme);
}


void exportHisto(OCTET * Img, int nTaille ){
    int histogramme[256] = {0};
    histogramme_Function(Img,nTaille,histogramme);

    FILE *fichier = NULL;

    fichier = fopen("histogramme.dat", "w+");

    FILE *fopen();

    char chaine[255];
    for (int i = 0; i <256 ; ++i) {
//        printf("\n%d" ,i );
//        printf("    %d  ", histogramme[i]) ;
        sprintf(chaine, "%d\n", histogramme[i]);
        fputs(chaine, fichier);
    }
}

int main(int argc, char* argv[])
{
    char ImageOriginal[250],Img2[250] ;
    int nH, nW, nTaille;



    sscanf (argv[1], "%s", ImageOriginal) ;

    OCTET *ImgIn,*ImgIn2, *ImgEncode, *ImgDecode;

    lire_nb_lignes_colonnes_image_pgm(ImageOriginal, &nH, &nW);
    nTaille = nH * nW;

    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(ImageOriginal, ImgIn, nH * nW);


    allocation_tableau(ImgEncode, OCTET, nTaille);
    allocation_tableau(ImgDecode, OCTET, nTaille);


//    for (int i=0; i < nH; i++)
//        for (int j=0; j < nW; j++)
//        {
//            ImgEncode[i * nH + j ] = ImgIn[i * nH + j ];
//        }


    int SID = 53 ;
    encode(ImgIn, ImgEncode, nTaille, SID);
//    decode(ImgEncode, ImgDecode, nTaille,SID);
//
//    ecrire_image_pgm("../ImgEncode.pgm", ImgEncode, nH, nW);
//    ecrire_image_pgm("../ImgDeco.pgm", ImgDecode, nH, nW);

    PSNR(ImgIn,ImgEncode,nTaille);
    entropie(ImgIn,nTaille);
    entropie(ImgEncode,nTaille);

    exportHisto(ImgEncode, nTaille);
    free(ImgIn);
    return 1;
}