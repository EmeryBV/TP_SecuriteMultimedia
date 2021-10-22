// test_couleur.cpp : Seuille une image en niveau de gris

#include <stdio.h>
#include "image_ppm.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


//Genere une sequence aleatoire
void randomGenerator(int value, int nTaille, int *result) {
    srand(value);
    for (int i = 0; i < nTaille; ++i) {
        result[i] = rand() % 256;
//        printf("\n %d", result[i]);
    }
}

//Cache une séquence de bit dans l'image
void hideMessage(OCTET *imgIn, OCTET *imgOut, int nW, int nH, int k, int SID) {
    int result[nW * nH];
    randomGenerator(SID, nW * nH, result);
    for (int i = 0; i < nH; ++i) {// On commence à la premiere ligne
        for (int j = 0; j < nW; ++j) {// On commence à la premiere colonne
            if (i == 0 || j == 0) {
                imgOut[i * nH + j] = imgIn[i * nH + j];
            } else {
                int new_value = result[i * nH + j];
                int bit_mask = (1 << k);
                imgOut[i * nH + j] = (imgIn[i * nH + j] & (~bit_mask) | (new_value << k));
            }
        }

    }
}

//Convertie l'image en binaire
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

//Permet de décoder l'image
void decode(OCTET *ImgIn, OCTET *ImgOut, int nTaille, int SID) {

    int result[nTaille];
    randomGenerator(SID, nTaille, result);

    for (int i = 0; i < nTaille; ++i) {

        int bit1 = ImgIn[i];
        int bit2 = result[i];
        ImgOut[i] = bit1 ^ bit2;

    }
}
//Calcule le PSNR
void PSNR(OCTET *Img1, OCTET *Img2, int nTaille) {

    float grandtotal = 0;
    int v1, v2;
    for (int i = 0; i < nTaille; i++) {
        v1 = Img1[i];
        v2 = Img2[i];
        grandtotal += ((v1 - v2) * (v1 - v2));
    }
    grandtotal /= (float) (nTaille);

    float result = 10 * log10((256 * 256) / ((float) (grandtotal)));
    printf("Le PSNR est %f\n", result);

}

void histogramme_Function(OCTET *Img, int nTaille, int *histogramme) {

    for (int i = 0; i < nTaille; ++i) {
        histogramme[Img[i]]++;
    }

}

//Calcule l'entropie
void entropie(OCTET *Img, int nTaille) {

    int histogramme[256] = {0};
    histogramme_Function(Img, nTaille, histogramme);
    float somme = 0;
    for (int i = 0; i < 256; ++i) {
        if (histogramme[i] > 0) {
            float pi = (float) histogramme[i] / (float) nTaille;
//            printf("La somme est %d\n", histogramme[i] );
            somme -= pi * log2(pi);
        }
    }
    printf("La somme est %f\n", somme);
}

//Permet d'exporter l'histogramme d'une image sous format txt
void exportHisto(OCTET *Img, int nTaille) {
    int histogramme[256] = {0};
    histogramme_Function(Img, nTaille, histogramme);
    FILE *fichier = NULL;
    fichier = fopen("histogramme.dat", "w+");
    FILE *fopen();
    char chaine[255];
    for (int i = 0; i < 256; ++i) {
        sprintf(chaine, "%d\n", histogramme[i]);
        fputs(chaine, fichier);
    }
}

//Inverse le k bit
int inverseBit(int valeur, int k) {
    if ((valeur & 1 << k) == 128) {
        int new_value = 0;
        int bit_mask = (1 << k);
        return (valeur & (~bit_mask) | (new_value << k));
    } else {
        int new_value = 1;
        int bit_mask = (1 << k);
        return (valeur & (~bit_mask) | (new_value << k));
    }
}

//Permet de prédire un pixel
void predict(OCTET *ImgIn, OCTET *pred, int i, int j, int nbrColumn) {
    pred[i * nbrColumn + j] = (ImgIn[i * nbrColumn + j - 1] + ImgIn[(i - 1) * nbrColumn + j] + ImgIn[(i - 1) * nbrColumn + (j - 1)]) / 3;
}

