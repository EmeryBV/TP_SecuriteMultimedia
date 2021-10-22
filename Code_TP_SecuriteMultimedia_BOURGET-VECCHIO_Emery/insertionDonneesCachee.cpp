// test_couleur.cpp : Seuille une image en niveau de gris

#include <stdio.h>
#include "image_ppm.h"


//Genere une sequence aleatoire
void randomGenerator(int value, int nTaille, int *result) {
    srand(value);
    for (int i = 0; i < nTaille; ++i) {
        result[i] = rand() % 2;
//        printf("\n %d", result[i]);
    }
}

void convertToBinary(unsigned int n) {
    if (n / 2 != 0) {
        convertToBinary(n / 2);
    }
    printf("%d", n % 2);
}


//Chiffre l'image avec une séquence aléatoire
void encode(OCTET *ImgIn, OCTET *ImgOut, int nTaille, int SID) {

    int result[nTaille];
    randomGenerator(SID, nTaille, result);

    for (int i = 0; i < nTaille; ++i) {

        int bit1 = ImgIn[i];
        int bit2 = result[i];
        ImgOut[i] = bit1 ^ bit2;

    }
}


void getBinaryPlan(OCTET *imgIn, OCTET *imgOut, int nTaille, int k) {
    for (int i = 0; i < nTaille; ++i) {
        imgOut[i] = (imgIn[i] & (1 << k)) == 0 ? 0 : 255;
        convertToBinary(1 << 7);
        convertToBinary(imgIn[i]);
    }

}


void PSNR(OCTET *Img1, OCTET *Img2, int nTaille) {

    float grandtotal = 0;
    int v1, v2;
    for (int i = 0; i < nTaille; i++) {
        v1 = Img1[i];
        v2 = Img2[i];
        grandtotal += ((v1 - v2) * (v1 - v2));
    }
    grandtotal /= (float) (nTaille);

    float result = 10 * log10((255 * 255) / ((float) (grandtotal)));
    printf("Le PSNR est %f\n", result);

}


void hideMessage(OCTET *imgIn, OCTET *imgOut, int nTaille, int k, int SID) {
    int result[nTaille];
    randomGenerator(SID, nTaille, result);
    for (int i = 0; i < nTaille; ++i) {
        int new_value = result[i];
        int bit_mask = (1 << k);
        imgOut[i] = (imgIn[i] & (~bit_mask) | (new_value << k));
    }
}

void histogramme_Function(OCTET *Img, int nTaille, int *histogramme) {

    for (int i = 0; i < nTaille; ++i) {
        histogramme[Img[i]]++;
    }

}

void exportHisto(OCTET *Img, int nTaille) {
    int histogramme[256] = {0};
    histogramme_Function(Img, nTaille, histogramme);

    FILE *fichier = NULL;

    fichier = fopen("histogrammeLSB.dat", "w+");

    FILE *fopen();

    char chaine[255];
    for (int i = 0; i < 256; ++i) {
//        printf("\n%d" ,i );
//        printf("    %d  ", histogramme[i]) ;
        sprintf(chaine, "%d\n", histogramme[i]);
        fputs(chaine, fichier);
    }
}


int main(int argc, char *argv[]) {
    char ImageOriginal[250], Img2[250];
    int nH, nW, nTaille;


    sscanf(argv[1], "%s", ImageOriginal);
//    sscanf (argv[2], "%s", Img2) ;

    OCTET *ImgIn, *ImgOut, *ImgEncode;

    lire_nb_lignes_colonnes_image_pgm(ImageOriginal, &nH, &nW);
    nTaille = nH * nW;

    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(ImageOriginal, ImgIn, nH * nW);

    allocation_tableau(ImgOut, OCTET, nTaille);
    allocation_tableau(ImgEncode, OCTET, nTaille);

    int SID = 53;
    encode(ImgIn, ImgEncode, nTaille, SID);
//    getBinaryPlan(ImgEncode, ImgOut, nTaille, 7);
    hideMessage(ImgIn, ImgOut, nTaille, 0, SID);
    PSNR(ImgIn, ImgOut, nTaille);
    exportHisto (ImgOut,nTaille);
    ecrire_image_pgm("../binaire.pgm", ImgOut, nH, nW);
    free(ImgIn);
    return 1;
}