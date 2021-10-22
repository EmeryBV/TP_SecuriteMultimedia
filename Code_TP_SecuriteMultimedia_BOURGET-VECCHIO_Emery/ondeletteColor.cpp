// test_couleur.cpp : Seuille une image couleur

#include <stdio.h>
#include <math.h>
#include "image_ppm.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


void RGB_to_YCrCb(OCTET *ImgIn, OCTET *ImgY, OCTET *ImgCr, OCTET *ImgCb, int nTaille) {
    for (int i = 0; i < nTaille; i++) {
        ImgY[i] = 0.3 * ImgIn[3 * i] + 0.6 * ImgIn[3 * i + 1] + 0.1 * ImgIn[3 * i + 2];
        ImgCr[i] = 0.5 * ImgIn[3 * i] - 0.42 * ImgIn[3 * i + 1] - 0.08 * ImgIn[3 * i + 2] + 128;
        ImgCb[i] = -0.17 * ImgIn[3 * i] - 0.33 * ImgIn[3 * i + 1] + 0.5 * ImgIn[3 * i + 2] + 128;

    }
//    ecrire_image_pgm("../Y.ppm ", ImgY, 512, 512);
//    ecrire_image_pgm("../Cr.ppm ", ImgCr, 512, 512);
//    ecrire_image_pgm("../Cb.ppm ", ImgCb, 512 ,512);
}

void YCrCb_to_RGB(OCTET *Y, OCTET *Cr, OCTET *Cb, OCTET *R, OCTET *G, OCTET *B, int nTaille) {
    for (int i = 0; i < nTaille; i++) {
        R[i] = Y[i] + 1.4 * (Cr[i] - 128);
        G[i] = Y[i] - 0.34 * (Cb[i] - 128) - 0.71 * (Cr[i] - 128);
        B[i] = Y[i] + 1.77 * (Cb[i] - 128);

        R[i] = R[i] > 255 ? 255 : (R[i] < 0 ? 0 : R[i]);
        G[i] = G[i] > 255 ? 255 : (G[i] < 0 ? 0 : G[i]);
        B[i] = B[i] > 255 ? 255 : (B[i] < 0 ? 0 : B[i]);
    }
}