//Réalise le pré-traitement d'une image
void preProcessing(OCTET *ImgIn, OCTET *ImgProcess, int nW, int nH) {
    OCTET *pred;
    allocation_tableau(pred, OCTET, nW * nH);
    for (int i = 0; i < nH; ++i)
        for (int j = 0; j < nW; ++j) {
            predict(ImgProcess, pred, i, j, nH);
            if (abs(pred[i * nH + j] - ImgIn[i * nH + j]) >= abs(pred[i * nH + j] - inverseBit(ImgIn[i * nH + j],7))) {
                if (ImgIn[i * nH + j] < 128) ImgProcess[i * nH + j] = pred[i * nH + j] - 63;
                else ImgProcess[i * nH + j] = pred[i * nH + j] + 63;
            } else ImgProcess[i * nH + j] = ImgIn[i * nH + j];
        }
}

//Reconstruit une image mal formée
void reconstructImage(OCTET *ImgIn, OCTET *ImgOut, int nW, int nH) {
    OCTET *pred;
    allocation_tableau(pred, OCTET, nW * nH);
    for (int i = 0; i < nH; ++i)
        for (int j = 0; j < nW; ++j) {
            if (i == 0 || j == 0) {
                ImgOut[i * nH + j] = ImgIn[i * nH + j];
            } else {
                predict(ImgOut, pred, i, j, nH);
                if (abs(pred[i * nH + j] - ImgIn[i * nH + j]) < abs(pred[i * nH + j] - inverseBit(ImgIn[i * nH + j],7))) {
                    ImgOut[i * nH + j] = ImgIn[i * nH + j];
                } else {
                    ImgOut[i * nH + j] = inverseBit(ImgIn[i * nH + j],7);
                }
            }
        }
}

int main(int argc, char *argv[]) {
    char ImageOriginal[250], Img2[250];
    int nH, nW, nTaille;


    sscanf(argv[1], "%s", ImageOriginal);

    OCTET *ImgIn, *ImgIn2, *ImgEncode, *ImgDecode, *ImgCrypt, *ImgReconstruct, *ImgPreProcessing;

    lire_nb_lignes_colonnes_image_pgm(ImageOriginal, &nH, &nW);
    nTaille = nH * nW;

    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(ImageOriginal, ImgIn, nH * nW);


    allocation_tableau(ImgEncode, OCTET, nTaille);
    allocation_tableau(ImgDecode, OCTET, nTaille);
    allocation_tableau(ImgCrypt, OCTET, nTaille);
    allocation_tableau(ImgReconstruct, OCTET, nTaille);
    allocation_tableau(ImgPreProcessing, OCTET, nTaille);

    int SID = 53;
    preProcessing(ImgIn, ImgPreProcessing, nW, nH);
    encode(ImgPreProcessing, ImgEncode, nTaille, SID);
    hideMessage(ImgEncode, ImgCrypt, nW, nH, 7, SID);
    decode(ImgCrypt, ImgDecode, nTaille, SID);
    reconstructImage(ImgDecode, ImgReconstruct, nH, nW);

    PSNR(ImgPreProcessing, ImgReconstruct, nTaille);

//
    ecrire_image_pgm("../ImgPreProcesing.pgm", ImgPreProcessing, nH, nW);
    ecrire_image_pgm("../ImgEncode.pgm", ImgEncode, nH, nW);
    ecrire_image_pgm("../ImgEncodeCrypt.pgm", ImgCrypt, nH, nW);
    ecrire_image_pgm("../ImgDecodeCrypt.pgm", ImgDecode, nH, nW);
    ecrire_image_pgm("../ImgDecodeCryptReconstruct.pgm", ImgReconstruct, nH, nW);
//    ecrire_image_pgm("../ImgDeco.pgm", ImgDecode, nH, nW);

//    PSNR(ImgIn,ImgPreProcessing,nTaille);
//    entropie(ImgIn,nTaille);
//    entropie(ImgEncode,nTaille);

//    exportHisto(ImgEncode, nTaille);
    free(ImgIn);
    return 1;
}