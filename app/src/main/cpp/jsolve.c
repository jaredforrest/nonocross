/**
 * This Program, which is solving Japan Crosswords.
 * author: Syromolotov Evgeniy.
 * email: zhesyr@sampo.ru.
 * License: GPL2. */

#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <jni.h>
#include <android/log.h>
#include <stdbool.h>

#include <string.h>

const unsigned char empty = 2;
const unsigned char yes = 1;
const unsigned char no = 0;
const unsigned char mas[3]={'X',' ','*'};

struct active {
    int cis, netu, m1, m2;
    unsigned char m3;
    bool notchoise;
    struct active *next[2], *prev[2];

};

struct passive {
    int cis, m1, m2, m12, m22;
    unsigned char m3, m32;
    struct passive *next[3], *prev[3];

};

struct field {
    struct active *element[2];
    struct passive start[2], finish[2];
    bool notchoise;
};

struct levpravverhniz {
    int i, j, znach, cis, vid;
    struct levpravverhniz *next[2], *prev[2];

};

struct kluch {
    int i, j;
    struct kluch *prev;
};

unsigned char **jpn;
int height, width;
#ifdef DEBUG
int test2=0;
#endif
int **rowNums, **colNums, **lev, **prav, **verh, **niz;
int *rowNumCount, *colNumCount, *m12, *m22;
bool er, **kluchlev, **kluchprav, **kluchverh, **kluchniz, *kluch12, *kluch22, perebor = false;
int netu, netu4, netu5, netu6, netu7, sumstr, sumsto,kolic, kol = 0;
int *kolic1;
clock_t time1;

struct field **jpn4;
struct passive passivenewstart, passivenewfinish, passivedelstart, passivedelfinish, *passivenew0, *passivedel0;
struct active activenewstart, activenewfinish, activedelstart, activedelfinish, *activenew0, *activedel0, start, finish, start1, finish1;

struct levpravverhniz ***lev2, ***prav2, ***verh2, ***niz2, lev2newstart, lev2newfinish, lev2delstart, lev2delfinish, *lev2new0, *lev2del0;

struct kluch kluchlevstart, kluchlevfinish, kluchverhstart, kluchverhfinish, kluchpravstart, kluchpravfinish, kluchnizstart, kluchnizfinish, kluchstart3, kluchfinish3, kluchstart4, kluchfinish4 = {
        0, 0, NULL};

void allocate();

void deallocate();

void kluchFree(struct kluch);

void vved(void);

void resheniee();

void podgon(int, struct active *, struct active *, struct passive *, struct passive *,
            struct levpravverhniz *, struct levpravverhniz *);

int main2() {
    perebor = false;
    vved();
    er = true;
    resheniee();
    if (er == false) {
        printf("No solutions\n");
        return 0;
    }
    if (netu == 0) {
        kol = kol + 1;
        return 0;

    }
    perebor = true;
    m12 = (int *) malloc((netu + 5) * sizeof(int));
    m22 = (int *) malloc((netu + 5) * sizeof(int));
    kolic1 = (int *) malloc((netu + 5) * sizeof(int));
    kolic1[0] = 0;
    kolic = 0;
    er = true;
    kol = 0;


    start.next[1] = &finish;
    finish.prev[1] = &start;
    start1.next[1] = &finish1;
    finish1.prev[1] = &start1;


    {
        int mi, mj;
        struct active *a;

        activenew0 = (struct active *) malloc(sizeof(struct active));
        (*activenew0).cis = 0;
        activedel0 = (struct active *) malloc(sizeof(struct active));
        (*activedel0).cis = 0;

        for (int i = 0; i <= 1; i++) {
            (*activenew0).prev[i] = &activenewstart;
            (*activenew0).next[i] = &activenewfinish;
            activenewstart.next[i] = activenew0;
            activenewfinish.prev[i] = activenew0;

            (*activedel0).prev[i] = &activedelstart;
            (*activedel0).next[i] = &activedelfinish;
            activedelstart.next[i] = activedel0;
            activedelfinish.prev[i] = activedel0;
        }


        passivenew0 = (struct passive *) malloc(sizeof(struct passive));
        (*passivenew0).cis = 0;
        passivedel0 = (struct passive *) malloc(sizeof(struct passive));
        (*passivedel0).cis = 0;

        for (int ii = 0; ii <= 2; ii++) {
            (*passivenew0).prev[ii] = &passivenewstart;
            (*passivenew0).next[ii] = &passivenewfinish;
            passivenewstart.next[ii] = passivenew0;
            passivenewfinish.prev[ii] = passivenew0;

            (*passivedel0).prev[ii] = &passivedelstart;
            (*passivedel0).next[ii] = &passivedelfinish;
            passivedelstart.next[ii] = passivedel0;
            passivedelfinish.prev[ii] = passivedel0;
        }


        jpn4 = (struct field **) malloc((height + 2) * sizeof(struct field *));

        for (int i = 2; i <= height + 1; i++) {
            jpn4[i] = (struct field *) malloc((width + 2) * sizeof(struct field));

            for (int j = 2; j <= width + 1; j++)
                if (jpn[i][j] == empty) {

                    jpn4[i][j].start[0].next[1] = &(jpn4[i][j].finish[0]);
                    jpn4[i][j].start[1].next[1] = &(jpn4[i][j].finish[1]);
                    jpn4[i][j].start[0].next[2] = &(jpn4[i][j].finish[0]);
                    jpn4[i][j].start[1].next[2] = &(jpn4[i][j].finish[1]);

                    jpn4[i][j].finish[0].prev[1] = &(jpn4[i][j].start[0]);
                    jpn4[i][j].finish[1].prev[1] = &(jpn4[i][j].start[1]);
                    jpn4[i][j].finish[0].prev[2] = &(jpn4[i][j].start[0]);
                    jpn4[i][j].finish[1].prev[2] = &(jpn4[i][j].start[1]);
                    jpn4[i][j].notchoise = true;

                    a = (struct active *) malloc(sizeof(struct active));
                    (*a).cis = 1;
                    (*a).m1 = i;
                    (*a).m2 = j;
                    (*a).m3 = yes;
                    (*a).netu = 0;
                    (*a).notchoise = false;
                    (*a).next[0] = activenew0;
                    (*a).prev[0] = (*activenew0).prev[0];
                    (*(*activenew0).prev[0]).next[0] = a;
                    (*activenew0).prev[0] = a;
                    (*a).next[1] = &finish;
                    (*a).prev[1] = finish.prev[1];
                    (*finish.prev[1]).next[1] = a;
                    finish.prev[1] = a;
                    jpn4[i][j].element[1] = a;

                    a = (struct active *) malloc(sizeof(struct active));
                    (*a).cis = 1;
                    (*a).m1 = i;
                    (*a).m2 = j;
                    (*a).m3 = no;
                    (*a).netu = 0;
                    (*a).notchoise = false;
                    (*a).next[0] = activenew0;
                    (*a).prev[0] = (*activenew0).prev[0];
                    (*(*activenew0).prev[0]).next[0] = a;
                    (*activenew0).prev[0] = a;
                    (*a).next[1] = &finish;
                    (*a).prev[1] = finish.prev[1];
                    (*finish.prev[1]).next[1] = a;
                    finish.prev[1] = a;
                    jpn4[i][j].element[0] = a;


                }
        }
    }

    {
        struct levpravverhniz *a;

        lev2new0 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2new0).cis = 0;
        lev2del0 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2del0).cis = 0;

        for (int i = 0; i <= 1; i++) {
            (*lev2new0).prev[i] = &lev2newstart;
            (*lev2new0).next[i] = &lev2newfinish;
            lev2newstart.next[i] = lev2new0;
            lev2newfinish.prev[i] = lev2new0;

            (*lev2del0).prev[i] = &lev2delstart;
            (*lev2del0).next[i] = &lev2delfinish;
            lev2delstart.next[i] = lev2del0;
            lev2delfinish.prev[i] = lev2del0;
        }

        lev2 = (struct levpravverhniz ***) malloc((height + 2) * sizeof(struct levpravverhniz **));
        prav2 = (struct levpravverhniz ***) malloc((height + 2) * sizeof(struct levpravverhniz **));
        for (int i = 2; i <= height + 1; i++) {
            lev2[i] = (struct levpravverhniz **) malloc(
                    (rowNumCount[i] + 2) * sizeof(struct levpravverhniz *));
            prav2[i] = (struct levpravverhniz **) malloc(
                    (rowNumCount[i] + 2) * sizeof(struct levpravverhniz *));

            for (int j = 0; j <= rowNumCount[i] + 1; j++) {
                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = i;
                (*a).j = j;
                (*a).vid = 1;
                (*a).znach = lev[i][j];
                (*a).next[0] = lev2new0;
                (*a).prev[0] = (*lev2new0).prev[0];
                (*(*lev2new0).prev[0]).next[0] = a;
                (*lev2new0).prev[0] = a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                lev2[i][j] = a;

                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = i;
                (*a).j = j;
                (*a).vid = 2;
                (*a).znach = prav[i][j];
                (*a).next[0] = lev2new0;
                (*a).prev[0] = (*lev2new0).prev[0];
                (*(*lev2new0).prev[0]).next[0] = a;
                (*lev2new0).prev[0] = a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                prav2[i][j] = a;

            }

        }


        verh2 = (struct levpravverhniz ***) malloc((width + 2) * sizeof(struct levpravverhniz **));
        niz2 = (struct levpravverhniz ***) malloc((width + 2) * sizeof(struct levpravverhniz **));

        for (int ii = 2; ii <= width + 1; ii++) {
            verh2[ii] = (struct levpravverhniz **) malloc(
                    (colNumCount[ii] + 2) * sizeof(struct levpravverhniz *));
            niz2[ii] = (struct levpravverhniz **) malloc(
                    (colNumCount[ii] + 2) * sizeof(struct levpravverhniz *));

            for (int jj = 0; jj <= colNumCount[ii] + 1; jj++) {
                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 3;
                (*a).znach = verh[ii][jj];
                (*a).next[0] = lev2new0;
                (*a).prev[0] = (*lev2new0).prev[0];
                (*(*lev2new0).prev[0]).next[0] = a;
                (*lev2new0).prev[0] = a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                verh2[ii][jj] = a;

                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 4;
                (*a).znach = niz[ii][jj];
                (*a).next[0] = lev2new0;
                (*a).prev[0] = (*lev2new0).prev[0];
                (*(*lev2new0).prev[0]).next[0] = a;
                (*lev2new0).prev[0] = a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                niz2[ii][jj] = a;

            }

        }


    }


    podgon(1, activenew0, activedel0, passivenew0, passivedel0, lev2new0, lev2del0);

    return 0;


}

int mainy() {

    main2();

    return 0;
}

void allocate() {
    jpn = (unsigned char **) malloc((height + 4) * sizeof(unsigned char *));

    for (int i = 0; i < height + 4; i++) {
        jpn[i] = (unsigned char *) malloc((width + 4) * sizeof(unsigned char));
    }

    lev = (int **) malloc((height + 2) * sizeof(int *));
    prav = (int **) malloc((height + 2) * sizeof(int *));
    kluchlev = (bool **) malloc((height + 2) * sizeof(bool *));
    kluchprav = (bool **) malloc((height + 2) * sizeof(bool *));
    kluch12 = (bool *) malloc((height + 2) * sizeof(bool));

    for (int i = 2; i < height + 2; i++) {
        lev[i] = (int *) malloc((rowNumCount[i] + 2) * sizeof(int));
        prav[i] = (int *) malloc((rowNumCount[i] + 2) * sizeof(int));
        kluchlev[i] = (bool *) malloc((rowNumCount[i] + 2) * sizeof(bool));
        kluchprav[i] = (bool *) malloc((rowNumCount[i] + 2) * sizeof(bool));


    }

    verh = (int **) malloc((width + 2) * sizeof(int *));
    niz = (int **) malloc((width + 2) * sizeof(int *));
    kluchverh = (bool **) malloc((width + 2) * sizeof(bool *));
    kluchniz = (bool **) malloc((width + 2) * sizeof(bool *));
    kluch22 = (bool *) malloc((width + 2) * sizeof(bool));

    for (int i = 2; i < width + 2; i++) {
        verh[i] = (int *) malloc((colNumCount[i] + 2) * sizeof(int));
        niz[i] = (int *) malloc((colNumCount[i] + 2) * sizeof(int));
        kluchverh[i] = (bool *) malloc((colNumCount[i] + 2) * sizeof(bool));
        kluchniz[i] = (bool *) malloc((colNumCount[i] + 2) * sizeof(bool));


    }


    kluchlevfinish.prev = &kluchlevstart;
    kluchpravfinish.prev = &kluchpravstart;
    kluchverhfinish.prev = &kluchverhstart;
    kluchnizfinish.prev = &kluchnizstart;

    kluchfinish3.prev = &kluchstart3;
    kluchfinish4.prev = &kluchstart4;

}

void deallocate() {
    for (int i = 0; i < height + 4; i++) {
        free(jpn[i]);
        jpn[i] = NULL;
    }

    free(jpn);
    jpn = NULL;


    for (int i = 2; i < height + 2; i++) {
        free(lev[i]);
        lev[i] = NULL;
        free(prav[i]);
        prav[i] = NULL;
        free(kluchlev[i]);
        kluchlev[i] = NULL;
        free(kluchprav[i]);
        kluchprav[i] = NULL;
    }

    free(lev);
    lev = NULL;
    free(prav);
    prav = NULL;
    free(kluchlev);
    kluchlev = NULL;
    free(kluchprav);
    kluchprav = NULL;
    free(kluch12);
    kluch12 = NULL;

    for (int i = 2; i < width + 2; i++) {
        free(verh[i]);
        verh[i] = NULL;
        free(niz[i]);
        niz[i] = NULL;
        free(kluchverh[i]);
        kluchverh[i] = NULL;
        free(kluchniz[i]);
        kluchniz[i] = NULL;


    }

    free(verh);
    verh = NULL;
    free(niz);
    niz = NULL;
    free(kluchverh);
    kluchverh = NULL;
    free(kluchniz);
    kluchniz = NULL;
    free(kluch22);
    kluch22 = NULL;

    for (int i = 1; i < height + 1; i++) {
        free(rowNums[i + 1]);
        rowNums[i + 1] = NULL;
    }
    for (int i = 1; i < width + 1; i++) {
        free(colNums[i + 1]);
        colNums[i + 1] = NULL;
    }

    free(rowNums);
    rowNums = NULL;
    free(colNums);
    colNums = NULL;
    free(rowNumCount);
    rowNumCount = NULL;
    free(colNumCount);
    colNumCount = NULL;

    kluchFree(kluchpravfinish);
    kluchFree(kluchfinish4);
    kluchFree(kluchnizfinish);
    kluchFree(kluchlevfinish);
    kluchFree(kluchfinish3);
    kluchFree(kluchverhfinish);

}