void ondelette(OCTET *Img, OCTET *ImgOut, OCTET *ImgOutBF, OCTET *ImgOutHF, OCTET *ImgOutBF_H, OCTET *ImgOutMF_V,
               int nH, int nW, int nTaille, int nbrRecursion, int nbrTours) {

    int ajustement = pow(2, nbrTours);

    int HF_Q = pow(2, nbrRecursion);
    int MF_Q = MAX(2, pow(2, nbrRecursion) / 2);
    for (int i = 0; i < nH / ajustement; i += 2) {
        for (int y = 0; y < nW / ajustement; y += 2) {
            OCTET P1 = Img[i * nH + y];
            OCTET P2 = Img[i * nH + y + 1];
            OCTET P3 = Img[(i + 1) * nH + y];
            OCTET P4 = Img[(i + 1) * nH + y + 1];

            if (i + 1 < nTaille && y + 1 < nTaille) {
                ImgOutBF[i * nH + y / 2] = (P1 + P2) / 2;
                ImgOutBF[(i + 1) * nH + y / 2] = (P3 + P4) / 2;

                ImgOutHF[(i * nH) + y] = 0;


                int diff = abs(MIN(128, MAX(-128, P1 - P2))) + 128;
                ImgOutHF[i * nW + y / 2] = diff;
                int diff2 = abs(MIN(128, MAX(-128, P3 - P4))) + 128;
                ImgOutHF[(i + 1) * nH + y / 2] = diff2;
            }
        }
    }
    for (int i = 0; i < (nH / pow(2, nbrTours)); i++) {
        for (int y = 0; y < ((nW / pow(2, nbrTours)) / 2) - 1; y++) {
            OCTET P1_BF = ImgOutBF[i * nH + y];
            OCTET P2_BF = ImgOutBF[i * nH + y + 1];
            OCTET P3_BF = ImgOutBF[(i + 1) * nH + y];
            OCTET P4_BF = ImgOutBF[(i + 1) * nH + y + 1];

            OCTET P1_HF = ImgOutHF[i * nH + y];
            OCTET P2_HF = ImgOutHF[i * nH + y + 1];
            OCTET P3_HF = ImgOutHF[(i + 1) * nH + y];
            OCTET P4_HF = ImgOutHF[(i + 1) * nH + y + 1];


            if (i + 1 < nTaille && y + 1 < nTaille) {
                ImgOutBF[(i * nH) + y] = 0;
                ImgOutBF[((i / 2) * nH) + y] = (P1_BF + P2_BF + P3_BF + P4_BF) / 4;
                ImgOut[i / 2 * nH + y] = (P1_BF + P2_BF + P3_BF + P4_BF) / 4;

                ImgOutMF_V[(i * nH) + y] = 0;
                int diff = MIN(128, MAX(-128, ((P1_BF - P2_BF + P3_BF - P4_BF) / 2) / MF_Q)) + 128; // P1 + P3 -P2 - P4
                ImgOutMF_V[((i / 2) * nH) + y] = diff;
                ImgOut[(i / 2) * nH + y + (nW / ajustement) / 2] = diff;

                //HF
                ImgOutHF[(i * nH) + y] = 0;
                diff = MIN(128, MAX(-128, (P1_HF - P2_HF - P3_HF + P4_HF) / HF_Q)) + 128; // P1 + P4 -P2-P3
                ImgOutHF[(i / 2 * nH) + y] = diff;
                ImgOut[(i / 2 + (nH / ajustement) / 2) * nH + y + (nW / ajustement) / 2] = diff;

                //BF_H
                ImgOutBF_H[(i * nH) + y] = 0;
                diff = MIN(128, MAX(-128, ((P1_HF - P2_HF + P3_HF - P4_HF) / 2) / MF_Q)) + 128; // P1 +P3 -P2-P4
                ImgOut[(i / 2 + (nH / ajustement) / 2) * nH + y] = diff;
                ImgOutBF_H[(i / 2 * nH) + y] = diff / 2;
            }
        }
    }

    OCTET *ImgOutBF_R, *ImgOutHF_R, *ImgOutBF_H_R, *ImgOutMF_V_R, *Imgout;
    allocation_tableau(Imgout, OCTET, nTaille);
    allocation_tableau(ImgOutBF_R, OCTET, nTaille);
    allocation_tableau(ImgOutHF_R, OCTET, nTaille);
    allocation_tableau(ImgOutBF_H_R, OCTET, nTaille);
    allocation_tableau(ImgOutMF_V_R, OCTET, nTaille);

    if (nbrRecursion > 0) {
        ondelette(ImgOutBF, Imgout, ImgOutBF_R, ImgOutHF_R, ImgOutBF_H_R, ImgOutMF_V_R, nH, nW,
                  nTaille, nbrRecursion - 1, nbrTours + 1);
        for (int i = 0; i < nH / ajustement; i++)
            for (int y = 0; y < (nW / ajustement) / 2; y++) {
                ImgOut[i / 2 * nH + y] = Imgout[i / 2 * nH + y];
            }
    }
}

void InverseConversion(OCTET *ImgIn, OCTET *ImgOut, int nTaille, int nbrRecursion, int nW, int nH) {
    OCTET *ImgOutBF, *ImgOutHF, *ImgOutMF_V, *ImgOutMF_H;

    allocation_tableau(ImgOutBF, OCTET, nTaille);
    allocation_tableau(ImgOutHF, OCTET, nTaille);
    allocation_tableau(ImgOutMF_V, OCTET, nTaille);
    allocation_tableau(ImgOutMF_H, OCTET, nTaille);


    int compteur = 0;
    while (compteur < nbrRecursion) {
        for (int i = 0; i < nH/2; ++i)
            for (int y = 0; y < nW/2; ++y) {
                if (i < nW / 2 && y < nH / 2) {
                    ImgOutBF[i * nH + y] = ImgIn[i * nH + y];
                    ImgOutMF_V[i * nH + y ] = ImgIn[i * nH + y+nW/2];
                    ImgOutMF_H[i * nH + y] = ImgIn[(i+nH/2) * nH + y];
                    ImgOutHF[i  * nH + y ] = ImgIn[(i+nH/2)* nH + y+nW/2];
            }
        }
        compteur++;
    }
    OCTET *ImgOutTemp;

    allocation_tableau(ImgOutTemp, OCTET, nTaille);

    for (int i = 0; i < nH;  i++)
        for (int y = 0; y < nW; y++) {
            float result = MIN(128, MAX(-255,(float((ImgOutBF[i * nH + y] + ImgOutMF_V[i * nH + y ]+ ImgOutMF_H[i * nH + y] +ImgOutHF[i  * nH + y ]))/4.0))) ;
            ImgOut[i * nH + y]= result ;
            ImgOutTemp[i * nH + y]= result ;

            ImgOut[i * nH + y+1]=  result ;
            ImgOutTemp[i * nH + y+1]=  result ;

            ImgOut[(i+1) * nH + y]=  result ;
            ImgOutTemp[(i+1) * nH + y]=  result ;

            ImgOut[(i+1) * nH + y+1]= result;
            ImgOutTemp[(i+1) * nH + y+1]= result;
        }

    for (int i = 0; i < nH;  i+=2)
        for (int y = 0; y < nW; y+=2) {
            int color = ImgOutTemp[i/2 * nH + y/2];
            ImgOut[i * nH + y]= color ;
            ImgOut[i * nH + y+1]= color ;
            ImgOut[(i+1) * nH + y]= color ;
            ImgOut[(i+1) * nH + y+1]= color ;

        }
    ecrire_image_pgm("../AZEAZE.pgm", ImgOut, nH, nW);
}