void vved() {
    struct kluch *g;
    allocate();


    for (int i = 2; i < height + 2; i++)
        for (int j = 2; j < width + 2; j++)
            jpn[i][j] = empty;

    for (int i = 2; i < height + 2; i++) {
        kluch12[i] = false;
        jpn[i][1] = no;
        jpn[i][width + 2] = no;
        jpn[i][0] = yes;
        jpn[i][width + 3] = yes;

    }

    for (int i = 2; i < width + 2; i++) {
        kluch22[i] = false;
        jpn[1][i] = no;
        jpn[height + 2][i] = no;
        jpn[0][i] = yes;
        jpn[height + 3][i] = yes;

    }
    netu = height * width;
    sumstr = 0;
    sumsto = 0;
    for (int i = 2; i < height + 2; i++) {
        lev[i][0] = 0;
        prav[i][0] = 0;
        rowNums[i][0] = 1;
        int k = rowNumCount[i];
        lev[i][k + 1] = width + 3;
        prav[i][k + 1] = width + 3;
        rowNums[i][k + 1] = 1;
        for (int j = 1; j < k + 1; j++)
            sumstr = sumstr + rowNums[i][j];
        for (int j = 1; j < k + 1; j++) {
            kluchlev[i][j] = false;
            lev[i][j] = lev[i][j - 1] + rowNums[i][j - 1] + 1;

        }
        for (int j = k; j > 0; j--) {
            kluchprav[i][j] = false;
            prav[i][j] = prav[i][j + 1] - rowNums[i][j + 1] - 1;
        }
    }

    for (int i = 2; i < width + 2; i++) {
        verh[i][0] = 0;
        niz[i][0] = 0;
        colNums[i][0] = 1;
        int k = colNumCount[i];
        verh[i][k + 1] = height + 3;
        niz[i][k + 1] = height + 3;
        colNums[i][k + 1] = 1;
        for (int j = 1; j < k + 1; j++)
            sumsto = sumsto + colNums[i][j];
        for (int j = 1; j < k + 1; j++) {
            kluchverh[i][j] = false;
            verh[i][j] = verh[i][j - 1] + colNums[i][j - 1] + 1;

        }

        for (int j = k; j > 0; j--) {
            kluchniz[i][j] = false;
            niz[i][j] = niz[i][j + 1] - colNums[i][j + 1] - 1;
        }


    }

    for (int i = 2; i < height + 2; i++) {
        int k = rowNumCount[i];
        for (int j = 1; j < k + 1; j++)
            if ((prav[i][j] - lev[i][j] + 1) < (2 * rowNums[i][j]))
                for (int s = (prav[i][j] - rowNums[i][j] + 1); s < (lev[i][j] + rowNums[i][j]); s++)
                    if (jpn[i][s] == empty) {
                        jpn[i][s] = yes;
                        int j1 = 0;
                        while (i >= verh[s][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                            kluchverh[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[s] + 1;
                        while (i <= niz[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                            kluchniz[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[s] == false) {
                            kluch22[s] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;
                    }
        for (int j = 1; j <= k + 1; j++)
            if ((lev[i][j] - prav[i][j - 1] - 1) > 0)
                for (int s = (prav[i][j - 1] + 1); s < (lev[i][j]); s++)
                    if (jpn[i][s] == empty) {
                        jpn[i][s] = no;
                        int j1 = 0;
                        while (i >= verh[s][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                            kluchverh[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[s] + 1;
                        while (i <= niz[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                            kluchniz[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[s] == false) {
                            kluch22[s] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;
                    }

    }

    for (int i = 2; i < width + 2; i++) {
        int k = colNumCount[i];
        for (int j = 1; j < k + 1; j++)
            if ((niz[i][j] - verh[i][j] + 1) < (2 * colNums[i][j]))
                for (int s = (niz[i][j] - colNums[i][j] + 1); s < (verh[i][j] + colNums[i][j]); s++)
                    if (jpn[s][i] == empty) {
                        jpn[s][i] = yes;
                        int j1 = 0;
                        while (i >= lev[s][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                            kluchlev[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[s] + 1;
                        while (i <= prav[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                            kluchprav[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }

                        if (kluch12[s] == false) {
                            kluch12[s] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        netu--;
                    }
        for (int j = 1; j <= k + 1; j++)
            if ((verh[i][j] - niz[i][j - 1] - 1) > 0)
                for (int s = (niz[i][j - 1] + 1); s < (verh[i][j]); s++)
                    if (jpn[s][i] == empty) {
                        jpn[s][i] = no;
                        int j1 = 0;
                        while (i >= lev[s][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                            kluchlev[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[s] + 1;
                        while (i <= prav[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                            kluchprav[s][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).j = j1;

                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }

                        if (kluch12[s] == false) {
                            kluch12[s] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s;
                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }
                        netu--;
                    }

    }

}

void postroenie(unsigned char **jpn)
{
/*    char str[40];
    char strr[4000];

    sprintf(str, "%s", "\n");
    strcat(strr, str);

    for (int i=2; i < height + 2; i++){
        for (int j=2; j < width + 2; j++){
            sprintf(str, "%c",mas[jpn[i][j]]);
            strcat(strr, str);
        }
        sprintf(str, "%s", "\n");
        strcat(strr, str);
	}
//    sprintf(str, "%s", "  ");
//    strcat(strr, str);
    sprintf(str, "%s", "\n");
    strcat(strr, str);
    __android_log_write(ANDROID_LOG_DEBUG, "Solutions", strr);*/
}

/*void postroenie1(unsigned char **jpn)
{
	int i_0,j_0,s,t;
	unsigned int j0;
	const int k_0=1;
	char name2[200];
	FILE *outfile;
	sprintf(name2,"%s%s%d.xpm",name,"_",kol);
	outfile = fopen(name2,"w");
	//fprintf(outfile,"/%s/\n","* XPM *");
	//fprintf(outfile,"static char *test_xpm[] = {\n");
	//fprintf(outfile,"%s%d%s%d%s","\"",width*k_0," ",height*k_0," 2 1\",\n");
	//fprintf(outfile,"\"X c white\",\n");
	//fprintf(outfile,"\" c black\",\n");
	for (i_0=2;i_0<height+2;i_0++)
	{
		for (s=1;s<k_0+1;s++)
		{
			//fprintf(outfile,"\"");
			for (j_0=2;j_0<width+2;j_0++)
			{
				for (t=1;t<k_0+1;t++)
				{
					fprintf(outfile,"%c",mas[jpn[i_0][j_0]]);
				}
			}
			fprintf(outfile,"\n");
		}
	}
	//fprintf(outfile,"};");
	fclose(outfile);

}*/

void reshstr1(int i, int j) {
    int j1;
    int currentlev = lev[i][j];
    int currentlev2 = lev[i][j + 1];
    bool shift = false;
    bool shift2 = false;
    struct kluch *g, *g1;
    int s = currentlev;
    int p = s - 1;
    int q = s - 2;
    label:
    while ((s < currentlev + rowNums[i][j]) && (s <= prav[i][j])) {
        if (jpn[i][s] == no) {

            while (jpn[i][s] == no)
                s++;
            if ((prav[i][j] - s + 1) < (rowNums[i][j])) {
                er = false;
                return;
            }

            currentlev = s;
            shift = true;

            p = s - 1;
        }

        if (jpn[i][s] == yes) {
            int t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s <= prav[i][j])) {
                s++;
                t++;
            }
            if (t > rowNums[i][j]) {
                if ((prav[i][j] - s + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentlev = s;
                shift = true;


            } else if (currentlev < s - rowNums[i][j]) {
                if ((prav[i][j] - (s - rowNums[i][j]) + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentlev = s - rowNums[i][j];
                shift = true;


            }
        }

        while ((jpn[i][s] == empty) && (s <= currentlev + rowNums[i][j]) && (s <= prav[i][j]))
            s++;

    }

    if (currentlev2 < currentlev + rowNums[i][j] + 1) {
        if ((prav[i][j + 1] - (currentlev + rowNums[i][j] + 1) + 1) < (rowNums[i][j + 1])) {
            er = false;
            return;
        }

        currentlev2 = currentlev + rowNums[i][j] + 1;
        shift2 = true;

    }

    while ((s < currentlev2) && (s <= prav[i][j])) {

        if (jpn[i][s] == yes) {
            int t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s <= prav[i][j])) {
                s++;
                t++;
            }

            if (t > rowNums[i][j]) {
                if ((prav[i][j] - s + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentlev = s;
                shift = true;
                goto label;
            }

            if (currentlev < s - rowNums[i][j]) {

                if ((prav[i][j] - (s - rowNums[i][j]) + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }


                currentlev = s - rowNums[i][j];
                shift = true;
                s = currentlev;
                p = s - 1;
                q = s - 2;
                goto label;


            }

            if (currentlev2 < currentlev + rowNums[i][j] + 1) {
                if ((prav[i][j + 1] - (currentlev + rowNums[i][j] + 1) + 1) < (rowNums[i][j + 1])) {
                    er = false;
                    return;
                }

                currentlev2 = currentlev + rowNums[i][j] + 1;
                shift2 = true;

            }
        }

        if (jpn[i][s] == no) {
            if (s - p - 1 < rowNums[i][j]) {
                if (q > p) {
                    while ((jpn[i][s] == no) && (s <= prav[i][j]))
                        s++;
                    if ((prav[i][j] - s + 1) < (rowNums[i][j])) {
                        er = false;
                        return;
                    }


                    currentlev = s;
                    shift = true;

                    p = s - 1;
                    goto label;
                }
            }
        }

        if (jpn[i][s] == no) {
            while ((jpn[i][s] == no) && (s <= prav[i][j]))
                s++;
            p = s - 1;
        }
        while ((jpn[i][s] == empty) && (s < currentlev2) && (s <= prav[i][j]))
            s++;
    }

    if ((lev[i][rowNumCount[i] + 1] != (width + 3))) {
        er = false;
        return;
    }

    if (shift) {
        if ((prav[i][j] - currentlev + 1) < (rowNums[i][j])) {
            er = false;
            return;
        }
        if ((prav[i][j] - currentlev + 1) < (2 * rowNums[i][j])) {
            for (s = (prav[i][j] - rowNums[i][j] + 1); s < (currentlev + rowNums[i][j]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {
                    jpn[i][s] = yes;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }
        }

        if (currentlev - prav[i][j - 1] > 1)
            for (s = prav[i][j - 1] + 1; s < currentlev; s++) {
                if (jpn[i][s] == yes) {

                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {

                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }


    }
    if (shift2) {
        if ((prav[i][j + 1] - currentlev2 + 1) < (rowNums[i][j + 1])) {
            er = false;
            return;
        }
        if ((prav[i][j + 1] - currentlev2 + 1) < (2 * rowNums[i][j + 1])) {
            for (s = (prav[i][j + 1] - rowNums[i][j + 1] + 1); s < (currentlev2 + rowNums[i][j + 1]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {
                    jpn[i][s] = yes;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }
        }
        if (currentlev2 - prav[i][j] > 1)
            for (s = prav[i][j] + 1; s < currentlev2; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {

                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }

    }
    if (shift) {
        lev[i][j] = currentlev;
        if ((j > 1) && (kluchlev[i][j - 1] == false)) {
            kluchlev[i][j - 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j - 1;
            (*g).prev = kluchlevfinish.prev;
            kluchlevfinish.prev = g;
        }
        if (kluch12[i] == false) {
            kluch12[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }

        if ((perebor) && (lev2[i][j] != NULL)) {
            (*lev2[i][j]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = lev2[i][j];
            lev2[i][j] = NULL;
        }

    }
    if (shift2) {
        lev[i][j + 1] = currentlev2;
        if (kluchlev[i][j + 1] == false) {
            kluchlev[i][j + 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j + 1;
            (*g).prev = kluchlevfinish.prev;
            kluchlevfinish.prev = g;
        }

        if (kluch12[i] == false) {
            kluch12[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }

        if ((perebor) && (lev2[i][j + 1] != NULL)) {
            (*lev2[i][j + 1]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = lev2[i][j + 1];
            lev2[i][j + 1] = NULL;
        }

    }

}

void reshstr2(int i, int j) {
    int s, p, q, t, c, a, b, j1;
    int currentprav = prav[i][j];
    int currentprav2 = prav[i][j - 1];
    bool shift = false;
    bool shift2 = false;
    struct kluch *g;
    s = currentprav;
    p = s + 1;
    q = s + 2;
    label:
    while ((s > currentprav - rowNums[i][j]) && (s >= lev[i][j])) {

        if (jpn[i][s] == no) {
            while (jpn[i][s] == no)
                s--;
            if ((s - lev[i][j] + 1) < (rowNums[i][j])) {
                er = false;
                return;
            }

            currentprav = s;
            shift = true;

            p = s + 1;
        }

        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s >= lev[i][j])) {
                s--;
                t++;
            }
            if (t > rowNums[i][j]) {
                if ((s - lev[i][j] + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentprav = s;
                shift = true;


            } else if (currentprav > s + rowNums[i][j]) {
                if (((s + rowNums[i][j]) - lev[i][j] + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentprav = s + rowNums[i][j];
                shift = true;

            }

        }
        while ((jpn[i][s] == empty) && (s >= currentprav - rowNums[i][j]) && (s >= lev[i][j]))
            s--;
    }

    if (currentprav2 > currentprav - rowNums[i][j] - 1) {
        if (((currentprav - rowNums[i][j] - 1) - lev[i][j - 1] + 1) < (rowNums[i][j - 1])) {
            er = false;
            return;
        }

        currentprav2 = currentprav - rowNums[i][j] - 1;
        shift2 = true;

    }

    while ((s > currentprav2) && (s >= lev[i][j])) {
        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s >= lev[i][j])) {
                s--;
                t++;
            }
            if (t > rowNums[i][j]) {
                if ((s - lev[i][j] + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentprav = s;
                shift = true;

                goto label;
            }
            if (currentprav > s + rowNums[i][j]) {
                if (((s + rowNums[i][j]) - lev[i][j] + 1) < (rowNums[i][j])) {
                    er = false;
                    return;
                }

                currentprav = s + rowNums[i][j];
                shift = true;
                s = currentprav;
                p = s + 1;
                q = s + 2;

                goto label;


            }
            if (currentprav2 > currentprav - rowNums[i][j] - 1) {
                if (((currentprav - rowNums[i][j] - 1) - lev[i][j - 1] + 1) < (rowNums[i][j - 1])) {
                    er = false;
                    return;
                }

                currentprav2 = currentprav - rowNums[i][j] - 1;
                shift2 = true;

            }
        }

        if (jpn[i][s] == no) {
            if (p - s - 1 < rowNums[i][j]) {
                if (q < p) {
                    while ((jpn[i][s] == no) && (s >= lev[i][j]))
                        s--;
                    if ((s - lev[i][j] + 1) < (rowNums[i][j])) {
                        er = false;
                        return;
                    }

                    currentprav = s;
                    shift = true;

                    p = s + 1;
                    goto label;
                }

            }
        }

        if (jpn[i][s] == no) {
            while ((jpn[i][s] == no) && (s >= lev[i][j]))
                s--;
            p = s + 1;
        }

        while ((jpn[i][s] == empty) && (s > currentprav2) && (s >= lev[i][j]))
            s--;
    }
    if ((prav[i][0] != 0)) {
        er = false;
        return;
    }

    if (shift) {
        if ((currentprav - lev[i][j] + 1) < (rowNums[i][j])) {
            er = false;
            return;
        }
        if ((currentprav - lev[i][j] + 1) < (2 * rowNums[i][j])) {
            for (s = (currentprav - rowNums[i][j] + 1); s < (lev[i][j] + rowNums[i][j]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {
                    jpn[i][s] = yes;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }
        }

        if (lev[i][j + 1] - currentprav > 1)
            for (s = currentprav + 1; s < lev[i][j + 1]; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {

                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }


    }
    if (shift2) {
        if ((currentprav2 - lev[i][j - 1] + 1) < (rowNums[i][j - 1])) {
            er = false;
            return;
        }
        if ((currentprav2 - lev[i][j - 1] + 1) < (2 * rowNums[i][j - 1])) {
            for (s = (currentprav2 - rowNums[i][j - 1] + 1); s < (lev[i][j - 1] + rowNums[i][j - 1]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {
                    jpn[i][s] = yes;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }
        }
        if (lev[i][j] - currentprav2 > 1)
            for (s = currentprav2 + 1; s < lev[i][j]; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == empty) {

                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }

                }
            }

    }
    if (shift) {
        prav[i][j] = currentprav;
        if ((j < rowNumCount[i]) && (kluchprav[i][j + 1] == false)) {
            kluchprav[i][j + 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j + 1;
            (*g).prev = kluchpravfinish.prev;
            kluchpravfinish.prev = g;
        }

        if (kluch12[i] == false) {
            kluch12[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }

        if ((perebor) && (prav2[i][j] != NULL)) {
            (*prav2[i][j]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = prav2[i][j];
            prav2[i][j] = NULL;
        }
    }
    if (shift2) {
        prav[i][j - 1] = currentprav2;
        if (kluchprav[i][j - 1] == false) {
            kluchprav[i][j - 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j - 1;
            (*g).prev = kluchpravfinish.prev;
            kluchpravfinish.prev = g;
        }
        if (kluch12[i] == false) {
            kluch12[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }

        if ((perebor) && (prav2[i][j - 1] != NULL)) {
            (*prav2[i][j - 1]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = prav2[i][j - 1];
            prav2[i][j - 1] = NULL;
        }
    }

}


void reshsto1(int i, int j) {
    int j1;
    int currentverh = verh[i][j];
    int currentverh2 = verh[i][j + 1];
    bool shift = false;
    bool shift2 = false;
    struct kluch *g;
    int s = currentverh;
    int p = s - 1;
    int q = s - 2;
    label:
    while ((s < currentverh + colNums[i][j]) && (s <= niz[i][j])) {
        if (jpn[s][i] == no) {

            while (jpn[s][i] == no)
                s++;

            if ((niz[i][j] - s + 1) < (colNums[i][j])) {
                er = false;
                return;
            }

            currentverh = s;
            shift = true;

            p = s - 1;
        }

        if (jpn[s][i] == yes) {
            int t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s <= niz[i][j])) {
                s++;
                t++;
            }
            if (t > colNums[i][j]) {
                if ((niz[i][j] - s + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentverh = s;
                shift = true;

            } else if (currentverh < s - colNums[i][j]) {
                if ((niz[i][j] - (s - colNums[i][j]) + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentverh = s - colNums[i][j];
                shift = true;

            }
        }
        while ((jpn[s][i] == empty) && (s <= currentverh + colNums[i][j]) && (s <= niz[i][j]))
            s++;
    }

    if (currentverh2 < currentverh + colNums[i][j] + 1) {
        if ((niz[i][j + 1] - (currentverh + colNums[i][j] + 1) + 1) < (colNums[i][j + 1])) {
            er = false;
            return;
        }
        currentverh2 = currentverh + colNums[i][j] + 1;
        shift2 = true;
    }

    while ((s < currentverh2) && (s <= niz[i][j])) {
        if (jpn[s][i] == yes) {
            int t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s <= niz[i][j])) {
                s++;
                t++;
            }

            if (t > colNums[i][j]) {
                if ((niz[i][j] - s + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentverh = s;
                shift = true;

                goto label;
            }

            if (currentverh < s - colNums[i][j]) {
                if ((niz[i][j] - (s - colNums[i][j]) + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentverh = s - colNums[i][j];
                shift = true;
                s = currentverh;
                p = s - 1;
                q = s - 2;

                goto label;

            }

            if (currentverh2 < currentverh + colNums[i][j] + 1) {
                if ((niz[i][j + 1] - (currentverh + colNums[i][j] + 1) + 1) < (colNums[i][j + 1])) {
                    er = false;
                    return;
                }

                currentverh2 = currentverh + colNums[i][j] + 1;
                shift2 = true;
            }
        }

        if (jpn[s][i] == no) {
            if (s - p - 1 < colNums[i][j]) {
                if (q > p) {
                    while ((jpn[s][i] == no) && (s <= niz[i][j]))
                        s++;
                    if ((niz[i][j] - s + 1) < (colNums[i][j])) {
                        er = false;
                        return;
                    }

                    currentverh = s;
                    shift = true;
                    p = s - 1;
                    goto label;
                }
            }
        }

        if (jpn[s][i] == no) {
            while ((jpn[s][i] == no) && (s <= niz[i][j]))
                s++;
            p = s - 1;
        }
        while ((jpn[s][i] == empty) && (s < currentverh2) && (s <= niz[i][j]))
            s++;
    }

    if ((verh[i][colNumCount[i] + 1] != (height + 3))) {
        er = false;
        return;
    }

    if (shift) {
        if ((niz[i][j] - currentverh + 1) < (colNums[i][j])) {
            er = false;
            return;
        }
        if ((niz[i][j] - currentverh + 1) < (2 * colNums[i][j])) {
            for (s = (niz[i][j] - colNums[i][j] + 1); s < (currentverh + colNums[i][j]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {
                    jpn[s][i] = yes;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }
        }

        if (currentverh - niz[i][j - 1] > 1)
            for (s = niz[i][j - 1] + 1; s < currentverh; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = no;

                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }


    }
    if (shift2) {
        if ((niz[i][j + 1] - currentverh2 + 1) < (colNums[i][j + 1])) {
            er = false;
            return;
        }
        if ((niz[i][j + 1] - currentverh2 + 1) < (2 * colNums[i][j + 1])) {
            for (s = (niz[i][j + 1] - colNums[i][j + 1] + 1); s < (currentverh2 + colNums[i][j + 1]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {
                    jpn[s][i] = yes;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }
        }
        if (currentverh2 - niz[i][j] > 1)
            for (s = niz[i][j] + 1; s < currentverh2; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = no;

                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }

    }
    if (shift) {
        verh[i][j] = currentverh;
        if ((j > 1) && (kluchverh[i][j - 1] == false)) {
            kluchverh[i][j - 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j - 1;
            (*g).prev = kluchverhfinish.prev;
            kluchverhfinish.prev = g;
        }

        if (kluch22[i] == false) {
            kluch22[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        if ((perebor) && (verh2[i][j] != NULL)) {
            (*verh2[i][j]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = verh2[i][j];
            verh2[i][j] = NULL;
        }
    }
    if (shift2) {
        verh[i][j + 1] = currentverh2;
        if (kluchverh[i][j + 1] == false) {
            kluchverh[i][j + 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j + 1;
            (*g).prev = kluchverhfinish.prev;
            kluchverhfinish.prev = g;
        }
        if (kluch22[i] == false) {
            kluch22[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        if ((perebor) && (verh2[i][j + 1] != NULL)) {
            (*verh2[i][j + 1]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = verh2[i][j + 1];
            verh2[i][j + 1] = NULL;
        }
    }

}

void reshsto2(int i, int j) {
    int j1;
    int currentniz = niz[i][j];
    int currentniz2 = niz[i][j - 1];
    bool shift = false;
    bool shift2 = false;
    struct kluch *g, *g1;
    int s = currentniz;
    int p = s + 1;
    int q = s + 2;
    label:
    while ((s > currentniz - colNums[i][j]) && (s >= verh[i][j])) {
        if (jpn[s][i] == no) {
            while (jpn[s][i] == no)
                s--;
            if ((s - verh[i][j] + 1) < (colNums[i][j])) {
                er = false;
                return;
            }

            currentniz = s;
            shift = true;

            p = s + 1;
        }

        if (jpn[s][i] == yes) {
            int t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s >= verh[i][j])) {
                s--;
                t++;
            }
            if (t > colNums[i][j]) {
                if ((s - verh[i][j] + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentniz = s;
                shift = true;

            } else if (currentniz > s + colNums[i][j]) {
                if (((s + colNums[i][j]) - verh[i][j] + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentniz = s + colNums[i][j];
                shift = true;
            }
        }

        while ((jpn[s][i] == empty) && (s >= currentniz - colNums[i][j]) && (s >= verh[i][j]))
            s--;

    }

    if (currentniz2 > currentniz - colNums[i][j] - 1) {

        if (((currentniz - colNums[i][j] - 1) - verh[i][j - 1] + 1) < (colNums[i][j - 1])) {
            er = false;
            return;
        }

        currentniz2 = currentniz - colNums[i][j] - 1;
        shift2 = true;

    }

    while ((s > currentniz2) && (s >= verh[i][j])) {

        if (jpn[s][i] == yes) {
            int t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s >= verh[i][j])) {
                s--;
                t++;
            }
            if (t > colNums[i][j]) {
                if ((s - verh[i][j] + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentniz = s;
                shift = true;

                goto label;
            }
            if (currentniz > s + colNums[i][j]) {
                if (((s + colNums[i][j]) - verh[i][j] + 1) < (colNums[i][j])) {
                    er = false;
                    return;
                }

                currentniz = s + colNums[i][j];
                shift = true;

                s = currentniz;
                p = s + 1;
                q = s + 2;

                goto label;


            }
            if (currentniz2 > currentniz - colNums[i][j] - 1) {
                if (((currentniz - colNums[i][j] - 1) - verh[i][j - 1] + 1) < (colNums[i][j - 1])) {
                    er = false;
                    return;
                }

                currentniz2 = currentniz - colNums[i][j] - 1;
                shift2 = true;

            }
        }

        if (jpn[s][i] == no) {
            if (p - s - 1 < colNums[i][j]) {
                if (q < p) {
                    while ((jpn[s][i] == no) && (s >= verh[i][j]))
                        s--;
                    if ((s - verh[i][j] + 1) < (colNums[i][j])) {
                        er = false;
                        return;
                    }

                    currentniz = s;
                    shift = true;

                    p = s + 1;
                    goto label;
                }

            }
        }

        if (jpn[s][i] == no) {
            while ((jpn[s][i] == no) && (s >= verh[i][j]))
                s--;
            p = s + 1;
        }

        while ((jpn[s][i] == empty) && (s > currentniz2) && (s >= verh[i][j]))
            s--;
    }
    if ((niz[i][0] != 0)) {
        er = false;
        return;
    }

    if (shift) {
        if ((currentniz - verh[i][j] + 1) < (colNums[i][j])) {
            er = false;
            return;
        }
        if ((currentniz - verh[i][j] + 1) < (2 * colNums[i][j])) {
            for (s = (currentniz - colNums[i][j] + 1); s < (verh[i][j] + colNums[i][j]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = yes;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }
        }

        if (verh[i][j + 1] - currentniz > 1)
            for (s = currentniz + 1; s < verh[i][j + 1]; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = no;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }


    }
    if (shift2) {
        if ((currentniz2 - verh[i][j - 1] + 1) < (colNums[i][j - 1])) {
            er = false;
            return;
        }
        if ((currentniz2 - verh[i][j - 1] + 1) < (2 * colNums[i][j - 1])) {
            for (s = (currentniz2 - colNums[i][j - 1] + 1); s < (verh[i][j - 1] + colNums[i][j - 1]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = yes;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }
        }
        if (verh[i][j] - currentniz2 > 1)
            for (s = currentniz2 + 1; s < verh[i][j]; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == empty) {

                    jpn[s][i] = no;
                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }
                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = s;
                        m22[kolic] = i;
                    }

                }
            }

    }
    if (shift) {
        niz[i][j] = currentniz;
        if ((j < colNumCount[i]) && (kluchniz[i][j + 1] == false)) {
            kluchniz[i][j + 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j + 1;
            (*g).prev = kluchnizfinish.prev;
            kluchnizfinish.prev = g;
        }

        if (kluch22[i] == false) {
            kluch22[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        if ((perebor) && (niz2[i][j] != NULL)) {
            (*niz2[i][j]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = niz2[i][j];
            niz2[i][j] = NULL;
        }
    }
    if (shift2) {
        niz[i][j - 1] = currentniz2;
        if (kluchniz[i][j - 1] == false) {
            kluchniz[i][j - 1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;
            (*g).j = j - 1;
            (*g).prev = kluchnizfinish.prev;
            kluchnizfinish.prev = g;
        }

        if (kluch22[i] == false) {
            kluch22[i] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        if ((perebor) && (niz2[i][j - 1] != NULL)) {
            (*niz2[i][j - 1]).prev[1] = lev2delfinish.prev[1];
            lev2delfinish.prev[1] = niz2[i][j - 1];
            niz2[i][j - 1] = NULL;
        }
    }

}

void reshstr3(int i, int j, int (*p), int (*q)) {
    int s, a, b, min, max, j1, t;
    struct kluch *g;
    s = lev[i][j];
    if (prav[i][j - 1] < lev[i][j])
        (*p) = s - 1;
    while ((s < lev[i][j + 1]) && (s <= prav[i][j])) {
        label:
        if (jpn[i][s] == no) {
            if ((*q) < (*p)) {
                a = j;
                while ((prav[i][a] >= s - 1) && (rowNums[i][a] > (s - (*p) - 1)))
                    a--;
                if ((prav[i][a] < s - 1) && (rowNums[i][a + 1] > (s - (*p) - 1)))
                    for (b = (*p) + 1; b < s; b++) {
                        jpn[i][b] = no;
                        j1 = 0;
                        while (b >= lev[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                            kluchlev[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                            kluchprav[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }

                        if (kluch12[i] == false) {
                            kluch12[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }
                        j1 = 0;
                        while (i >= verh[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                            kluchverh[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                            kluchniz[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[b] == false) {
                            kluch22[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;
                        if (perebor) {
                            kolic++;
                            m12[kolic] = i;
                            m22[kolic] = b;
                        }

                    }
            }

            while ((jpn[i][s] == no) && (s < lev[i][j + 1]) && (s <= prav[i][j]))
                s++;
            (*p) = s - 1;
        }
        if ((s < lev[i][j + 1]) && (s <= prav[i][j]) && (jpn[i][s] == yes)) {
            if ((s < lev[i][j + 1]) && (s <= prav[i][j]) && (jpn[i][s] == yes) && ((*q) < (*p))) {
                (*q) = s;
                while (jpn[i][s] == yes)
                    s++;
                max = rowNums[i][j];
                a = j - 1;
                while (prav[i][a] >= s - 1) {
                    if (rowNums[i][a] > max)
                        max = rowNums[i][a];
                    a--;
                }
                if (s - max - 2 - (*p) >= 0) {
                    min = 0;
                    if (prav[i][j - 1] >= (*q) - 2)
                        min = rowNums[i][j - 1];
                    a = j - 2;
                    while (prav[i][a] > (*p)) {
                        if ((rowNums[i][a] < min) && (rowNums[i][a + 1] >= s - (*q)))
                            min = rowNums[i][a];
                        a--;
                    }
                    if ((*q) - 2 - (*p) < min) {
                        for (b = (*p) + 1; b < s - max; b++) {
                            if (jpn[i][b] == yes) {
                                er = false;
                                return;
                            }
                            jpn[i][b] = no;
                            j1 = 0;
                            while (b >= lev[i][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                                kluchlev[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchlevfinish.prev;
                                kluchlevfinish.prev = g;
                            }
                            j1 = rowNumCount[i] + 1;
                            while (b <= prav[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                                kluchprav[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchpravfinish.prev;
                                kluchpravfinish.prev = g;
                            }
                            if (kluch12[i] == false) {
                                kluch12[i] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;

                                (*g).prev = kluchfinish3.prev;
                                kluchfinish3.prev = g;
                            }

                            j1 = 0;
                            while (i >= verh[b][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                                kluchverh[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchverhfinish.prev;
                                kluchverhfinish.prev = g;
                            }
                            j1 = colNumCount[b] + 1;
                            while (i <= niz[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                                kluchniz[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchnizfinish.prev;
                                kluchnizfinish.prev = g;
                            }

                            if (kluch22[b] == false) {
                                kluch22[b] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;

                                (*g).prev = kluchfinish4.prev;
                                kluchfinish4.prev = g;
                            }


                            netu--;
                            if (perebor) {
                                kolic++;
                                m12[kolic] = i;
                                m22[kolic] = b;
                            }

                        }
                        (*p) = s - max - 1;
                    }
                }
                min = rowNums[i][j];
                a = j - 1;
                while (prav[i][a] >= s - 1) {
                    if ((rowNums[i][a] < min) && (rowNums[i][a] >= s - (*q)))
                        min = rowNums[i][a];
                    a--;
                }
                for (b = s; b < (*p) + min + 1; b++) {
                    if (jpn[i][b] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[i][b] == empty) {
                        if (jpn[i][b] == no) {
                            er = false;
                            return;
                        }
                        jpn[i][b] = yes;
                        j1 = 0;
                        while (b >= lev[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                            kluchlev[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                            kluchprav[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }
                        if (kluch12[i] == false) {
                            kluch12[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        j1 = 0;
                        while (i >= verh[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                            kluchverh[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                            kluchniz[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[b] == false) {
                            kluch22[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }


                        netu--;
                        if (perebor) {
                            kolic++;
                            m12[kolic] = i;
                            m22[kolic] = b;
                        }

                    }
                }
                while (jpn[i][s] == yes)
                    s++;
            }
            if ((s < lev[i][j + 1]) && (s <= prav[i][j]) && (jpn[i][s] == yes) && ((*q) > (*p))) {
                int k = (*q);
                while (jpn[i][k] == yes)
                    k++;
                if ((k == s - 1) && (jpn[i][k] == empty)) {
                    k = s;
                    while (jpn[i][k] == yes)
                        k++;
                    if (lev[i][j] <= (*q))
                        a = j;
                    else
                        a = j - 1;
                    while ((prav[i][a] >= (*q)) && ((*q) + rowNums[i][a] < k - 1))
                        a--;
                    if (prav[i][a] < (*q)) {
                        if (jpn[i][s - 1] == yes) {
                            er = false;
                            return;
                        }

                        jpn[i][s - 1] = no;
                        j1 = 0;
                        while (s - 1 >= lev[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                            kluchlev[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[i] + 1;
                        while (s - 1 <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                            kluchprav[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }
                        if (kluch12[i] == false) {
                            kluch12[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        j1 = 0;
                        while (i >= verh[s - 1][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[s - 1][j1] == false)) {
                            kluchverh[s - 1][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[s - 1] + 1;
                        while (i <= niz[s - 1][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[s - 1] + 1) && (kluchniz[s - 1][j1] == false)) {
                            kluchniz[s - 1][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[s - 1] == false) {
                            kluch22[s - 1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;

                        if (perebor) {
                            kolic++;
                            m12[kolic] = i;
                            m22[kolic] = s - 1;
                        }
                        s = s - 1;
                        goto label;

                    }
                }
            }
            if ((s < lev[i][j + 1]) && (s <= prav[i][j]) && (jpn[i][s] == yes)) {
                (*q) = s;
                while (jpn[i][s] == yes)
                    s++;
            }
            t = s - (*q);
            if ((t == rowNums[i][j]) && (jpn[i][s] == empty)) {
                a = j;
                while ((prav[i][a] >= s - 1) &&
                       ((rowNums[i][a] <= rowNums[i][j]) || (prav[i][a] == s - 1)))
                    a--;
                if ((prav[i][a] < s - 1) &&
                    ((rowNums[i][a + 1] <= rowNums[i][j]) || (prav[i][a + 1] == s - 1))) {
                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }
                }
            }

        }

        while ((jpn[i][s] == empty) && (s < lev[i][j + 1]) && (s <= prav[i][j]))
            s++;

    }
}

void reshstr4(int i, int j, int (*p), int (*q)) {
    int s, a, b, min, max, j1, t;
    struct kluch *g;
    s = prav[i][j];
    if (lev[i][j + 1] > prav[i][j]) {
        (*p) = s + 1;
    }
    while ((s > prav[i][j - 1]) && (s >= lev[i][j])) {
        if (jpn[i][s] == no) {
            if ((*q) > (*p)) {
                a = j;
                while ((lev[i][a] <= s + 1) && (rowNums[i][a] > ((*p) - s - 1)))
                    a++;
                if ((lev[i][a] > s + 1) && (rowNums[i][a - 1] > ((*p) - s - 1)))
                    for (b = (*p) - 1; b > s; b--) {
                        jpn[i][b] = no;
                        j1 = 0;
                        while (b >= lev[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                            kluchlev[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                            kluchprav[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }

                        if (kluch12[i] == false) {
                            kluch12[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }
                        j1 = 0;
                        while (i >= verh[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                            kluchverh[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                            kluchniz[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[b] == false) {
                            kluch22[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;
                        if (perebor) {
                            kolic++;
                            m12[kolic] = i;
                            m22[kolic] = b;
                        }

                    }
            }

            while ((jpn[i][s] == no) && (s > prav[i][j - 1]) && (s >= lev[i][j]))
                s--;
            (*p) = s + 1;
        }
        if ((s > prav[i][j - 1]) && (s >= lev[i][j]) && (jpn[i][s] == yes)) {
            if ((s > prav[i][j - 1]) && (s >= lev[i][j]) && (jpn[i][s] == yes) && ((*q) > (*p))) {
                (*q) = s;
                while (jpn[i][s] == yes)
                    s--;
                max = rowNums[i][j];
                a = j + 1;
                while (lev[i][a] <= s + 1) {
                    if (rowNums[i][a] > max)
                        max = rowNums[i][a];
                    a++;
                }
                if ((*p) - s - max - 2 >= 0) {
                    min = 0;
                    if (lev[i][j + 1] <= (*q) + 2)
                        min = rowNums[i][j + 1];
                    a = j + 2;
                    while (lev[i][a] < (*p)) {
                        if ((rowNums[i][a] < min) && (rowNums[i][a - 1] >= (*q) - s))
                            min = rowNums[i][a];
                        a++;
                    }
                    if ((*p) - (*q) - 2 < min)
                        for (b = s + max + 1; b < (*p); b++) {
                            if (jpn[i][b] == yes) {
                                er = false;
                                return;
                            }
                            jpn[i][b] = no;
                            j1 = 0;
                            while (b >= lev[i][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                                kluchlev[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchlevfinish.prev;
                                kluchlevfinish.prev = g;
                            }
                            j1 = rowNumCount[i] + 1;
                            while (b <= prav[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                                kluchprav[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchpravfinish.prev;
                                kluchpravfinish.prev = g;
                            }
                            if (kluch12[i] == false) {
                                kluch12[i] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;

                                (*g).prev = kluchfinish3.prev;
                                kluchfinish3.prev = g;
                            }

                            j1 = 0;
                            while (i >= verh[b][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                                kluchverh[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchverhfinish.prev;
                                kluchverhfinish.prev = g;
                            }
                            j1 = colNumCount[b] + 1;
                            while (i <= niz[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                                kluchniz[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchnizfinish.prev;
                                kluchnizfinish.prev = g;
                            }

                            if (kluch22[b] == false) {
                                kluch22[b] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;

                                (*g).prev = kluchfinish4.prev;
                                kluchfinish4.prev = g;
                            }

                            netu--;
                            if (perebor) {
                                kolic++;
                                m12[kolic] = i;
                                m22[kolic] = b;
                            }

                        }
                }
                min = rowNums[i][j];
                a = j + 1;
                while (lev[i][a] <= s + 1) {
                    if ((rowNums[i][a] < min) && (rowNums[i][a] >= (*q) - s))
                        min = rowNums[i][a];
                    a++;
                }
                for (b = s; b > (*p) - min - 1; b--) {
                    if (jpn[i][b] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[i][b] == empty) {
                        if (jpn[i][b] == no) {
                            er = false;
                            return;
                        }
                        jpn[i][b] = yes;
                        j1 = 0;
                        while (b >= lev[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                            kluchlev[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                            kluchprav[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }
                        if (kluch12[i] == false) {
                            kluch12[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        j1 = 0;
                        while (i >= verh[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[b][j1] == false)) {
                            kluchverh[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }
                        j1 = colNumCount[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[b] + 1) && (kluchniz[b][j1] == false)) {
                            kluchniz[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        if (kluch22[b] == false) {
                            kluch22[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        netu--;
                        if (perebor) {
                            kolic++;
                            m12[kolic] = i;
                            m22[kolic] = b;
                        }

                    }
                }
                while (jpn[i][s] == yes)
                    s--;

            }
            if ((s > prav[i][j - 1]) && (s >= lev[i][j]) && (jpn[i][s] == yes)) {
                (*q) = s;
                while (jpn[i][s] == yes)
                    s--;
            }
            t = (*q) - s;
            if ((t == rowNums[i][j]) && (jpn[i][s] == empty)) {
                a = j;
                while ((lev[i][a] <= s + 1) && ((rowNums[i][a] <= rowNums[i][j]) || (lev[i][a] == s + 1)))
                    a++;
                if ((lev[i][a] > s + 1) &&
                    ((rowNums[i][a - 1] <= rowNums[i][j]) || (lev[i][a - 1] == s + 1))) {
                    jpn[i][s] = no;
                    j1 = 0;
                    while (s >= lev[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[i][j1] == false)) {
                        kluchlev[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[i] + 1) && (kluchprav[i][j1] == false)) {
                        kluchprav[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }

                    if (kluch12[i] == false) {
                        kluch12[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }
                    j1 = 0;
                    while (i >= verh[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[s][j1] == false)) {
                        kluchverh[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }
                    j1 = colNumCount[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[s] + 1) && (kluchniz[s][j1] == false)) {
                        kluchniz[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    if (kluch22[s] == false) {
                        kluch22[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    netu--;
                    if (perebor) {
                        kolic++;
                        m12[kolic] = i;
                        m22[kolic] = s;
                    }
                }
            }

        }
        while ((jpn[i][s] == empty) && (s > prav[i][j - 1]) && (s >= lev[i][j]))
            s--;
    }
}

void reshstrdva(int i) {
    int j, p, q;
    q = 0;
    for (j = 1; j < rowNumCount[i] + 1; j++) {
        if ((prav[i][j] - lev[i][j] + 1) > (rowNums[i][j]))
            reshstr3(i, j, &p, &q);
        else
            q = lev[i][j];
        if (er == false)
            return;
    }
    q = width + 3;
    for (j = rowNumCount[i]; j > 0; j--) {
        if ((prav[i][j] - lev[i][j] + 1) > (rowNums[i][j]))
            reshstr4(i, j, &p, &q);
        else
            q = prav[i][j];
        if (er == false)
            return;
    }

}

void reshsto3(int ri, int rj, int (*p), int (*q)) {
    int s, a, b, min, max, j1, t;
    struct kluch *g;
    s = verh[ri][rj];
    if (niz[ri][rj - 1] < verh[ri][rj])
        (*p) = s - 1;
    while ((s < verh[ri][rj + 1]) && (s <= niz[ri][rj])) {
        label:
        if (jpn[s][ri] == no) {
            if ((*q) < (*p)) {
                a = rj;
                while ((niz[ri][a] >= s - 1) && (colNums[ri][a] > (s - (*p) - 1)))
                    a--;
                if ((niz[ri][a] < s - 1) && (colNums[ri][a + 1] > (s - (*p) - 1)))
                    for (b = (*p) + 1; b < s; b++) {
                        jpn[b][ri] = no;

                        j1 = 0;
                        while (b >= verh[ri][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[ri][j1] == false)) {
                            kluchverh[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }

                        j1 = colNumCount[ri] + 1;
                        while (b <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[ri] + 1) && (kluchniz[ri][j1] == false)) {
                            kluchniz[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        j1 = 0;
                        while (ri >= lev[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                            kluchlev[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[b] + 1;
                        while (ri <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                            kluchprav[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }


                        if (kluch22[ri] == false) {
                            kluch22[ri] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        if (kluch12[b] == false) {
                            kluch12[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        netu--;
                        if (perebor) {
                            kolic++;
                            m22[kolic] = ri;
                            m12[kolic] = b;
                        }

                    }
            }

            while ((jpn[s][ri] == no) && (s < verh[ri][rj + 1]) && (s <= niz[ri][rj]))
                s++;
            (*p) = s - 1;
        }
        if ((s < verh[ri][rj + 1]) && (s <= niz[ri][rj]) && (jpn[s][ri] == yes)) {
            if ((s < verh[ri][rj + 1]) && (s <= niz[ri][rj]) && (jpn[s][ri] == yes) &&
                ((*q) < (*p))) {
                (*q) = s;
                while (jpn[s][ri] == yes)
                    s++;
                max = colNums[ri][rj];
                a = rj - 1;
                while (niz[ri][a] >= s - 1) {
                    if (colNums[ri][a] > max)
                        max = colNums[ri][a];
                    a--;
                }
                if (s - max - 2 - (*p) >= 0) {
                    min = 0;
                    if (niz[ri][rj - 1] >= (*q) - 2)
                        min = colNums[ri][rj - 1];
                    a = rj - 2;
                    /* Original no condition that a >= 0 but it causes a crash otherwise */
                    while (a >= 0 && niz[ri][a] > (*p)) {
                        if ((colNums[ri][a] < min) && (colNums[ri][a + 1] >= s - (*q)))
                            min = colNums[ri][a];
                        a--;
                    }
                    if ((*q) - 2 - (*p) < min) {
                        for (b = (*p) + 1; b < s - max; b++) {
                            if (jpn[b][ri] == yes) {
                                er = false;
                                return;
                            }
                            jpn[b][ri] = no;
                            j1 = 0;
                            while (b >= verh[ri][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchverh[ri][j1] == false)) {
                                kluchverh[ri][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = ri;
                                (*g).j = j1;
                                (*g).prev = kluchverhfinish.prev;
                                kluchverhfinish.prev = g;
                            }

                            j1 = colNumCount[ri] + 1;
                            while (b <= niz[ri][j1])
                                j1--;
                            j1++;
                            if ((j1 < colNumCount[ri] + 1) && (kluchniz[ri][j1] == false)) {
                                kluchniz[ri][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = ri;
                                (*g).j = j1;
                                (*g).prev = kluchnizfinish.prev;
                                kluchnizfinish.prev = g;
                            }

                            j1 = 0;
                            while (ri >= lev[b][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                                kluchlev[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchlevfinish.prev;
                                kluchlevfinish.prev = g;
                            }
                            j1 = rowNumCount[b] + 1;
                            while (ri <= prav[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                                kluchprav[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchpravfinish.prev;
                                kluchpravfinish.prev = g;
                            }


                            if (kluch22[ri] == false) {
                                kluch22[ri] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = ri;

                                (*g).prev = kluchfinish4.prev;
                                kluchfinish4.prev = g;
                            }

                            if (kluch12[b] == false) {
                                kluch12[b] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;

                                (*g).prev = kluchfinish3.prev;
                                kluchfinish3.prev = g;
                            }


                            netu--;
                            if (perebor) {
                                kolic++;
                                m22[kolic] = ri;
                                m12[kolic] = b;
                            }

                        }
                        (*p) = s - max - 1;
                    }
                }
                min = colNums[ri][rj];
                a = rj - 1;
                while (niz[ri][a] >= s - 1) {
                    if ((colNums[ri][a] < min) && (colNums[ri][a] >= s - (*q)))
                        min = colNums[ri][a];
                    a--;
                }
                for (b = s; b < (*p) + min + 1; b++) {
                    if (jpn[b][ri] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[b][ri] == empty) {
                        if (jpn[b][ri] == no) {
                            er = false;
                            return;
                        }

                        jpn[b][ri] = yes;
                        j1 = 0;
                        while (b >= verh[ri][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[ri][j1] == false)) {
                            kluchverh[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }

                        j1 = colNumCount[ri] + 1;
                        while (b <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[ri] + 1) && (kluchniz[ri][j1] == false)) {
                            kluchniz[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        j1 = 0;
                        while (ri >= lev[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                            kluchlev[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[b] + 1;
                        while (ri <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                            kluchprav[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }


                        if (kluch22[ri] == false) {
                            kluch22[ri] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        if (kluch12[b] == false) {
                            kluch12[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }


                        netu--;
                        if (perebor) {
                            kolic++;
                            m22[kolic] = ri;
                            m12[kolic] = b;
                        }

                    }
                }
                while (jpn[s][ri] == yes)
                    s++;
            }
            if ((s < verh[ri][rj + 1]) && (s <= niz[ri][rj]) && (jpn[s][ri] == yes) &&
                ((*q) > (*p))) {
                int k = (*q);
                while (jpn[k][ri] == yes)
                    k++;
                if ((k == s - 1) && (jpn[k][ri] == empty)) {
                    k = s;
                    while (jpn[k][ri] == yes)
                        k++;
                    if (verh[ri][rj] <= (*q))
                        a = rj;
                    else
                        a = rj - 1;
                    while ((niz[ri][a] >= (*q)) && ((*q) + colNums[ri][a] < k - 1))
                        a--;
                    if (niz[ri][a] < (*q)) {
                        if (jpn[s - 1][ri] == yes) {
                            er = false;
                            return;
                        }

                        jpn[s - 1][ri] = no;
                        j1 = 0;
                        while (s - 1 >= verh[ri][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[ri][j1] == false)) {
                            kluchverh[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }

                        j1 = colNumCount[ri] + 1;
                        while (s - 1 <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[ri] + 1) && (kluchniz[ri][j1] == false)) {
                            kluchniz[ri][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        j1 = 0;
                        while (ri >= lev[s - 1][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[s - 1][j1] == false)) {
                            kluchlev[s - 1][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[s - 1] + 1;
                        while (ri <= prav[s - 1][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[s - 1] + 1) && (kluchprav[s - 1][j1] == false)) {
                            kluchprav[s - 1][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }


                        if (kluch22[ri] == false) {
                            kluch22[ri] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = ri;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        if (kluch12[s - 1] == false) {
                            kluch12[s - 1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = s - 1;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        netu--;

                        if (perebor) {
                            kolic++;
                            m22[kolic] = ri;
                            m12[kolic] = s - 1;
                        }
                        s = s - 1;
                        goto label;
                    }
                }
            }
            if ((s < verh[ri][rj + 1]) && (s <= niz[ri][rj]) && (jpn[s][ri] == yes)) {
                (*q) = s;
                while (jpn[s][ri] == yes)
                    s++;
            }
            t = s - (*q);
            if ((t == colNums[ri][rj]) && (jpn[s][ri] == empty)) {
                a = rj;
                while ((niz[ri][a] >= s - 1) &&
                       ((colNums[ri][a] <= colNums[ri][rj]) || (niz[ri][a] == s - 1)))
                    a--;
                if ((niz[ri][a] < s - 1) &&
                    ((colNums[ri][a + 1] <= colNums[ri][rj]) || (niz[ri][a + 1] == s - 1))) {
                    jpn[s][ri] = no;

                    j1 = 0;
                    while (s >= verh[ri][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[ri][j1] == false)) {
                        kluchverh[ri][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = ri;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }

                    j1 = colNumCount[ri] + 1;
                    while (s <= niz[ri][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[ri] + 1) && (kluchniz[ri][j1] == false)) {
                        kluchniz[ri][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = ri;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    j1 = 0;
                    while (ri >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (ri <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }


                    if (kluch22[ri] == false) {
                        kluch22[ri] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = ri;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m22[kolic] = ri;
                        m12[kolic] = s;
                    }
                }
            }

        }

        while ((jpn[s][ri] == empty) && (s < verh[ri][rj + 1]) && (s <= niz[ri][rj]))
            s++;

    }
}

void reshsto4(int i, int j, int (*p), int (*q)) {
    int s, a, b, min, max, j1, t;
    struct kluch *g;
    s = niz[i][j];
    if (verh[i][j + 1] > niz[i][j]) {
        (*p) = s + 1;
    }
    while ((s > niz[i][j - 1]) && (s >= verh[i][j])) {
        if (jpn[s][i] == no) {
            if ((*q) > (*p)) {
                a = j;
                while ((verh[i][a] <= s + 1) && (colNums[i][a] > ((*p) - s - 1)))
                    a++;
                if ((verh[i][a] > s + 1) && (colNums[i][a - 1] > ((*p) - s - 1)))
                    for (b = (*p) - 1; b > s; b--) {
                        jpn[b][i] = no;

                        j1 = 0;
                        while (b >= verh[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                            kluchverh[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }

                        j1 = colNumCount[i] + 1;
                        while (b <= niz[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                            kluchniz[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        j1 = 0;
                        while (i >= lev[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                            kluchlev[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[b] + 1;
                        while (i <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                            kluchprav[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }


                        if (kluch22[i] == false) {
                            kluch22[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        if (kluch12[b] == false) {
                            kluch12[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }

                        netu--;
                        if (perebor) {
                            kolic++;
                            m22[kolic] = i;
                            m12[kolic] = b;
                        }

                    }
            }

            while ((jpn[s][i] == no) && (s > niz[i][j - 1]) && (s >= verh[i][j]))
                s--;
            (*p) = s + 1;
        }
        if ((s > niz[i][j - 1]) && (s >= verh[i][j]) && (jpn[s][i] == yes)) {
            if ((s > niz[i][j - 1]) && (s >= verh[i][j]) && (jpn[s][i] == yes) && ((*q) > (*p))) {
                (*q) = s;
                while (jpn[s][i] == yes)
                    s--;
                max = colNums[i][j];
                a = j + 1;
                while (verh[i][a] <= s + 1) {
                    if (colNums[i][a] > max)
                        max = colNums[i][a];
                    a++;
                }
                if ((*p) - s - max - 2 >= 0) {
                    min = 0;
                    if (verh[i][j + 1] <= (*q) + 2)
                        min = colNums[i][j + 1];
                    a = j + 2;
                    //char asffd[100];
                    //sprintf(asffd, "%p %d", &(verh[i_0][a]), a );
                    //sprintf(asffd, "%d %d", a, colNumCount[i_0] + 2);
                    //__android_log_write(ANDROID_LOG_DEBUG,"qwWer", asffd);
                    //verh[i_0][colNumCount[i_0]+1] = 22;
                    /* Original no condition that a < colNumCount[i_0] but it causes a crash otherwise */
                    while (a < colNumCount[i] + 2 && verh[i][a] < (*p)) {
                        if ((colNums[i][a] < min) && (colNums[i][a - 1] >= (*q) - s))
                            min = colNums[i][a];
                        a++;
                    }
                    if ((*p) - (*q) - 2 < min)
                        for (b = s + max + 1; b < (*p); b++) {
                            if (jpn[b][i] == yes) {
                                er = false;
                                return;
                            }
                            jpn[b][i] = no;
                            j1 = 0;
                            while (b >= verh[i][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                                kluchverh[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchverhfinish.prev;
                                kluchverhfinish.prev = g;
                            }

                            j1 = colNumCount[i] + 1;
                            while (b <= niz[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                                kluchniz[i][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;
                                (*g).j = j1;
                                (*g).prev = kluchnizfinish.prev;
                                kluchnizfinish.prev = g;
                            }

                            j1 = 0;
                            while (i >= lev[b][j1])
                                j1++;
                            j1--;
                            if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                                kluchlev[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchlevfinish.prev;
                                kluchlevfinish.prev = g;
                            }
                            j1 = rowNumCount[b] + 1;
                            while (i <= prav[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                                kluchprav[b][j1] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;
                                (*g).j = j1;
                                (*g).prev = kluchpravfinish.prev;
                                kluchpravfinish.prev = g;
                            }


                            if (kluch22[i] == false) {
                                kluch22[i] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = i;

                                (*g).prev = kluchfinish4.prev;
                                kluchfinish4.prev = g;
                            }

                            if (kluch12[b] == false) {
                                kluch12[b] = true;
                                g = (struct kluch *) malloc(sizeof(struct kluch));
                                (*g).i = b;

                                (*g).prev = kluchfinish3.prev;
                                kluchfinish3.prev = g;
                            }


                            netu--;
                            if (perebor) {
                                kolic++;
                                m22[kolic] = i;
                                m12[kolic] = b;
                            }

                        }
                }
                min = colNums[i][j];
                a = j + 1;
                while (verh[i][a] <= s + 1) {
                    if ((colNums[i][a] < min) && (colNums[i][a] >= (*q) - s))
                        min = colNums[i][a];
                    a++;
                }
                for (b = s; b > (*p) - min - 1; b--) {
                    if (jpn[b][i] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[b][i] == empty) {
                        if (jpn[b][i] == no) {
                            er = false;
                            return;
                        }
                        jpn[b][i] = yes;
                        j1 = 0;
                        while (b >= verh[i][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                            kluchverh[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchverhfinish.prev;
                            kluchverhfinish.prev = g;
                        }

                        j1 = colNumCount[i] + 1;
                        while (b <= niz[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                            kluchniz[i][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;
                            (*g).j = j1;
                            (*g).prev = kluchnizfinish.prev;
                            kluchnizfinish.prev = g;
                        }

                        j1 = 0;
                        while (i >= lev[b][j1])
                            j1++;
                        j1--;
                        if ((j1 > 0) && (kluchlev[b][j1] == false)) {
                            kluchlev[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchlevfinish.prev;
                            kluchlevfinish.prev = g;
                        }
                        j1 = rowNumCount[b] + 1;
                        while (i <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < rowNumCount[b] + 1) && (kluchprav[b][j1] == false)) {
                            kluchprav[b][j1] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;
                            (*g).j = j1;
                            (*g).prev = kluchpravfinish.prev;
                            kluchpravfinish.prev = g;
                        }


                        if (kluch22[i] == false) {
                            kluch22[i] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = i;

                            (*g).prev = kluchfinish4.prev;
                            kluchfinish4.prev = g;
                        }

                        if (kluch12[b] == false) {
                            kluch12[b] = true;
                            g = (struct kluch *) malloc(sizeof(struct kluch));
                            (*g).i = b;

                            (*g).prev = kluchfinish3.prev;
                            kluchfinish3.prev = g;
                        }


                        netu--;
                        if (perebor) {
                            kolic++;
                            m22[kolic] = i;
                            m12[kolic] = b;
                        }

                    }
                }
                while (jpn[s][i] == yes)
                    s--;
            }
            if ((s > niz[i][j - 1]) && (s >= verh[i][j]) && (jpn[s][i] == yes)) {
                (*q) = s;
                while (jpn[s][i] == yes)
                    s--;
            }
            t = (*q) - s;
            if ((t == colNums[i][j]) && (jpn[s][i] == empty)) {
                a = j;
                while ((verh[i][a] <= s + 1) && ((colNums[i][a] <= colNums[i][j]) || (verh[i][a] == s + 1)))
                    a++;
                if ((verh[i][a] > s + 1) &&
                    ((colNums[i][a - 1] <= colNums[i][j]) || (verh[i][a - 1] == s + 1))) {
                    jpn[s][i] = no;

                    j1 = 0;
                    while (s >= verh[i][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchverh[i][j1] == false)) {
                        kluchverh[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchverhfinish.prev;
                        kluchverhfinish.prev = g;
                    }

                    j1 = colNumCount[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < colNumCount[i] + 1) && (kluchniz[i][j1] == false)) {
                        kluchniz[i][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;
                        (*g).j = j1;
                        (*g).prev = kluchnizfinish.prev;
                        kluchnizfinish.prev = g;
                    }

                    j1 = 0;
                    while (i >= lev[s][j1])
                        j1++;
                    j1--;
                    if ((j1 > 0) && (kluchlev[s][j1] == false)) {
                        kluchlev[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchlevfinish.prev;
                        kluchlevfinish.prev = g;
                    }
                    j1 = rowNumCount[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < rowNumCount[s] + 1) && (kluchprav[s][j1] == false)) {
                        kluchprav[s][j1] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;
                        (*g).j = j1;
                        (*g).prev = kluchpravfinish.prev;
                        kluchpravfinish.prev = g;
                    }


                    if (kluch22[i] == false) {
                        kluch22[i] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = i;

                        (*g).prev = kluchfinish4.prev;
                        kluchfinish4.prev = g;
                    }

                    if (kluch12[s] == false) {
                        kluch12[s] = true;
                        g = (struct kluch *) malloc(sizeof(struct kluch));
                        (*g).i = s;

                        (*g).prev = kluchfinish3.prev;
                        kluchfinish3.prev = g;
                    }


                    netu--;
                    if (perebor) {
                        kolic++;
                        m22[kolic] = i;
                        m12[kolic] = s;
                    }
                }
            }

        }
        while ((jpn[s][i] == empty) && (s > niz[i][j - 1]) && (s >= verh[i][j]))
            s--;
    }
}

void reshstodva(int i) {
    int j, p, q;
    q = 0;
    for (j = 1; j < colNumCount[i] + 1; j++) {
        if ((niz[i][j] - verh[i][j] + 1) > (colNums[i][j]))
            reshsto3(i, j, &p, &q);
        else
            q = verh[i][j];
        if (er == false)
            return;
    }
    q = height + 3;
    for (j = colNumCount[i]; j > 0; j--) {
        if ((niz[i][j] - verh[i][j] + 1) > (colNums[i][j]))
            reshsto4(i, j, &p, &q);
        else
            q = niz[i][j];
        if (er == false)
            return;
    }

}

void resheniee() {
    struct kluch *g, *h, *g1, *h1;
    int i, j;


    while (&kluchlevstart != kluchlevfinish.prev || &kluchpravstart != kluchpravfinish.prev ||
           &kluchverhstart != kluchverhfinish.prev || &kluchnizstart != kluchnizfinish.prev ||
           &kluchstart3 != kluchfinish3.prev || &kluchstart4 != kluchfinish4.prev) {
        if (&kluchlevstart != kluchlevfinish.prev || &kluchpravstart != kluchpravfinish.prev ||
            &kluchverhstart != kluchverhfinish.prev || &kluchnizstart != kluchnizfinish.prev) {

            g = kluchlevfinish.prev;
            kluchlevfinish.prev = &kluchlevstart;
            while (g != &kluchlevstart) {
                i = (*g).i;
                j = (*g).j;

                h = (*g).prev;
                free(g);
                g = h;
                kluchlev[i][j] = false;
                reshstr1(i, j);

                if (er == false) {
                    while (g != &kluchlevstart) {
                        i = (*g).i;
                        j = (*g).j;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluchlev[i][j] = false;
                    }
                    return;
                }
            }

            g = kluchpravfinish.prev;
            kluchpravfinish.prev = &kluchpravstart;
            while (g != &kluchpravstart) {
                i = (*g).i;
                j = (*g).j;
                h = (*g).prev;
                free(g);
                g = h;
                kluchprav[i][j] = false;
                reshstr2(i, j);

                if (er == false) {
                    while (g != &kluchpravstart) {
                        i = (*g).i;
                        j = (*g).j;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluchprav[i][j] = false;
                    }
                    return;
                }
            }

            g = kluchverhfinish.prev;
            kluchverhfinish.prev = &kluchverhstart;
            while (g != &kluchverhstart) {
                i = (*g).i;
                j = (*g).j;

                h = (*g).prev;
                free(g);
                g = h;
                kluchverh[i][j] = false;
                reshsto1(i, j);

                if (er == false) {
                    while (g != &kluchverhstart) {
                        i = (*g).i;
                        j = (*g).j;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluchverh[i][j] = false;
                    }
                    return;
                }
            }

            g = kluchnizfinish.prev;
            kluchnizfinish.prev = &kluchnizstart;
            while (g != &kluchnizstart) {
                i = (*g).i;
                j = (*g).j;

                h = (*g).prev;
                free(g);
                g = h;
                kluchniz[i][j] = false;
                reshsto2(i, j);

                if (er == false) {
                    while (g != &kluchnizstart) {
                        i = (*g).i;
                        j = (*g).j;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluchniz[i][j] = false;
                    }
                    return;
                }

            }


        } else {

            g = kluchfinish3.prev;
            kluchfinish3.prev = &kluchstart3;
            while (g != &kluchstart3) {
                i = (*g).i;

                h = (*g).prev;
                free(g);
                g = h;
                kluch12[i] = false;
                reshstrdva(i);

                if (er == false) {
                    while (g != &kluchstart3) {
                        i = (*g).i;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluch12[i] = false;
                    }
                    return;
                }
            }

            g = kluchfinish4.prev;
            kluchfinish4.prev = &kluchstart4;
            while (g != &kluchstart4) {
                i = (*g).i;

                h = (*g).prev;
                free(g);
                g = h;
                kluch22[i] = false;
                reshstodva(i);

                if (er == false) {
                    while (g != &kluchstart4) {
                        i = (*g).i;

                        h = (*g).prev;
                        free(g);
                        g = h;
                        kluch22[i] = false;
                    }
                    return;
                }

            }


        }


    }
}

void podgon(int cis, struct active *activenew, struct active *activedel, struct passive *passivenew,
            struct passive *passivedel, struct levpravverhniz *lev2new,
            struct levpravverhniz *lev2del) {
    int i0, j0, k, i, j, j1, m13, m23;
    unsigned char k0, m33;
    struct active *a, *b, *activenew2, *activedel2;
    struct passive *c, *d, *passivenew2, *passivedel2;
    struct levpravverhniz *e, *f, *lev2new2, *lev2del2;
    bool cond;

    struct kluch *g, *h, *g1;
#ifdef DEBUG
                                                                                                                            test2++;
	if (test2%1000==0)
		printf("%s%d%s%d%s%d%s","test2 = ",test2," cis = ",cis,", netu= ",netu,"\n");
#endif

    if ((clock() - time1) > CLOCKS_PER_SEC / 10) {
        //This is taking too long
        kol = 0;
        return;
    }

    a = start.next[1];
    while (a != &finish) {

        i0 = (*a).m1;
        j0 = (*a).m2;
        k0 = (*a).m3;
        jpn[i0][j0] = k0;
        kolic++;
        m12[kolic] = i0;
        m22[kolic] = j0;
        j1 = 0;
        while (j0 >= lev[i0][j1])
            j1++;
        j1--;
        if (kluchlev[i0][j1] == false) {
            kluchlev[i0][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i0;
            (*g).j = j1;

            (*g).prev = kluchlevfinish.prev;
            kluchlevfinish.prev = g;
        }
        j1 = rowNumCount[i0] + 1;
        while (j0 <= prav[i0][j1])
            j1--;
        j1++;
        if (kluchprav[i0][j1] == false) {
            kluchprav[i0][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i0;
            (*g).j = j1;

            (*g).prev = kluchpravfinish.prev;
            kluchpravfinish.prev = g;
        }
        j1 = 0;
        while (i0 >= verh[j0][j1])
            j1++;
        j1--;
        if (kluchverh[j0][j1] == false) {
            kluchverh[j0][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = j0;
            (*g).j = j1;

            (*g).prev = kluchverhfinish.prev;
            kluchverhfinish.prev = g;
        }
        j1 = colNumCount[j0] + 1;
        while (i0 <= niz[j0][j1])
            j1--;
        j1++;
        if (kluchniz[j0][j1] == false) {
            kluchniz[j0][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = j0;
            (*g).j = j1;

            (*g).prev = kluchnizfinish.prev;
            kluchnizfinish.prev = g;
        }

        if (kluch12[i0] == false) {
            kluch12[i0] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = i0;

            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }
        if (kluch22[j0] == false) {
            kluch22[j0] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = j0;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        netu--;
        c = jpn4[i0][j0].start[k0].next[1];
        while (c != (&jpn4[i0][j0].finish[k0])) {
            jpn[(*c).m1][(*c).m2] = (*c).m3;
            kolic++;
            m12[kolic] = (*c).m1;
            m22[kolic] = (*c).m2;
            j1 = 0;
            while ((*c).m2 >= lev[(*c).m1][j1])
                j1++;
            j1--;
            if (kluchlev[(*c).m1][j1] == false) {
                kluchlev[(*c).m1][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m1;
                (*g).j = j1;

                (*g).prev = kluchlevfinish.prev;
                kluchlevfinish.prev = g;
            }
            j1 = rowNumCount[(*c).m1] + 1;
            while ((*c).m2 <= prav[(*c).m1][j1])
                j1--;
            j1++;
            if (kluchprav[(*c).m1][j1] == false) {
                kluchprav[(*c).m1][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m1;
                (*g).j = j1;

                (*g).prev = kluchpravfinish.prev;
                kluchpravfinish.prev = g;
            }
            j1 = 0;
            while ((*c).m1 >= verh[(*c).m2][j1])
                j1++;
            j1--;
            if (kluchverh[(*c).m2][j1] == false) {
                kluchverh[(*c).m2][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m2;
                (*g).j = j1;

                (*g).prev = kluchverhfinish.prev;
                kluchverhfinish.prev = g;
            }
            j1 = colNumCount[(*c).m2] + 1;
            while ((*c).m1 <= niz[(*c).m2][j1])
                j1--;
            j1++;
            if (kluchniz[(*c).m2][j1] == false) {
                kluchniz[(*c).m2][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m2;
                (*g).j = j1;

                (*g).prev = kluchnizfinish.prev;
                kluchnizfinish.prev = g;
            }

            if (kluch12[(*c).m1] == false) {
                kluch12[(*c).m1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m1;

                (*g).prev = kluchfinish3.prev;
                kluchfinish3.prev = g;
            }
            if (kluch22[(*c).m2] == false) {
                kluch22[(*c).m2] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = (*c).m2;

                (*g).prev = kluchfinish4.prev;
                kluchfinish4.prev = g;
            }

            netu--;
            c = (*c).next[1];

        }
        k = kolic + 1;
        er = true;

        resheniee();
        if ((er) && (netu == 0)) {
            return;
        }
        if (er == false) {

            for (k = kolic1[cis - 1] + 1; k < kolic + 1; k++) {
                jpn[m12[k]][m22[k]] = empty;
                netu++;
            }
            e = lev2delfinish.prev[1];
            while (e != lev2del) {
                f = (*e).prev[1];
                if ((*e).vid == 1) {
                    lev[(*e).i][(*e).j] = (*e).znach;
                    lev2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 2) {
                    prav[(*e).i][(*e).j] = (*e).znach;
                    prav2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 3) {
                    verh[(*e).i][(*e).j] = (*e).znach;
                    verh2[(*e).i][(*e).j] = e;
                } else {
                    niz[(*e).i][(*e).j] = (*e).znach;
                    niz2[(*e).i][(*e).j] = e;
                }

                (*e).prev[1] = NULL;

                e = f;
            }
            lev2delfinish.prev[1] = lev2del;

            g = kluchlevfinish.prev;
            while (g != &kluchlevstart) {
                kluchlev[(*g).i][(*g).j] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }
            g = kluchpravfinish.prev;
            while (g != &kluchpravstart) {
                kluchprav[(*g).i][(*g).j] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }
            g = kluchverhfinish.prev;
            while (g != &kluchverhstart) {
                kluchverh[(*g).i][(*g).j] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }
            g = kluchnizfinish.prev;
            while (g != &kluchnizstart) {
                kluchniz[(*g).i][(*g).j] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }
            g = kluchfinish3.prev;
            while (g != &kluchstart3) {
                kluch12[(*g).i] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }
            g = kluchfinish4.prev;
            while (g != &kluchstart4) {
                kluch22[(*g).i] = false;
                h = (*g).prev;
                free(g);
                g = h;
            }

            kluchlevfinish.prev = &kluchlevstart;
            kluchpravfinish.prev = &kluchpravstart;
            kluchverhfinish.prev = &kluchverhstart;
            kluchnizfinish.prev = &kluchnizstart;
            kluchfinish3.prev = &kluchstart3;
            kluchfinish4.prev = &kluchstart4;

            kolic = kolic1[cis - 1];
            if ((*a).m3 == yes)
                jpn[i0][j0] = no;
            else
                jpn[i0][j0] = yes;
            kolic++;
            m12[kolic] = i0;
            m22[kolic] = j0;
            j1 = 0;
            while (j0 >= lev[i0][j1])
                j1++;
            j1--;
            if (kluchlev[i0][j1] == false) {
                kluchlev[i0][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = i0;
                (*g).j = j1;

                (*g).prev = kluchlevfinish.prev;
                kluchlevfinish.prev = g;
            }
            j1 = rowNumCount[i0] + 1;
            while (j0 <= prav[i0][j1])
                j1--;
            j1++;
            if (kluchprav[i0][j1] == false) {
                kluchprav[i0][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = i0;
                (*g).j = j1;

                (*g).prev = kluchpravfinish.prev;
                kluchpravfinish.prev = g;
            }
            j1 = 0;
            while (i0 >= verh[j0][j1])
                j1++;
            j1--;
            if (kluchverh[j0][j1] == false) {
                kluchverh[j0][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = j0;
                (*g).j = j1;

                (*g).prev = kluchverhfinish.prev;
                kluchverhfinish.prev = g;
            }
            j1 = colNumCount[j0] + 1;
            while (i0 <= niz[j0][j1])
                j1--;
            j1++;
            if (kluchniz[j0][j1] == false) {
                kluchniz[j0][j1] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = j0;
                (*g).j = j1;

                (*g).prev = kluchnizfinish.prev;
                kluchnizfinish.prev = g;
            }

            if (kluch12[i0] == false) {
                kluch12[i0] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = i0;

                (*g).prev = kluchfinish3.prev;
                kluchfinish3.prev = g;
            }
            if (kluch22[j0] == false) {
                kluch22[j0] = true;
                g = (struct kluch *) malloc(sizeof(struct kluch));
                (*g).i = j0;

                (*g).prev = kluchfinish4.prev;
                kluchfinish4.prev = g;
            }

            netu--;
            er = true;
            resheniee();

            if ((er == false) || (netu == 0)) {
                e = lev2delfinish.prev[1];
                while (e != lev2del) {
                    f = (*e).prev[1];
                    if ((*e).vid == 1) {
                        lev[(*e).i][(*e).j] = (*e).znach;
                        lev2[(*e).i][(*e).j] = e;
                    } else if ((*e).vid == 2) {
                        prav[(*e).i][(*e).j] = (*e).znach;
                        prav2[(*e).i][(*e).j] = e;
                    } else if ((*e).vid == 3) {
                        verh[(*e).i][(*e).j] = (*e).znach;
                        verh2[(*e).i][(*e).j] = e;
                    } else {
                        niz[(*e).i][(*e).j] = (*e).znach;
                        niz2[(*e).i][(*e).j] = e;
                    }

                    (*e).prev[1] = NULL;

                    e = f;
                }
                lev2delfinish.prev[1] = lev2del;

                return;
            }


            e = lev2delfinish.prev[1];
            (*e).next[1] = &lev2delfinish;
            while (e != lev2del) {
                f = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*f).cis = cis;
                (*f).vid = (*e).vid;
                (*f).i = (*e).i;
                (*f).j = (*e).j;
                if ((*e).vid == 1) {
                    (*f).znach = lev[(*e).i][(*e).j];
                    lev2[(*e).i][(*e).j] = f;

                } else if ((*e).vid == 2) {
                    (*f).znach = prav[(*e).i][(*e).j];
                    prav2[(*e).i][(*e).j] = f;
                } else if ((*e).vid == 3) {
                    (*f).znach = verh[(*e).i][(*e).j];
                    verh2[(*e).i][(*e).j] = f;
                } else {
                    (*f).znach = niz[(*e).i][(*e).j];
                    niz2[(*e).i][(*e).j] = f;
                }
                (*f).prev[0] = (*lev2new).prev[0];
                (*f).next[0] = lev2new;
                (*(*lev2new).prev[0]).next[0] = f;
                (*lev2new).prev[0] = f;
                (*f).prev[1] = NULL;
                (*f).next[1] = NULL;
                f = (*e).prev[1];
                (*f).next[1] = e;
                if ((*e).cis == cis) {
                    (*(*e).prev[0]).next[0] = (*e).next[0];
                    (*(*e).next[0]).prev[0] = (*e).prev[0];
                    (*(*e).prev[1]).next[1] = (*e).next[1];
                    (*(*e).next[1]).prev[1] = (*e).prev[1];
                    free(e);


                }


                e = f;
            }
            (*(*lev2del).prev[1]).next[1] = (*lev2del).next[1];
            (*(*lev2del).next[1]).prev[1] = (*lev2del).prev[1];
            (*lev2del).prev[1] = lev2delfinish.prev[1];
            (*lev2del).next[1] = &lev2delfinish;
            (*lev2delfinish.prev[1]).next[1] = lev2del;
            lev2delfinish.prev[1] = lev2del;

            k = kolic;
            for (i = kolic1[cis - 1] + 1; i <= kolic; i++) {
                i0 = m12[i];
                j0 = m22[i];
                for (j = 0; j <= 1; j++) {
                    cond = (jpn4[i0][j0].element[j] != NULL);
                    if (cond) {

                        b = jpn4[i0][j0].element[j];

                        (*(*b).prev[1]).next[1] = (*b).next[1];
                        (*(*b).next[1]).prev[1] = (*b).prev[1];
                        if ((*b).cis < cis) {
                            (*b).prev[1] = (*activedel).prev[1];
                            (*b).next[1] = activedel;
                            (*(*activedel).prev[1]).next[1] = b;
                            (*activedel).prev[1] = b;
                        } else {
                            (*(*b).prev[0]).next[0] = (*b).next[0];
                            (*(*b).next[0]).prev[0] = (*b).prev[0];
                            free(b);
                            b = NULL;
                        }
                        jpn4[i0][j0].element[j] = NULL;

                        d = jpn4[i0][j0].start[j].next[1];
                        while (d != &(jpn4[i0][j0].finish[j])) {
                            c = (*d).next[1];

                            (*(*d).prev[2]).next[2] = (*d).next[2];
                            (*(*d).next[2]).prev[2] = (*d).prev[2];

                            if ((*d).cis < cis) {
                                (*d).prev[1] = (*passivedel).prev[1];
                                (*d).next[1] = passivedel;
                                (*(*passivedel).prev[1]).next[1] = d;
                                (*passivedel).prev[1] = d;
                                (*d).prev[2] = (*passivedel).prev[2];
                                (*d).next[2] = passivedel;
                                (*(*passivedel).prev[2]).next[2] = d;
                                (*passivedel).prev[2] = d;

                            } else {
                                (*(*d).prev[0]).next[0] = (*d).next[0];
                                (*(*d).next[0]).prev[0] = (*d).prev[0];
                                free(d);
                            }

                            d = c;

                        }
                        jpn4[i0][j0].start[j].next[1] = &(jpn4[i0][j0].finish[j]);
                        jpn4[i0][j0].finish[j].prev[1] = &(jpn4[i0][j0].start[j]);
                    }
                    d = jpn4[i0][j0].start[j].next[2];
                    while (d != &(jpn4[i0][j0].finish[j])) {

                        c = (*d).next[2];
                        if (jpn[(*d).m12][(*d).m22] == empty) {
                            b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                            if ((*b).notchoise) {
                                (*b).notchoise = false;
                                (*(*b).prev[1]).next[1] = (*b).next[1];
                                (*(*b).next[1]).prev[1] = (*b).prev[1];
                                (*b).next[1] = &finish;
                                (*b).prev[1] = finish.prev[1];
                                (*finish.prev[1]).next[1] = b;
                                finish.prev[1] = b;
                            }
                        }
                        (*(*d).prev[1]).next[1] = (*d).next[1];
                        (*(*d).next[1]).prev[1] = (*d).prev[1];

                        if ((*d).cis < cis) {
                            (*d).prev[1] = (*passivedel).prev[1];
                            (*d).next[1] = passivedel;
                            (*(*passivedel).prev[1]).next[1] = d;
                            (*passivedel).prev[1] = d;
                            (*d).prev[2] = (*passivedel).prev[2];
                            (*d).next[2] = passivedel;
                            (*(*passivedel).prev[2]).next[2] = d;
                            (*passivedel).prev[2] = d;

                        } else {
                            (*(*d).prev[0]).next[0] = (*d).next[0];
                            (*(*d).next[0]).prev[0] = (*d).prev[0];
                            free(d);
                        }

                        d = c;

                    }
                    jpn4[i0][j0].start[j].next[2] = &(jpn4[i0][j0].finish[j]);
                    jpn4[i0][j0].finish[j].prev[2] = &(jpn4[i0][j0].start[j]);

                }
                if (jpn[i0 + 1][j0] == empty && jpn4[i0 + 1][j0].notchoise) {
                    jpn4[i0 + 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 + 1;
                    m22[k] = j0;
                }
                if (jpn[i0 - 1][j0] == empty && jpn4[i0 - 1][j0].notchoise) {
                    jpn4[i0 - 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 - 1;
                    m22[k] = j0;
                }
                if (jpn[i0][j0 + 1] == empty && jpn4[i0][j0 + 1].notchoise) {
                    jpn4[i0][j0 + 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 + 1;
                }
                if (jpn[i0][j0 - 1] == empty && jpn4[i0][j0 - 1].notchoise) {
                    jpn4[i0][j0 - 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 - 1;
                }

            }

            for (i = kolic + 1; i <= k; i++) {
                i0 = m12[i];
                j0 = m22[i];
                jpn4[i0][j0].notchoise = true;
                for (j = 0; j <= 1; j++) {
                    cond = (jpn4[i0][j0].element[j] != NULL);
                    if (cond) {

                        b = jpn4[i0][j0].element[j];
                        if ((*b).notchoise) {
                            (*b).notchoise = false;
                            (*(*b).prev[1]).next[1] = (*b).next[1];
                            (*(*b).next[1]).prev[1] = (*b).prev[1];
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                        }
                    }
                    d = jpn4[i0][j0].start[j].next[2];
                    while (d != &(jpn4[i0][j0].finish[j])) {


                        b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                        if ((*b).notchoise) {
                            (*b).notchoise = false;
                            (*(*b).prev[1]).next[1] = (*b).next[1];
                            (*(*b).next[1]).prev[1] = (*b).prev[1];
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                        }

                        d = (*d).next[2];
                    }

                }
            }

            kolic1[cis - 1] = kolic;

        } else {
            for (i = k; i <= kolic; i++) {
                c = (struct passive *) malloc(sizeof(struct passive));
                (*c).cis = cis;
                (*c).m1 = m12[i];
                (*c).m2 = m22[i];
                (*c).m3 = jpn[m12[i]][m22[i]];
                (*c).m12 = i0;
                (*c).m22 = j0;
                (*c).m32 = k0;

                (*c).next[0] = passivenew;
                (*c).prev[0] = (*passivenew).prev[0];
                (*(*passivenew).prev[0]).next[0] = c;
                (*passivenew).prev[0] = c;

                (*c).next[1] = &(jpn4[i0][j0].finish[k0]);
                (*c).prev[1] = jpn4[i0][j0].finish[k0].prev[1];
                (*jpn4[i0][j0].finish[k0].prev[1]).next[1] = c;
                jpn4[i0][j0].finish[k0].prev[1] = c;


                (*c).next[2] = &(jpn4[(*c).m1][(*c).m2].finish[(*c).m3]);
                (*c).prev[2] = jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2];
                (*jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2]).next[2] = c;
                jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2] = c;


            }
            (*(*a).prev[1]).next[1] = (*a).next[1];
            (*(*a).next[1]).prev[1] = (*a).prev[1];
            if ((*a).cis < cis) {
                (*a).prev[1] = (*activedel).prev[1];
                (*a).next[1] = activedel;
                (*(*activedel).prev[1]).next[1] = a;
                (*activedel).prev[1] = a;
            } else {
                (*(*a).prev[0]).next[0] = (*a).next[0];
                (*(*a).next[0]).prev[0] = (*a).prev[0];
                free(a);
            }

            a = (struct active *) malloc(sizeof(struct active));
            (*a).cis = cis;
            (*a).m1 = i0;
            (*a).m2 = j0;
            (*a).m3 = k0;
            (*a).netu = kolic - kolic1[cis - 1];
            (*a).notchoise = true;
            (*a).next[0] = activenew;
            (*a).prev[0] = (*activenew).prev[0];
            (*(*activenew).prev[0]).next[0] = a;
            (*activenew).prev[0] = a;
            (*a).next[1] = &finish1;
            (*a).prev[1] = finish1.prev[1];
            (*finish1.prev[1]).next[1] = a;
            finish1.prev[1] = a;
            jpn4[i0][j0].element[k0] = a;


            for (k = kolic1[cis - 1] + 1; k < kolic + 1; k++) {
                jpn[m12[k]][m22[k]] = empty;
                netu++;
            }
            e = lev2delfinish.prev[1];
            while (e != lev2del) {
                f = (*e).prev[1];
                if ((*e).vid == 1) {
                    lev[(*e).i][(*e).j] = (*e).znach;
                    lev2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 2) {
                    prav[(*e).i][(*e).j] = (*e).znach;
                    prav2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 3) {
                    verh[(*e).i][(*e).j] = (*e).znach;
                    verh2[(*e).i][(*e).j] = e;
                } else {
                    niz[(*e).i][(*e).j] = (*e).znach;
                    niz2[(*e).i][(*e).j] = e;
                }

                (*e).prev[1] = NULL;

                e = f;
            }
            lev2delfinish.prev[1] = lev2del;

            kolic = kolic1[cis - 1];

        }
        a = start.next[1];
    }
    netu4 = 0;
    netu5 = 0;
    m13 = 0;
    m23 = 0;
    m33 = 0;
    a = start1.next[1];
    while (a != &finish1) {
        if ((*a).m3 == 0) {
            i0 = (*a).m1;
            j0 = (*a).m2;
            netu6 = (*a).netu;
            netu7 = (*jpn4[i0][j0].element[1]).netu;
            if ((netu6 > netu4 && netu7 > netu4) ||
                (netu6 >= netu4 && netu7 >= netu4 && netu6 + netu7 > netu4 + netu5)) {
                m13 = i0;
                m23 = j0;
                if (netu6 > netu7) {
                    m33 = no;
                    netu4 = netu7;
                    netu5 = netu6;
                } else {
                    m33 = yes;
                    netu4 = netu6;
                    netu5 = netu7;

                }
            }
        }

        a = (*a).next[1];
    }

    activenew2 = (struct active *) malloc(sizeof(struct active));
    (*activenew2).cis = 0;
    activedel2 = (struct active *) malloc(sizeof(struct active));
    (*activedel2).cis = 0;

    for (i = 0; i <= 1; i++) {
        (*activenew2).prev[i] = activenew;
        (*activenew2).next[i] = &activenewfinish;
        (*activenew).next[i] = activenew2;
        activenewfinish.prev[i] = activenew2;

        (*activedel2).prev[i] = activedel;
        (*activedel2).next[i] = &activedelfinish;
        (*activedel).next[i] = activedel2;
        activedelfinish.prev[i] = activedel2;

    }

    passivenew2 = (struct passive *) malloc(sizeof(struct passive));
    (*passivenew2).cis = 0;
    passivedel2 = (struct passive *) malloc(sizeof(struct passive));
    (*passivedel2).cis = 0;

    for (i = 0; i <= 2; i++) {
        (*passivenew2).prev[i] = passivenew;
        (*passivenew2).next[i] = &passivenewfinish;
        (*passivenew).next[i] = passivenew2;
        passivenewfinish.prev[i] = passivenew2;

        (*passivedel2).prev[i] = passivedel;
        (*passivedel2).next[i] = &passivedelfinish;
        (*passivedel).next[i] = passivedel2;
        passivedelfinish.prev[i] = passivedel2;

    }
    lev2new2 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
    (*lev2new2).cis = 0;
    lev2del2 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
    (*lev2del2).cis = 0;

    for (i = 0; i <= 1; i++) {
        (*lev2new2).prev[i] = lev2new;
        (*lev2new2).next[i] = &lev2newfinish;
        (*lev2new).next[i] = lev2new2;
        lev2newfinish.prev[i] = lev2new2;

        (*lev2del2).prev[i] = lev2del;
        (*lev2del2).next[i] = &lev2delfinish;
        (*lev2del).next[i] = lev2del2;
        lev2delfinish.prev[i] = lev2del2;

    }

    kolic++;
    m12[kolic] = m13;
    m22[kolic] = m23;
    jpn[m13][m23] = m33;
    netu--;
    j1 = 0;
    while (m23 >= lev[m13][j1])
        j1++;
    j1--;
    if (kluchlev[m13][j1] == false) {
        kluchlev[m13][j1] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m13;
        (*g).j = j1;

        (*g).prev = kluchlevfinish.prev;
        kluchlevfinish.prev = g;
    }
    j1 = rowNumCount[m13] + 1;
    while (m23 <= prav[m13][j1])
        j1--;
    j1++;
    if (kluchprav[m13][j1] == false) {
        kluchprav[m13][j1] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m13;
        (*g).j = j1;

        (*g).prev = kluchpravfinish.prev;
        kluchpravfinish.prev = g;
    }
    j1 = 0;
    while (m13 >= verh[m23][j1])
        j1++;
    j1--;
    if (kluchverh[m23][j1] == false) {
        kluchverh[m23][j1] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m23;
        (*g).j = j1;

        (*g).prev = kluchverhfinish.prev;
        kluchverhfinish.prev = g;
    }
    j1 = colNumCount[m23] + 1;
    while (m13 <= niz[m23][j1])
        j1--;
    j1++;
    if (kluchniz[m23][j1] == false) {
        kluchniz[m23][j1] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m23;
        (*g).j = j1;

        (*g).prev = kluchnizfinish.prev;
        kluchnizfinish.prev = g;
    }

    if (kluch12[m13] == false) {
        kluch12[m13] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m13;

        (*g).prev = kluchfinish3.prev;
        kluchfinish3.prev = g;
    }
    if (kluch22[m23] == false) {
        kluch22[m23] = true;
        g = (struct kluch *) malloc(sizeof(struct kluch));
        (*g).i = m23;

        (*g).prev = kluchfinish4.prev;
        kluchfinish4.prev = g;
    }

    resheniee();

    if (er)
        if (netu == 0) {
            return;
        }
    if (er == false) {
        e = lev2delfinish.prev[1];
        while (e != lev2del2) {
            f = (*e).prev[1];
            if ((*e).vid == 1) {
                lev[(*e).i][(*e).j] = (*e).znach;
                lev2[(*e).i][(*e).j] = e;
            } else if ((*e).vid == 2) {
                prav[(*e).i][(*e).j] = (*e).znach;
                prav2[(*e).i][(*e).j] = e;
            } else if ((*e).vid == 3) {
                verh[(*e).i][(*e).j] = (*e).znach;
                verh2[(*e).i][(*e).j] = e;
            } else {
                niz[(*e).i][(*e).j] = (*e).znach;
                niz2[(*e).i][(*e).j] = e;
            }

            (*e).prev[1] = NULL;

            e = f;
        }
        lev2delfinish.prev[1] = lev2del2;

    }

    if (er) {
        k = kolic;
        for (i = kolic1[cis - 1] + 1; i <= kolic; i++) {
            i0 = m12[i];
            j0 = m22[i];
            for (j = 0; j <= 1; j++) {
                cond = (jpn4[i0][j0].element[j] != NULL);
                if (cond) {

                    b = jpn4[i0][j0].element[j];
                    (*(*b).prev[1]).next[1] = (*b).next[1];
                    (*(*b).next[1]).prev[1] = (*b).prev[1];
                    if ((*b).cis < cis + 1) {
                        (*b).prev[1] = (*activedel2).prev[1];
                        (*b).next[1] = activedel2;
                        (*(*activedel2).prev[1]).next[1] = b;
                        (*activedel2).prev[1] = b;
                    } else {
                        (*(*b).prev[0]).next[0] = (*b).next[0];
                        (*(*b).next[0]).prev[0] = (*b).prev[0];
                        free(b);
                        b = NULL;
                    }
                    jpn4[i0][j0].element[j] = NULL;

                    d = jpn4[i0][j0].start[j].next[1];
                    while (d != &(jpn4[i0][j0].finish[j])) {
                        c = (*d).next[1];
                        (*(*d).prev[2]).next[2] = (*d).next[2];
                        (*(*d).next[2]).prev[2] = (*d).prev[2];

                        if ((*d).cis < cis + 1) {
                            (*d).prev[1] = (*passivedel2).prev[1];
                            (*d).next[1] = passivedel2;
                            (*(*passivedel2).prev[1]).next[1] = d;
                            (*passivedel2).prev[1] = d;
                            (*d).prev[2] = (*passivedel2).prev[2];
                            (*d).next[2] = passivedel2;
                            (*(*passivedel2).prev[2]).next[2] = d;
                            (*passivedel2).prev[2] = d;

                        } else {
                            (*(*d).prev[0]).next[0] = (*d).next[0];
                            (*(*d).next[0]).prev[0] = (*d).prev[0];
                            free(d);
                        }

                        d = c;

                    }
                    jpn4[i0][j0].start[j].next[1] = &(jpn4[i0][j0].finish[j]);
                    jpn4[i0][j0].finish[j].prev[1] = &(jpn4[i0][j0].start[j]);
                }
                d = jpn4[i0][j0].start[j].next[2];
                while (d != &(jpn4[i0][j0].finish[j])) {
                    c = (*d).next[2];
                    if (jpn[(*d).m12][(*d).m22] == empty) {
                        b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                        if ((*b).notchoise) {
                            (*b).notchoise = false;
                            (*(*b).prev[1]).next[1] = (*b).next[1];
                            (*(*b).next[1]).prev[1] = (*b).prev[1];
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                        }
                    }
                    (*(*d).prev[1]).next[1] = (*d).next[1];
                    (*(*d).next[1]).prev[1] = (*d).prev[1];

                    if ((*d).cis < cis + 1) {
                        (*d).prev[1] = (*passivedel2).prev[1];
                        (*d).next[1] = passivedel2;
                        (*(*passivedel2).prev[1]).next[1] = d;
                        (*passivedel2).prev[1] = d;
                        (*d).prev[2] = (*passivedel2).prev[2];
                        (*d).next[2] = passivedel2;
                        (*(*passivedel2).prev[2]).next[2] = d;
                        (*passivedel2).prev[2] = d;

                    } else {
                        (*(*d).prev[0]).next[0] = (*d).next[0];
                        (*(*d).next[0]).prev[0] = (*d).prev[0];
                        free(d);
                    }

                    d = c;

                }
                jpn4[i0][j0].start[j].next[2] = &(jpn4[i0][j0].finish[j]);
                jpn4[i0][j0].finish[j].prev[2] = &(jpn4[i0][j0].start[j]);

            }
            if (jpn[i0 + 1][j0] == empty && jpn4[i0 + 1][j0].notchoise) {
                jpn4[i0 + 1][j0].notchoise = false;
                k++;
                m12[k] = i0 + 1;
                m22[k] = j0;
            }
            if (jpn[i0 - 1][j0] == empty && jpn4[i0 - 1][j0].notchoise) {
                jpn4[i0 - 1][j0].notchoise = false;
                k++;
                m12[k] = i0 - 1;
                m22[k] = j0;
            }
            if (jpn[i0][j0 + 1] == empty && jpn4[i0][j0 + 1].notchoise) {
                jpn4[i0][j0 + 1].notchoise = false;
                k++;
                m12[k] = i0;
                m22[k] = j0 + 1;
            }
            if (jpn[i0][j0 - 1] == empty && jpn4[i0][j0 - 1].notchoise) {
                jpn4[i0][j0 - 1].notchoise = false;
                k++;
                m12[k] = i0;
                m22[k] = j0 - 1;
            }


        }
        for (i = kolic + 1; i <= k; i++) {
            i0 = m12[i];
            j0 = m22[i];
            jpn4[i0][j0].notchoise = true;
            for (j = 0; j <= 1; j++) {
                cond = (jpn4[i0][j0].element[j] != NULL);
                if (cond) {

                    b = jpn4[i0][j0].element[j];
                    if ((*b).notchoise) {
                        (*b).notchoise = false;
                        (*(*b).prev[1]).next[1] = (*b).next[1];
                        (*(*b).next[1]).prev[1] = (*b).prev[1];
                        (*b).next[1] = &finish;
                        (*b).prev[1] = finish.prev[1];
                        (*finish.prev[1]).next[1] = b;
                        finish.prev[1] = b;
                    }
                }
                d = jpn4[i0][j0].start[j].next[2];
                while (d != &(jpn4[i0][j0].finish[j])) {


                    b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                    if ((*b).notchoise) {
                        (*b).notchoise = false;
                        (*(*b).prev[1]).next[1] = (*b).next[1];
                        (*(*b).next[1]).prev[1] = (*b).prev[1];
                        (*b).next[1] = &finish;
                        (*b).prev[1] = finish.prev[1];
                        (*finish.prev[1]).next[1] = b;
                        finish.prev[1] = b;
                    }

                    d = (*d).next[2];
                }

            }
        }


        kolic1[cis] = kolic;

        e = lev2delfinish.prev[1];
        (*e).next[1] = &lev2delfinish;
        while (e != lev2del2) {
            f = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
            (*f).cis = cis + 1;
            (*f).vid = (*e).vid;
            (*f).i = (*e).i;
            (*f).j = (*e).j;
            if ((*e).vid == 1) {
                (*f).znach = lev[(*e).i][(*e).j];
                lev2[(*e).i][(*e).j] = f;

            } else if ((*e).vid == 2) {
                (*f).znach = prav[(*e).i][(*e).j];
                prav2[(*e).i][(*e).j] = f;
            } else if ((*e).vid == 3) {
                (*f).znach = verh[(*e).i][(*e).j];
                verh2[(*e).i][(*e).j] = f;
            } else {
                (*f).znach = niz[(*e).i][(*e).j];
                niz2[(*e).i][(*e).j] = f;
            }
            (*f).prev[0] = (*lev2new2).prev[0];
            (*f).next[0] = lev2new2;
            (*(*lev2new2).prev[0]).next[0] = f;
            (*lev2new2).prev[0] = f;
//				(*f).prev[1] = (*lev2new0).prev[1];
//				(*f).next[1]=lev2new0;
//				(*(*lev2new0).prev[1]).next[1]=f;
//				(*lev2new0).prev[1]=f;
            (*f).prev[1] = NULL;
            (*f).next[1] = NULL;
            f = (*e).prev[1];
            (*f).next[1] = e;
            if ((*e).cis == cis + 1) {
                (*(*e).prev[0]).next[0] = (*e).next[0];
                (*(*e).next[0]).prev[0] = (*e).prev[0];
                (*(*e).prev[1]).next[1] = (*e).next[1];
                (*(*e).next[1]).prev[1] = (*e).prev[1];
                free(e);
            }

            e = f;
        }
        (*(*lev2del2).prev[1]).next[1] = (*lev2del2).next[1];
        (*(*lev2del2).next[1]).prev[1] = (*lev2del2).prev[1];
        (*lev2del2).prev[1] = lev2delfinish.prev[1];
        (*lev2del2).next[1] = &lev2delfinish;
        (*lev2delfinish.prev[1]).next[1] = lev2del2;
        lev2delfinish.prev[1] = lev2del2;

        podgon(cis + 1, activenew2, activedel2, passivenew2, passivedel2, lev2new2, lev2del2);
    }

    if (er == false) {

        a = (*activenew).next[0];
        while (a != &activenewfinish) {
            b = (*a).next[0];
            (*(*a).prev[1]).next[1] = (*a).next[1];
            (*(*a).next[1]).prev[1] = (*a).prev[1];
            if ((*a).cis != 0)
                jpn4[(*a).m1][(*a).m2].element[(*a).m3] = NULL;
            free(a);
            a = b;

        }
        (*activenew).next[0] = &activenewfinish;
        activenewfinish.prev[0] = activenew;

        c = (*passivenew).next[0];
        while (c != &passivenewfinish) {
            d = (*c).next[0];
            (*(*c).prev[2]).next[2] = (*c).next[2];
            (*(*c).next[2]).prev[2] = (*c).prev[2];
            (*(*c).prev[1]).next[1] = (*c).next[1];
            (*(*c).next[1]).prev[1] = (*c).prev[1];
            (*(*c).prev[0]).next[0] = (*c).next[0];
            (*(*c).next[0]).prev[0] = (*c).prev[0];
            free(c);
            c = d;

        }
        (*passivenew).next[0] = &passivenewfinish;
        passivenewfinish.prev[0] = passivenew;

        e = (*lev2new).next[0];
        while (e != &lev2newfinish) {
            f = (*e).next[0];
            if ((*e).prev[1] != NULL)
                (*(*e).prev[1]).next[1] = (*e).next[1];
            if ((*e).next[1] != NULL)
                (*(*e).next[1]).prev[1] = (*e).prev[1];
            free(e);
            e = f;
        }
        (*lev2new).next[0] = &lev2newfinish;
        lev2newfinish.prev[0] = lev2new;

        a = (*activedel).next[1];
        while (a != &activedelfinish) {
            b = (*a).next[1];
            if ((*a).cis != 0) {
                jpn4[(*a).m1][(*a).m2].element[(*a).m3] = a;
                (*a).next[1] = &finish1;
                (*a).prev[1] = finish1.prev[1];
                (*finish1.prev[1]).next[1] = a;
                finish1.prev[1] = a;
                (*a).notchoise = true;
            } else {

                (*(*a).prev[0]).next[0] = (*a).next[0];
                (*(*a).next[0]).prev[0] = (*a).prev[0];
                free(a);
            }
            a = b;

        }
        (*activedel).next[1] = &activedelfinish;
        activedelfinish.prev[1] = activedel;

/*		a=start.next[1];
		while (a!=&finish)
		{
			b = (*a).next[1];
			(*a).next[1]=&finish1;
			(*a).prev[1] = finish1.prev[1];
			(*finish1.prev[1]).next[1] = a;
			finish1.prev[1] = a;
			(*a).notchoise=true;
			a=b;

		}
		start.next[1] = &finish;
		finish.prev[1] = &start;
*/
        c = (*passivedel).next[1];
        while (c != &passivedelfinish) {
            d = (*c).next[1];
            if ((*c).cis != 0) {
                (*c).next[1] = &jpn4[(*c).m12][(*c).m22].finish[(*c).m32];
                (*c).prev[1] = jpn4[(*c).m12][(*c).m22].finish[(*c).m32].prev[1];
                (*jpn4[(*c).m12][(*c).m22].finish[(*c).m32].prev[1]).next[1] = c;
                jpn4[(*c).m12][(*c).m22].finish[(*c).m32].prev[1] = c;

//				(*(*c).prev[2]).next[2] = (*c).next[2];
//				(*(*c).next[2]).prev[2] = (*c).prev[2];
                (*c).next[2] = &jpn4[(*c).m1][(*c).m2].finish[(*c).m3];
                (*c).prev[2] = jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2];
                (*jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2]).next[2] = c;
                jpn4[(*c).m1][(*c).m2].finish[(*c).m3].prev[2] = c;

            } else {
//				(*(*c).prev[1]).next[1] = (*c).next[1];
//				(*(*c).next[1]).prev[1] = (*c).prev[1];
                (*(*c).prev[0]).next[0] = (*c).next[0];
                (*(*c).next[0]).prev[0] = (*c).prev[0];
                free(c);
            }
            c = d;

        }
        (*passivedel).next[1] = &passivedelfinish;
        passivedelfinish.prev[1] = passivedel;
        (*passivedel).next[2] = &passivedelfinish;
        passivedelfinish.prev[2] = passivedel;

        for (k = kolic1[cis - 1] + 1; k < kolic + 1; k++) {
            jpn[m12[k]][m22[k]] = empty;
            netu++;
        }

        e = (*lev2del).next[1];
        while (e != &lev2delfinish) {
            f = (*e).next[1];
            if ((*e).cis != 0) {
                if ((*e).vid == 1) {
                    lev[(*e).i][(*e).j] = (*e).znach;
                    lev2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 2) {
                    prav[(*e).i][(*e).j] = (*e).znach;
                    prav2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 3) {
                    verh[(*e).i][(*e).j] = (*e).znach;
                    verh2[(*e).i][(*e).j] = e;
                } else {
                    niz[(*e).i][(*e).j] = (*e).znach;
                    niz2[(*e).i][(*e).j] = e;
                }

//				(*(*e).prev[0]).next[1] = e;
//				(*(*e).next[0]).prev[1] = e;
//				(*e).prev[1]=(*e).prev[0];
//				(*e).next[1] = (*e).next[0];
                (*e).prev[1] = NULL;
                (*e).next[1] = NULL;
            } else {
                (*(*e).prev[0]).next[0] = (*e).next[0];
                (*(*e).next[0]).prev[0] = (*e).prev[0];
                free(e);
            }

            e = f;
        }
        (*lev2del).next[1] = &lev2delfinish;
        lev2delfinish.prev[1] = lev2del;

        g = kluchlevfinish.prev;
        while (g != &kluchlevstart) {
            kluchlev[(*g).i][(*g).j] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }
        g = kluchpravfinish.prev;
        while (g != &kluchpravstart) {
            kluchprav[(*g).i][(*g).j] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }
        g = kluchverhfinish.prev;
        while (g != &kluchverhstart) {
            kluchverh[(*g).i][(*g).j] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }
        g = kluchnizfinish.prev;
        while (g != &kluchnizstart) {
            kluchniz[(*g).i][(*g).j] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }
        g = kluchfinish3.prev;
        while (g != &kluchstart3) {
            kluch12[(*g).i] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }
        g = kluchfinish4.prev;
        while (g != &kluchstart4) {
            kluch22[(*g).i] = false;
            h = (*g).prev;
            free(g);
            g = h;
        }

        kluchlevfinish.prev = &kluchlevstart;
        kluchpravfinish.prev = &kluchpravstart;
        kluchverhfinish.prev = &kluchverhstart;
        kluchnizfinish.prev = &kluchnizstart;

        kluchfinish3.prev = &kluchstart3;
        kluchfinish4.prev = &kluchstart4;


        if (m33 == yes)
            m33 = no;
        else
            m33 = yes;
        er = true;
        kolic = kolic1[cis - 1];


        activenew2 = (struct active *) malloc(sizeof(struct active));
        (*activenew2).cis = 0;
        activedel2 = (struct active *) malloc(sizeof(struct active));
        (*activedel2).cis = 0;


        for (i = 0; i <= 1; i++) {
            (*activenew2).prev[i] = activenew;
            (*activenew2).next[i] = &activenewfinish;
            (*activenew).next[i] = activenew2;
            activenewfinish.prev[i] = activenew2;

            (*activedel2).prev[i] = activedel;
            (*activedel2).next[i] = &activedelfinish;
            (*activedel).next[i] = activedel2;
            activedelfinish.prev[i] = activedel2;

        }

        passivenew2 = (struct passive *) malloc(sizeof(struct passive));
        (*passivenew2).cis = 0;
        passivedel2 = (struct passive *) malloc(sizeof(struct passive));
        (*passivedel2).cis = 0;

        for (i = 0; i <= 2; i++) {
            (*passivenew2).prev[i] = passivenew;
            (*passivenew2).next[i] = &passivenewfinish;
            (*passivenew).next[i] = passivenew2;
            passivenewfinish.prev[i] = passivenew2;

            (*passivedel2).prev[i] = passivedel;
            (*passivedel2).next[i] = &passivedelfinish;
            (*passivedel).next[i] = passivedel2;
            passivedelfinish.prev[i] = passivedel2;

        }

        lev2new2 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2new2).cis = 0;
        lev2del2 = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2del2).cis = 0;

        for (i = 0; i <= 1; i++) {
            (*lev2new2).prev[i] = lev2new;
            (*lev2new2).next[i] = &lev2newfinish;
            (*lev2new).next[i] = lev2new2;
            lev2newfinish.prev[i] = lev2new2;

            (*lev2del2).prev[i] = lev2del;
            (*lev2del2).next[i] = &lev2delfinish;
            (*lev2del).next[i] = lev2del2;
            lev2delfinish.prev[i] = lev2del2;

        }

        kolic++;
        m12[kolic] = m13;
        m22[kolic] = m23;
        jpn[m13][m23] = m33;
        netu--;
        j1 = 0;
        while (m23 >= lev[m13][j1])
            j1++;
        j1--;
        if (kluchlev[m13][j1] == false) {
            kluchlev[m13][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m13;
            (*g).j = j1;

            (*g).prev = kluchlevfinish.prev;
            kluchlevfinish.prev = g;
        }
        j1 = rowNumCount[m13] + 1;
        while (m23 <= prav[m13][j1])
            j1--;
        j1++;
        if (kluchprav[m13][j1] == false) {
            kluchprav[m13][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m13;
            (*g).j = j1;

            (*g).prev = kluchpravfinish.prev;
            kluchpravfinish.prev = g;
        }
        j1 = 0;
        while (m13 >= verh[m23][j1])
            j1++;
        j1--;
        if (kluchverh[m23][j1] == false) {
            kluchverh[m23][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m23;
            (*g).j = j1;

            (*g).prev = kluchverhfinish.prev;
            kluchverhfinish.prev = g;
        }
        j1 = colNumCount[m23] + 1;
        while (m13 <= niz[m23][j1])
            j1--;
        j1++;
        if (kluchniz[m23][j1] == false) {
            kluchniz[m23][j1] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m23;
            (*g).j = j1;

            (*g).prev = kluchnizfinish.prev;
            kluchnizfinish.prev = g;
        }

        if (kluch12[m13] == false) {
            kluch12[m13] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m13;

            (*g).prev = kluchfinish3.prev;
            kluchfinish3.prev = g;
        }
        if (kluch22[m23] == false) {
            kluch22[m23] = true;
            g = (struct kluch *) malloc(sizeof(struct kluch));
            (*g).i = m23;

            (*g).prev = kluchfinish4.prev;
            kluchfinish4.prev = g;
        }

        resheniee();

        if (er)
            if (netu == 0) {
                return;
            }
        if (er == false) {
            e = lev2delfinish.prev[1];
            while (e != lev2del2) {
                f = (*e).prev[1];
                if ((*e).vid == 1) {
                    lev[(*e).i][(*e).j] = (*e).znach;
                    lev2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 2) {
                    prav[(*e).i][(*e).j] = (*e).znach;
                    prav2[(*e).i][(*e).j] = e;
                } else if ((*e).vid == 3) {
                    verh[(*e).i][(*e).j] = (*e).znach;
                    verh2[(*e).i][(*e).j] = e;
                } else {
                    niz[(*e).i][(*e).j] = (*e).znach;
                    niz2[(*e).i][(*e).j] = e;
                }

                (*e).prev[1] = NULL;

                e = f;
            }
            lev2delfinish.prev[1] = lev2del2;

        }

        if (er) {
            k = kolic;
            for (i = kolic1[cis - 1] + 1; i <= kolic; i++) {
                i0 = m12[i];
                j0 = m22[i];
                for (j = 0; j <= 1; j++) {
                    cond = (jpn4[i0][j0].element[j] != NULL);
                    if (cond) {

                        b = jpn4[i0][j0].element[j];
                        (*(*b).prev[1]).next[1] = (*b).next[1];
                        (*(*b).next[1]).prev[1] = (*b).prev[1];

                        if ((*b).cis < cis + 1) {
                            (*b).prev[1] = (*activedel2).prev[1];
                            (*b).next[1] = activedel2;
                            (*(*activedel2).prev[1]).next[1] = b;
                            (*activedel2).prev[1] = b;
                        } else {
                            (*(*b).prev[0]).next[0] = (*b).next[0];
                            (*(*b).next[0]).prev[0] = (*b).prev[0];
                            free(b);
                        }
                        jpn4[i0][j0].element[j] = NULL;

                        d = jpn4[i0][j0].start[j].next[1];
                        while (d != &(jpn4[i0][j0].finish[j])) {
                            c = (*d).next[1];

                            (*(*d).prev[2]).next[2] = (*d).next[2];
                            (*(*d).next[2]).prev[2] = (*d).prev[2];

                            if ((*d).cis < cis + 1) {
                                (*d).prev[1] = (*passivedel2).prev[1];
                                (*d).next[1] = passivedel2;
                                (*(*passivedel2).prev[1]).next[1] = d;
                                (*passivedel2).prev[1] = d;
                                (*d).prev[2] = (*passivedel).prev[2];
                                (*d).next[2] = passivedel;
                                (*(*passivedel).prev[2]).next[2] = d;
                                (*passivedel).prev[2] = d;

                            } else {
                                (*(*d).prev[0]).next[0] = (*d).next[0];
                                (*(*d).next[0]).prev[0] = (*d).prev[0];
                                free(d);
                            }

                            d = c;

                        }
                        jpn4[i0][j0].start[j].next[1] = &(jpn4[i0][j0].finish[j]);
                        jpn4[i0][j0].finish[j].prev[1] = &(jpn4[i0][j0].start[j]);
                    }
                    d = jpn4[i0][j0].start[j].next[2];
                    while (d != &(jpn4[i0][j0].finish[j])) {
                        c = (*d).next[2];
                        if (jpn[(*d).m12][(*d).m22] == empty) {
                            b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                            if ((*b).notchoise) {
                                (*b).notchoise = false;
                                (*(*b).prev[1]).next[1] = (*b).next[1];
                                (*(*b).next[1]).prev[1] = (*b).prev[1];
                                (*b).next[1] = &finish;
                                (*b).prev[1] = finish.prev[1];
                                (*finish.prev[1]).next[1] = b;
                                finish.prev[1] = b;
                            }
                        }
                        (*(*d).prev[1]).next[1] = (*d).next[1];
                        (*(*d).next[1]).prev[1] = (*d).prev[1];
                        if ((*d).cis < cis + 1) {
                            (*d).prev[1] = (*passivedel2).prev[1];
                            (*d).next[1] = passivedel2;
                            (*(*passivedel2).prev[1]).next[1] = d;
                            (*passivedel2).prev[1] = d;
                            (*d).prev[2] = (*passivedel).prev[2];
                            (*d).next[2] = passivedel;
                            (*(*passivedel).prev[2]).next[2] = d;
                            (*passivedel).prev[2] = d;

                        } else {
                            (*(*d).prev[0]).next[0] = (*d).next[0];
                            (*(*d).next[0]).prev[0] = (*d).prev[0];
                            free(d);
                        }

                        d = c;

                    }
                    jpn4[i0][j0].start[j].next[2] = &(jpn4[i0][j0].finish[j]);
                    jpn4[i0][j0].finish[j].prev[2] = &(jpn4[i0][j0].start[j]);

                }
                if (jpn[i0 + 1][j0] == empty && jpn4[i0 + 1][j0].notchoise) {
                    jpn4[i0 + 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 + 1;
                    m22[k] = j0;
                }
                if (jpn[i0 - 1][j0] == empty && jpn4[i0 - 1][j0].notchoise) {
                    jpn4[i0 - 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 - 1;
                    m22[k] = j0;
                }
                if (jpn[i0][j0 + 1] == empty && jpn4[i0][j0 + 1].notchoise) {
                    jpn4[i0][j0 + 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 + 1;
                }
                if (jpn[i0][j0 - 1] == empty && jpn4[i0][j0 - 1].notchoise) {
                    jpn4[i0][j0 - 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 - 1;
                }

            }
            for (i = kolic + 1; i <= k; i++) {
                i0 = m12[i];
                j0 = m22[i];
                jpn4[i0][j0].notchoise = true;
                for (j = 0; j <= 1; j++) {
                    cond = (jpn4[i0][j0].element[j] != NULL);
                    if (cond) {

                        b = jpn4[i0][j0].element[j];
                        if ((*b).notchoise) {
                            (*b).notchoise = false;
                            (*(*b).prev[1]).next[1] = (*b).next[1];
                            (*(*b).next[1]).prev[1] = (*b).prev[1];
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                        }
                    }
                    d = jpn4[i0][j0].start[j].next[2];
                    while (d != &(jpn4[i0][j0].finish[j])) {


                        b = jpn4[(*d).m12][(*d).m22].element[(*d).m32];
                        if ((*b).notchoise) {
                            (*b).notchoise = false;
                            (*(*b).prev[1]).next[1] = (*b).next[1];
                            (*(*b).next[1]).prev[1] = (*b).prev[1];
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                        }

                        d = (*d).next[2];
                    }

                }
            }


            kolic1[cis] = kolic;

            e = lev2delfinish.prev[1];
            (*e).next[1] = &lev2delfinish;
            while (e != lev2del2) {
                f = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*f).cis = cis + 1;
                (*f).vid = (*e).vid;
                (*f).i = (*e).i;
                (*f).j = (*e).j;
                if ((*e).vid == 1) {
                    (*f).znach = lev[(*e).i][(*e).j];
                    lev2[(*e).i][(*e).j] = f;

                } else if ((*e).vid == 2) {
                    (*f).znach = prav[(*e).i][(*e).j];
                    prav2[(*e).i][(*e).j] = f;
                } else if ((*e).vid == 3) {
                    (*f).znach = verh[(*e).i][(*e).j];
                    verh2[(*e).i][(*e).j] = f;
                } else {
                    (*f).znach = niz[(*e).i][(*e).j];
                    niz2[(*e).i][(*e).j] = f;
                }
                (*f).prev[0] = (*lev2new2).prev[0];
                (*f).next[0] = lev2new2;
                (*(*lev2new2).prev[0]).next[0] = f;
                (*lev2new2).prev[0] = f;
                (*f).prev[1] = NULL;
                (*f).next[1] = NULL;
                f = (*e).prev[1];
                (*f).next[1] = e;
                if ((*e).cis == cis + 1) {
                    (*(*e).prev[0]).next[0] = (*e).next[0];
                    (*(*e).next[0]).prev[0] = (*e).prev[0];
                    (*(*e).prev[1]).next[1] = (*e).next[1];
                    (*(*e).next[1]).prev[1] = (*e).prev[1];
                    free(e);


                }


                e = f;
            }
            (*(*lev2del2).prev[1]).next[1] = (*lev2del2).next[1];
            (*(*lev2del2).next[1]).prev[1] = (*lev2del2).prev[1];
            (*lev2del2).prev[1] = lev2delfinish.prev[1];
            (*lev2del2).next[1] = &lev2delfinish;
            (*lev2delfinish.prev[1]).next[1] = lev2del2;
            lev2delfinish.prev[1] = lev2del2;


            podgon(cis + 1, activenew2, activedel2, passivenew2, passivedel2, lev2new2, lev2del2);
        }


    }

}

void kluchFree(struct kluch kl) {
    struct kluch *h = kl.prev;
    struct kluch *g = NULL;

    while (h->prev != NULL) {
        g = h->prev;
        free(h);
        h = g;
    }
}

JNIEXPORT jint JNICALL
Java_com_picross_nonocross_util_usergrid_CheckUnique_checkUniqueFromJNI(
        JNIEnv *env,
        jobject this,
        jint height0,
        jint width0,
        jintArray row_num_count0,
        jintArray col_num_count0,
        jintArray row_nums_flat,
        jintArray col_nums_flat) {
    //kol = 0;
    height = height0;
    width = width0;

    jint *row_num_count_1 = (*env)->GetIntArrayElements(env, row_num_count0, 0);
    jint *col_num_count_1 = (*env)->GetIntArrayElements(env, col_num_count0, 0);
    jint *row_nums_flat0 = (*env)->GetIntArrayElements(env, row_nums_flat, 0);
    jint *col_nums_flat0 = (*env)->GetIntArrayElements(env, col_nums_flat, 0);

    rowNums = (int **) malloc((height + 2) * sizeof(int *));
    colNums = (int **) malloc((width + 2) * sizeof(int *));
    rowNumCount = (int *) malloc((height + 2) * sizeof(int));
    colNumCount = (int *) malloc((width + 2) * sizeof(int));

    int k = 0;
    for(int i = 0; i<height; i++){
        rowNumCount[i+2] = row_num_count_1[i];
        rowNums[i + 2] = (int *) malloc((row_num_count_1[i] + 2) * sizeof(int));
        for(int j = 0;j < rowNumCount[i+2]; j++){
            rowNums[i+2][j+1] = row_nums_flat0[k];
            k++;
        }
    }
    __android_log_write(ANDROID_LOG_DEBUG, "Parsed", "rows");

    (*env)->ReleaseIntArrayElements(env, row_num_count0, row_num_count_1, 0);
    (*env)->ReleaseIntArrayElements(env, row_nums_flat, row_nums_flat0, 0);

    k = 0;
    for(int i = 0; i<width; i++){
        colNumCount[i+2] = col_num_count_1[i];
        colNums[i + 2] = (int *) malloc((col_num_count_1[i] + 2) * sizeof(int));
        for(int j = 0;j < colNumCount[i+2]; j++){
            colNums[i+2][j+1] = col_nums_flat0[k];
            k++;
        }
    }

    (*env)->ReleaseIntArrayElements(env, col_num_count0, col_num_count_1, 0);
    (*env)->ReleaseIntArrayElements(env, col_nums_flat, col_nums_flat0, 0);

    __android_log_write(ANDROID_LOG_DEBUG, "Parsed", "cols");

    time1 = clock();
    if (mainy() == 0) {
        /*char str[10];
        sprintf(str, "%d", kol);
        __android_log_write(ANDROID_LOG_DEBUG, "Solutions", str);
        time2 = clock() - time1;
        sprintf(str, "%f", (double) time2 / (double) CLOCKS_PER_SEC);
        __android_log_write(ANDROID_LOG_DEBUG, "TimeTaken", str);*/
        postroenie(jpn);
        deallocate();
        return kol;
    } else {
        deallocate();
        return 0;
    }
}