int main(int argc, char *argv[]) {
    char cNomImgLue[250], cNomImgCr[250];

    int nH, nW, nTaille;

    sscanf(argv[1], "%s", cNomImgLue);

    OCTET *ImgIn, *ImgOutY, *ImgOutCr, *ImgOutCb,
            *ImgY, *ImgCr, *ImgCb,
            *OutBF_Y, *OutMFV_Y, *OutMFH_Y, *OutHF_Y,
            *OutBF_Cr, *OutMFV_Cr, *OutMFH_Cr, *OutHF_Cr,
            *OutBF_Cb, *OutMFV_Cb, *OutMFH_Cb, *OutHF_Cb,
            *R, *G, *B, *ImgOut,
            *OutY_I, *OutCr_I, *OutCb_I;

    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW;

    int nTaille3 = nTaille * 3;
    allocation_tableau(ImgIn, OCTET, nTaille3);
    lire_image_ppm(cNomImgLue, ImgIn, nH * nW);

    allocation_tableau(ImgOutY, OCTET, nTaille);
    allocation_tableau(ImgOutCb, OCTET, nTaille);
    allocation_tableau(ImgOutCr, OCTET, nTaille);
    allocation_tableau(ImgY, OCTET, nTaille);
    allocation_tableau(ImgCr, OCTET, nTaille);
    allocation_tableau(ImgCb, OCTET, nTaille);


    allocation_tableau(OutBF_Y, OCTET, nTaille);
    allocation_tableau(OutMFV_Y, OCTET, nTaille);
    allocation_tableau(OutMFH_Y, OCTET, nTaille);
    allocation_tableau(OutHF_Y, OCTET, nTaille);


    allocation_tableau(OutBF_Cr, OCTET, nTaille);
    allocation_tableau(OutMFV_Cr, OCTET, nTaille);
    allocation_tableau(OutMFH_Cr, OCTET, nTaille);
    allocation_tableau(OutHF_Cr, OCTET, nTaille);


    allocation_tableau(OutBF_Cb, OCTET, nTaille);
    allocation_tableau(OutMFV_Cb, OCTET, nTaille);
    allocation_tableau(OutMFH_Cb, OCTET, nTaille);
    allocation_tableau(OutHF_Cb, OCTET, nTaille);

    allocation_tableau(R, OCTET, nTaille);
    allocation_tableau(G, OCTET, nTaille);
    allocation_tableau(B, OCTET, nTaille);

    allocation_tableau(ImgOut, OCTET, nTaille3);

    allocation_tableau(OutY_I, OCTET, nTaille);
    allocation_tableau(OutCr_I, OCTET, nTaille);
    allocation_tableau(OutCb_I, OCTET, nTaille);


    RGB_to_YCrCb(ImgIn, ImgY, ImgCr, ImgCb, nTaille);

    int nbrRecursion = 0;

    ondelette(ImgY, ImgOutY, OutBF_Y, OutHF_Y, OutMFV_Y, OutMFH_Y, nH, nW, nTaille, nbrRecursion, 0);
    ondelette(ImgCr, ImgOutCr, OutBF_Cr, OutHF_Cb, OutMFV_Cr, OutMFH_Cr, nH, nW, nTaille, nbrRecursion, 0);
    ondelette(ImgCb, ImgOutCb, OutBF_Cb, OutHF_Cr, OutMFV_Cb, OutMFH_Cb, nH, nW, nTaille, nbrRecursion, 0);

    YCrCb_to_RGB(ImgOutY, ImgOutCr, ImgOutCb, R, G, B, nTaille);

    for (int i = 0; i < nTaille3; i += 3) {
        ImgIn[i] = R[i / 3];
        ImgIn[i + 1] = G[i / 3];
        ImgIn[i + 2] = B[i / 3];
    }

    ecrire_image_pgm("../ImgOutY.pgm ", ImgOutY, nH, nW);
    ecrire_image_ppm("../imgReconstruite.ppm", ImgIn, nH, nW);
    ecrire_image_pgm("../ImgOutCb.pgm ", ImgOutCb, nH, nW);
    ecrire_image_pgm("../ImgOutCr.pgm ", ImgOutCr, nH, nW);

    RGB_to_YCrCb(ImgIn,ImgY,ImgCr,ImgCb,nTaille);

    InverseConversion(ImgY,OutY_I,nTaille,1, nH,nW);
    InverseConversion(ImgCb,OutCr_I,nTaille,1, nH,nW);
    InverseConversion(ImgCr,OutCb_I,nTaille,1, nH,nW);


    YCrCb_to_RGB(OutY_I,OutCr_I ,OutCb_I, R,G,B, nTaille);

//    for (int i = 0; i < nTaille3; i += 3) {
//        ImgOut[i] = OutY_I[i / 3];
//        ImgOut[i + 1] = OutCr_I[i / 3];
//        ImgOut[i + 2] = OutCb_I[i / 3];
//    }

    for (int i = 0; i < nTaille3; i += 3) {
        ImgOut[i] = R[i / 3];
        ImgOut[i + 1] = G[i / 3];
        ImgOut[i + 2] = B[i / 3];
    }
    ecrire_image_ppm("../imgDecompress.ppm", ImgOut, nH, nW);

    free(ImgIn);
    free(ImgOutY);
    free(ImgOutCr);
    free(ImgOutCb);
    free(ImgY);
    free(ImgCr);
    free(ImgCb);
    free(OutBF_Y);
    free(OutMFV_Y);
    free(OutMFH_Y);
    free(OutHF_Y);
    free(OutBF_Cr);
    free(OutMFV_Cr);
    free(OutMFH_Cr);
    free(OutHF_Cr);
    free(OutBF_Cb);
    free(OutMFV_Cb);
    free(OutMFH_Cb);
    free(OutHF_Cb);
    return 1;
}