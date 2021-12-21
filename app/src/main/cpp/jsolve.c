/**
 * This Programm, which is solving Japan Croswords.
 * author: Syromolotov Evgeniy.
 * email: zhesyr@sampo.ru.
 * License: GPL2. */

//#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <jni.h>
#include <android/log.h>

typedef char bool;
#define true 1
#define false 0

//bool xpm = false; // -x
bool all = false; // -a
//bool hp = false;
//bool qt = false;  // -q
//bool vb = false;  // -v
int to = 0;      // -n
//int  mo1 =0;       // -m
int mo = 0;       // -m

const unsigned char pusto = 2;
const unsigned char yes = 1;
const unsigned char no = 0;
//const unsigned char board = 4;
//const unsigned char mas[3]={'X',' ','*'};

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

unsigned char **jpn, m3;
int height, width, k, i, j;
#ifdef DEBUG
int test2=0;
#endif
int **stro, **sto, **lev, **prav, **verh, **niz;
int *kolstr, *kolsto, /*m1, m2,*/ *m12, *m22;
bool er, **kluchlev, **kluchprav, **kluchverh, **kluchniz, *kluch12, *kluch22, perebor = false;
int netu, netu4, netu5, netu6, netu7, sumstr, sumsto,/*k0,*/kolic, kol = 0;
int *kolic1;
clock_t time1, time2;
const char *name;
char *file;
double sec;

struct field **jpn4;
struct passive passivenewstart, passivenewfinish, passivedelstart, passivedelfinish, *passivenew, *passivedel;
struct active activenewstart, activenewfinish, activedelstart, activedelfinish, *activenew, *activedel, start, finish, start1, finish1;

struct levpravverhniz ***lev2, ***prav2, ***verh2, ***niz2, lev2newstart, lev2newfinish, lev2delstart, lev2delfinish, *lev2new, *lev2del;

struct kluch kluchlevstart, kluchlevfinish, kluchverhstart, kluchverhfinish, kluchpravstart, kluchpravfinish, kluchnizstart, kluchnizfinish, kluchstart3, kluchfinish3, kluchstart4, kluchfinish4 = {
        0, 0, NULL};

int parse();

void allocate();

void deallocate();

void kluchFree(struct kluch);

void vved(void);

//void postroenie(unsigned char **jpn);

//void postroenie1(unsigned char **jpn);

void resheniee();

void podgon(int, struct active *, struct active *, struct passive *, struct passive *,
            struct levpravverhniz *, struct levpravverhniz *);

int main2() {
    perebor = false;
    vved();
    er = true;
    resheniee();
    if (er == false) {
        //time2 = clock() - time1;
        //printf("Finish\n");
        //sec = (double) time2/(double) CLOCKS_PER_SEC;
        //printf("%s%.2f%s","time = ",sec," second\n");
        printf("No solutions\n");
        return 0;
    }
    if (netu == 0) {
        kol = kol + 1;
        //time2 = clock() - time1;
        //printf("Finish\n");
        //sec = (double)time2/(double)CLOCKS_PER_SEC;
        //printf("%s%.2f%s","time = ",sec," second\n");
        /*if (xpm)
			postroenie1(jpn);
		else
			postroenie(jpn);
		printf("%d%s",kol," solution\n");*/
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

        activenew = (struct active *) malloc(sizeof(struct active));
        (*activenew).cis = 0;
        activedel = (struct active *) malloc(sizeof(struct active));
        (*activedel).cis = 0;

        for (int ii = 0; ii <= 1; ii++) {
            (*activenew).prev[ii] = &activenewstart;
            (*activenew).next[ii] = &activenewfinish;
            activenewstart.next[ii] = activenew;
            activenewfinish.prev[ii] = activenew;

            (*activedel).prev[ii] = &activedelstart;
            (*activedel).next[ii] = &activedelfinish;
            activedelstart.next[ii] = activedel;
            activedelfinish.prev[ii] = activedel;
        }


        passivenew = (struct passive *) malloc(sizeof(struct passive));
        (*passivenew).cis = 0;
        passivedel = (struct passive *) malloc(sizeof(struct passive));
        (*passivedel).cis = 0;

        for (int ii = 0; ii <= 2; ii++) {
            (*passivenew).prev[ii] = &passivenewstart;
            (*passivenew).next[ii] = &passivenewfinish;
            passivenewstart.next[ii] = passivenew;
            passivenewfinish.prev[ii] = passivenew;

            (*passivedel).prev[ii] = &passivedelstart;
            (*passivedel).next[ii] = &passivedelfinish;
            passivedelstart.next[ii] = passivedel;
            passivedelfinish.prev[ii] = passivedel;
        }


        jpn4 = (struct field **) malloc((height + 2) * sizeof(struct field *));

        for (int ii = 2; ii <= height + 1; ii++) {
            jpn4[ii] = (struct field *) malloc((width + 2) * sizeof(struct field));

            for (int jj = 2; jj <= width + 1; jj++)
                if (jpn[ii][jj] == pusto) {

                    jpn4[ii][jj].start[0].next[1] = &(jpn4[ii][jj].finish[0]);
                    jpn4[ii][jj].start[1].next[1] = &(jpn4[ii][jj].finish[1]);
                    jpn4[ii][jj].start[0].next[2] = &(jpn4[ii][jj].finish[0]);
                    jpn4[ii][jj].start[1].next[2] = &(jpn4[ii][jj].finish[1]);

                    jpn4[ii][jj].finish[0].prev[1] = &(jpn4[ii][jj].start[0]);
                    jpn4[ii][jj].finish[1].prev[1] = &(jpn4[ii][jj].start[1]);
                    jpn4[ii][jj].finish[0].prev[2] = &(jpn4[ii][jj].start[0]);
                    jpn4[ii][jj].finish[1].prev[2] = &(jpn4[ii][jj].start[1]);
                    jpn4[ii][jj].notchoise = true;

                    a = (struct active *) malloc(sizeof(struct active));
                    (*a).cis = 1;
                    (*a).m1 = ii;
                    (*a).m2 = jj;
                    (*a).m3 = yes;
                    (*a).netu = 0;
                    (*a).notchoise = false;
                    (*a).next[0] = activenew;
                    (*a).prev[0] = (*activenew).prev[0];
                    (*(*activenew).prev[0]).next[0] = a;
                    (*activenew).prev[0] = a;
                    (*a).next[1] = &finish;
                    (*a).prev[1] = finish.prev[1];
                    (*finish.prev[1]).next[1] = a;
                    finish.prev[1] = a;
                    jpn4[ii][jj].element[1] = a;

                    a = (struct active *) malloc(sizeof(struct active));
                    (*a).cis = 1;
                    (*a).m1 = ii;
                    (*a).m2 = jj;
                    (*a).m3 = no;
                    (*a).netu = 0;
                    (*a).notchoise = false;
                    (*a).next[0] = activenew;
                    (*a).prev[0] = (*activenew).prev[0];
                    (*(*activenew).prev[0]).next[0] = a;
                    (*activenew).prev[0] = a;
                    (*a).next[1] = &finish;
                    (*a).prev[1] = finish.prev[1];
                    (*finish.prev[1]).next[1] = a;
                    finish.prev[1] = a;
                    jpn4[ii][jj].element[0] = a;


                }
        }
    }

    {
        struct levpravverhniz *a;

        lev2new = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2new).cis = 0;
        lev2del = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
        (*lev2del).cis = 0;

        for (int ii = 0; ii <= 1; ii++) {
            (*lev2new).prev[ii] = &lev2newstart;
            (*lev2new).next[ii] = &lev2newfinish;
            lev2newstart.next[ii] = lev2new;
            lev2newfinish.prev[ii] = lev2new;

            (*lev2del).prev[ii] = &lev2delstart;
            (*lev2del).next[ii] = &lev2delfinish;
            lev2delstart.next[ii] = lev2del;
            lev2delfinish.prev[ii] = lev2del;
        }

        lev2 = (struct levpravverhniz ***) malloc((height + 2) * sizeof(struct levpravverhniz **));
        prav2 = (struct levpravverhniz ***) malloc((height + 2) * sizeof(struct levpravverhniz **));
        for (int ii = 2; ii <= height + 1; ii++) {
            lev2[ii] = (struct levpravverhniz **) malloc(
                    (kolstr[ii] + 2) * sizeof(struct levpravverhniz *));
            prav2[ii] = (struct levpravverhniz **) malloc(
                    (kolstr[ii] + 2) * sizeof(struct levpravverhniz *));

            for (int jj = 0; jj <= kolstr[ii] + 1; jj++) {
                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 1;
                (*a).znach = lev[ii][jj];
                (*a).next[0] = lev2new;
                (*a).prev[0] = (*lev2new).prev[0];
                (*(*lev2new).prev[0]).next[0] = a;
                (*lev2new).prev[0] = a;
//			(*a).next[1]=lev2new;
//			(*a).prev[1]=(*lev2new).prev[1];
//			(*(*lev2new).prev[1]).next[1]=a;
//			(*lev2new).prev[1]=a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                lev2[ii][jj] = a;

                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 2;
                (*a).znach = prav[ii][jj];
                (*a).next[0] = lev2new;
                (*a).prev[0] = (*lev2new).prev[0];
                (*(*lev2new).prev[0]).next[0] = a;
                (*lev2new).prev[0] = a;

//			(*a).next[1]=lev2new;
//			(*a).prev[1]=(*lev2new).prev[1];
//			(*(*lev2new).prev[1]).next[1]=a;
//			(*lev2new).prev[1]=a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                prav2[ii][jj] = a;

            }

        }


        verh2 = (struct levpravverhniz ***) malloc((width + 2) * sizeof(struct levpravverhniz **));
        niz2 = (struct levpravverhniz ***) malloc((width + 2) * sizeof(struct levpravverhniz **));

        for (int ii = 2; ii <= width + 1; ii++) {
            verh2[ii] = (struct levpravverhniz **) malloc(
                    (kolsto[ii] + 2) * sizeof(struct levpravverhniz *));
            niz2[ii] = (struct levpravverhniz **) malloc(
                    (kolsto[ii] + 2) * sizeof(struct levpravverhniz *));

            for (int jj = 0; jj <= kolsto[ii] + 1; jj++) {
                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 3;
                (*a).znach = verh[ii][jj];
                (*a).next[0] = lev2new;
                (*a).prev[0] = (*lev2new).prev[0];
                (*(*lev2new).prev[0]).next[0] = a;
                (*lev2new).prev[0] = a;
//			(*a).next[1]=lev2new;
//			(*a).prev[1]=(*lev2new).prev[1];
//			(*(*lev2new).prev[1]).next[1]=a;
//			(*lev2new).prev[1]=a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                verh2[ii][jj] = a;

                a = (struct levpravverhniz *) malloc(sizeof(struct levpravverhniz));
                (*a).cis = 1;
                (*a).i = ii;
                (*a).j = jj;
                (*a).vid = 4;
                (*a).znach = niz[ii][jj];
                (*a).next[0] = lev2new;
                (*a).prev[0] = (*lev2new).prev[0];
                (*(*lev2new).prev[0]).next[0] = a;
                (*lev2new).prev[0] = a;
//			(*a).next[1]=lev2new;
//			(*a).prev[1]=(*lev2new).prev[1];
//			(*(*lev2new).prev[1]).next[1]=a;
//			(*lev2new).prev[1]=a;

                (*a).next[1] = NULL;
                (*a).prev[1] = NULL;

                niz2[ii][jj] = a;

            }

        }


    }


    podgon(1, activenew, activedel, passivenew, passivedel, lev2new, lev2del);

    /*if (all==false)
	{
		if (er)
		{
			//time2 = clock() - time1;
			if (xpm)
				postroenie1(jpn);
			else
				postroenie(jpn);
			printf("Finish\n");
			//sec = (double)time2/(double)CLOCKS_PER_SEC;
			//printf("%s%.2f%s","time = ",sec," second\n");
#ifdef DEBUG
			printf("%s%d%s","test2= ",test2,"\n");
#endif

			return 0;
		}
		else
		{
			//time2 = clock() - time1;
			//printf("Finish\n");
			//sec = (double)time2/(double)CLOCKS_PER_SEC;
			//printf("%s%.2f%s","time = ",sec," second\n");
			if (to==0)
				printf("No solutions\n");
			else
				printf("%d%s",kol," solution\n");

			return 0;

		}
	}*/
    //time2 = clock() - time1;
    //printf("Finish\n");
    //sec = (double)time2/(double)CLOCKS_PER_SEC;
    //printf("%s%.2f%s","time = ",sec," second\n");
    //printf("%d%s",kol," solution\n");

    return 0;


}

int mainy() {
    if (parse() == 1) {
        //printf("scaf has error");
        return 1;
    }
    //printf("Start\n");
    time1 = clock();

    main2();

    return 0;
}

int parse() {
    all = true;
    /*int i,j,k;
	char c;
	bool jump=false;
	i=1;
	while (argv[i][0]=='-')
	{
		j=1;
		while (argv[i][j]!='\0')
		{
			if (argv[i][j]=='x')
				xpm=true;
			else
			if (argv[i][j]=='a')
				all=true;
			else
			if (argv[i][j]=='q')
				qt=true;
			else
			if (argv[i][j]=='v')
				vb=true;
			else
			if (argv[i][j]=='n')
			{
				jump=true;
				k=0;
				to=0;
				while (argv[i+1][k]!='\0')
				{
					c=argv[i+1][k];
					if (c<'0' || c>'9')
					{
						help();
						return;
					}
					to = 10*to+(c-'0');
					k++;
				}
			}
			else
			if (argv[i][j]=='m')
			{
				jump=true;
				k=0;
				mo=0;
				while (argv[i+1][k]!='\0')
				{
					c=argv[i+1][k];
					if (c<'0' || c>'9')
					{
						help();
						return;
					}
					mo = 10*mo+(c-'0');
					k++;
				}
			}


			else
			{
				help();
				return;
			}
			j=j+1;

		}
		if (jump)
		{
			jump=false;
			i=i+1;
		}

		i=i+1;
	}
	if (i==argc-1)
		name=argv[i];
	else
	{
		help();
		return;
	}*/
//	char name[20];
//	std::cout << "Введите название файла\n";
//	std::cin >> name;


    char *stroka;
    height = 0;

    stroka = (char *) name;
    while (*stroka != '#') {
        while (*stroka != '\n') {
            stroka++;
        }
        height++;
        stroka++;
    }
    while (*stroka != '\n') {
        stroka++;
    }
    width = 0;
    stroka++;
    while (*stroka != '#') {
        while (*stroka != '\n') {
            stroka++;
        }
        width++;
        stroka++;
    }
    stro = (int **) malloc((height + 2) * sizeof(int *));
    sto = (int **) malloc((width + 2) * sizeof(int *));
    kolstr = (int *) malloc((height + 2) * sizeof(int));
    kolsto = (int *) malloc((width + 2) * sizeof(int));
    stroka = (char *) name;
    int movePos;
    for (i = 1; i < height + 1; i++) {
        j = 0;
        do {
            if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
                return 1;
            stroka += movePos;
            if (k != 0) {
                j++;
            }

        } while (k != 0);
        kolstr[i + 1] = j;
        stro[i + 1] = (int *) malloc((j + 2) * sizeof(int));
    }
    do
        if (*(stroka + 1) == 0)
            return 1;
    while (*stroka++ != '\n');
    do
        if (*(stroka + 1) == 0)
            return 1;
    while (*stroka++ != '\n');

    for (i = 1; i < width + 1; i++) {
        j = 0;
        do {
            if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
                return 1;
            stroka += movePos;
            if (k != 0) {
                j++;
            }

        } while (k != 0);
        kolsto[i + 1] = j;
        sto[i + 1] = (int *) malloc((j + 2) * sizeof(int));
    }
    stroka = (char *) name;
    for (i = 1; i < height + 1; i++) {
        for (j = 1; j < kolstr[i + 1] + 1; j++) {
            if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
                return 1;
            stroka += movePos;
            stro[i + 1][j] = k;
        }
        if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
            return 1;
        stroka += movePos;
    }
    do
        if (*(stroka + 1) == 0)
            return 1;
    while (*stroka++ != '\n');
    do
        if (*(stroka + 1) == 0)
            return 1;
    while (*stroka++ != '\n');
    for (i = 1; i < width + 1; i++) {
        for (j = 1; j < kolsto[i + 1] + 1; j++) {
            if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
                return 1;
            stroka += movePos;
            sto[i + 1][j] = k;
        }
        if (sscanf(stroka, "%d%n", &k, &movePos) != 1)
            return 1;
        stroka += movePos;

    }
    return 0;
}

void allocate() {
    jpn = (unsigned char **) malloc((height + 4) * sizeof(unsigned char *));

    for (int ii = 0; ii < height + 4; ii++) {
        jpn[ii] = (unsigned char *) malloc((width + 4) * sizeof(unsigned char));
    }

    lev = (int **) malloc((height + 2) * sizeof(int *));
    prav = (int **) malloc((height + 2) * sizeof(int *));
    kluchlev = (bool **) malloc((height + 2) * sizeof(bool *));
    kluchprav = (bool **) malloc((height + 2) * sizeof(bool *));
    kluch12 = (bool *) malloc((height + 2) * sizeof(bool));

    for (int ii = 2; ii < height + 2; ii++) {
        lev[ii] = (int *) malloc((kolstr[ii] + 2) * sizeof(int));
        prav[ii] = (int *) malloc((kolstr[ii] + 2) * sizeof(int));
        kluchlev[ii] = (bool *) malloc((kolstr[ii] + 2) * sizeof(bool));
        kluchprav[ii] = (bool *) malloc((kolstr[ii] + 2) * sizeof(bool));

//		kluch12[ii] = (bool*) malloc((kolstr[ii]+2)*sizeof(bool));

    }

    verh = (int **) malloc((width + 2) * sizeof(int *));
    niz = (int **) malloc((width + 2) * sizeof(int *));
    kluchverh = (bool **) malloc((width + 2) * sizeof(bool *));
    kluchniz = (bool **) malloc((width + 2) * sizeof(bool *));
    kluch22 = (bool *) malloc((width + 2) * sizeof(bool));

    for (int ii = 2; ii < width + 2; ii++) {
        verh[ii] = (int *) malloc((kolsto[ii] + 2) * sizeof(int));
        niz[ii] = (int *) malloc((kolsto[ii] + 2) * sizeof(int));
        kluchverh[ii] = (bool *) malloc((kolsto[ii] + 2) * sizeof(bool));
        kluchniz[ii] = (bool *) malloc((kolsto[ii] + 2) * sizeof(bool));

//		kluch22[ii] = (bool*) malloc((kolsto[ii]+2)*sizeof(bool));

    }


    kluchlevfinish.prev = &kluchlevstart;
    kluchpravfinish.prev = &kluchpravstart;
    kluchverhfinish.prev = &kluchverhstart;
    kluchnizfinish.prev = &kluchnizstart;

    kluchfinish3.prev = &kluchstart3;
    kluchfinish4.prev = &kluchstart4;

}

void deallocate() {
    for (int ii = 0; ii < height + 4; ii++) {
        free(jpn[ii]);
        jpn[ii] = NULL;
    }

    free(jpn);
    jpn = NULL;


    for (int ii = 2; ii < height + 2; ii++) {
        free(lev[ii]);
        lev[ii] = NULL;
        free(prav[ii]);
        prav[ii] = NULL;
        free(kluchlev[ii]);
        kluchlev[ii] = NULL;
        free(kluchprav[ii]);
        kluchprav[ii] = NULL;
//		kluch12[ii] = (bool*) malloc((kolstr[ii]+2)*sizeof(bool));
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

    for (int ii = 2; ii < width + 2; ii++) {
        free(verh[ii]);
        verh[ii] = NULL;
        free(niz[ii]);
        niz[ii] = NULL;
        free(kluchverh[ii]);
        kluchverh[ii] = NULL;
        free(kluchniz[ii]);
        kluchniz[ii] = NULL;

//		kluch22[ii] = (bool*) malloc((kolsto[ii]+2)*sizeof(bool));

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

    for (int ii = 1; ii < height + 1; ii++) {
        free(stro[ii + 1]);
        stro[ii + 1] = NULL;
    }
    for (int ii = 1; ii < width + 1; ii++) {
        free(sto[ii + 1]);
        sto[ii + 1] = NULL;
    }

    free(stro);
    stro = NULL;
    free(sto);
    sto = NULL;
    free(kolstr);
    kolstr = NULL;
    free(kolsto);
    kolsto = NULL;

    kluchFree(kluchpravfinish);
    kluchFree(kluchfinish4);
    kluchFree(kluchnizfinish);
    kluchFree(kluchlevfinish);
    kluchFree(kluchfinish3);
    kluchFree(kluchverhfinish);

}

void vved() {
    int i, j, k, s, j1;
    struct kluch *g;
    allocate();


//	kluch1[1]=kluch2[1]=kluch12[1]=kluch22[1]=false;

    for (int ii = 2; ii < height + 2; ii++)
        for (j = 2; j < width + 2; j++)
            jpn[ii][j] = pusto;

    for (int ii = 2; ii < height + 2; ii++) {
//         	kluch1[ii]=false;
        kluch12[ii] = false;
        jpn[ii][1] = no;
        jpn[ii][width + 2] = no;
        jpn[ii][0] = yes;
        jpn[ii][width + 3] = yes;

    }

    for (int ii = 2; ii < width + 2; ii++) {
        //       	kluch2[ii]=false;
        kluch22[ii] = false;
        jpn[1][ii] = no;
        jpn[height + 2][ii] = no;
        jpn[0][ii] = yes;
        jpn[height + 3][ii] = yes;

    }
    netu = height * width;
    sumstr = 0;
    sumsto = 0;
    for (int ii = 2; ii < height + 2; ii++) {
        lev[ii][0] = 0;
        prav[ii][0] = 0;
        stro[ii][0] = 1;
        k = kolstr[ii];
        lev[ii][k + 1] = width + 3;
        prav[ii][k + 1] = width + 3;
        stro[ii][k + 1] = 1;
        for (int jj = 1; jj < k + 1; jj++)
            sumstr = sumstr + stro[ii][jj];
        for (int jj = 1; jj < k + 1; jj++) {
            kluchlev[ii][jj] = false;
            lev[ii][jj] = lev[ii][jj - 1] + stro[ii][jj - 1] + 1;

        }
        for (int jj = k; jj > 0; jj--) {
            kluchprav[ii][jj] = false;
            prav[ii][jj] = prav[ii][jj + 1] - stro[ii][jj + 1] - 1;
        }
    }

    for (int ii = 2; ii < width + 2; ii++) {
        verh[ii][0] = 0;
        niz[ii][0] = 0;
        sto[ii][0] = 1;
        k = kolsto[ii];
        verh[ii][k + 1] = height + 3;
        niz[ii][k + 1] = height + 3;
        sto[ii][k + 1] = 1;
        for (int jj = 1; jj < k + 1; jj++)
            sumsto = sumsto + sto[ii][jj];
        for (int jj = 1; jj < k + 1; jj++) {
            kluchverh[ii][jj] = false;
            verh[ii][jj] = verh[ii][jj - 1] + sto[ii][jj - 1] + 1;

        }

        for (int jj = k; jj > 0; jj--) {
            kluchniz[ii][jj] = false;
            niz[ii][jj] = niz[ii][jj + 1] - sto[ii][jj + 1] - 1;
        }


    }

    for (int ii = 2; ii < height + 2; ii++) {
        k = kolstr[ii];
        for (int jj = 1; jj < k + 1; jj++)
            if ((prav[ii][jj] - lev[ii][jj] + 1) < (2 * stro[ii][jj]))
                for (s = (prav[ii][jj] - stro[ii][jj] + 1); s < (lev[ii][jj] + stro[ii][jj]); s++)
                    if (jpn[ii][s] == pusto) {
                        jpn[ii][s] = yes;
                        j1 = 0;
                        while (ii >= verh[s][j1])
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
                        j1 = kolsto[s] + 1;
                        while (ii <= niz[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
        for (int jj = 1; jj <= k + 1; jj++)
            if ((lev[ii][jj] - prav[ii][jj - 1] - 1) > 0)
                for (s = (prav[ii][jj - 1] + 1); s < (lev[ii][jj]); s++)
                    if (jpn[ii][s] == pusto) {
                        jpn[ii][s] = no;
                        j1 = 0;
                        while (ii >= verh[s][j1])
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
                        j1 = kolsto[s] + 1;
                        while (ii <= niz[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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

    for (int ii = 2; ii < width + 2; ii++) {
        k = kolsto[ii];
        for (int jj = 1; jj < k + 1; jj++)
            if ((niz[ii][jj] - verh[ii][jj] + 1) < (2 * sto[ii][jj]))
                for (s = (niz[ii][jj] - sto[ii][jj] + 1); s < (verh[ii][jj] + sto[ii][jj]); s++)
                    if (jpn[s][ii] == pusto) {
                        jpn[s][ii] = yes;
                        j1 = 0;
                        while (ii >= lev[s][j1])
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
                        j1 = kolstr[s] + 1;
                        while (ii <= prav[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
        for (int jj = 1; jj <= k + 1; jj++)
            if ((verh[ii][jj] - niz[ii][jj - 1] - 1) > 0)
                for (s = (niz[ii][jj - 1] + 1); s < (verh[ii][jj]); s++)
                    if (jpn[s][ii] == pusto) {
                        jpn[s][ii] = no;
                        j1 = 0;
                        while (ii >= lev[s][j1])
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
                        j1 = kolstr[s] + 1;
                        while (ii <= prav[s][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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

/*void postroenie(unsigned char **jpn)
{
	if (qt==false)
	{
		int i,j;
		//printf("   ");
		for (i=1;i<width+1;i++)
		{
			if ((i%10)==0)
				printf("%d",(i/10)%10);
			else
				printf(" ");
		}
		printf("\n   ");
		for (i=1;i<width+1;i++)
			printf("%d",i%10);
		printf("\n  ");
		for (i=0;i<width+2;i++)
			printf("*");
		printf("\n");

		for (i=2;i<height+2;i++)
		{
			printf("%d%d%s",(((i-1)/10)%10),((i-1)%10),"*");
			for (j=2;j<width+2;j++)
			{
				printf("%c",mas[jpn[i][j]]);
			}
			printf("*\n");
		}
		printf("  ");
		for (i=0;i<width+2;i++)
			printf("*");
		printf("\n");
	}
}

void postroenie1(unsigned char **jpn)
{
	int i,j,s,t;
	unsigned int j0;
	const int k=1;
	char name2[200];
	FILE *outfile;
	sprintf(name2,"%s%s%d.xpm",name,"_",kol);
	outfile = fopen(name2,"w");
	//fprintf(outfile,"/%s/\n","* XPM *");
	//fprintf(outfile,"static char *test_xpm[] = {\n");
	//fprintf(outfile,"%s%d%s%d%s","\"",width*k," ",height*k," 2 1\",\n");
	//fprintf(outfile,"\"X c white\",\n");
	//fprintf(outfile,"\" c black\",\n");
	for (i=2;i<height+2;i++)
	{
		for (s=1;s<k+1;s++)
		{
			//fprintf(outfile,"\"");
			for (j=2;j<width+2;j++)
			{
				for (t=1;t<k+1;t++)
				{
					fprintf(outfile,"%c",mas[jpn[i][j]]);
				}
			}
			fprintf(outfile,"\n");
		}
	}
	//fprintf(outfile,"};");
	fclose(outfile);

}*/

void reshstr1(int i, int j) {
    int s, p, q, t, a, b, c, j1, teklev = lev[i][j], teklev2 = lev[i][j + 1];
    bool sdvig = false, sdvig2 = false;
    struct kluch *g, *g1;
    s = teklev;
    p = s - 1;
    q = s - 2;
    label:
    while ((s < teklev + stro[i][j]) && (s <= prav[i][j])) {
        if (jpn[i][s] == no) {

            while (jpn[i][s] == no)
                s++;
            if ((prav[i][j] - s + 1) < (stro[i][j])) {
                er = false;
                return;
            }

            teklev = s;
            sdvig = true;

            p = s - 1;
        }

        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s <= prav[i][j])) {
                s++;
                t++;
            }
            if (t > stro[i][j]) {
                if ((prav[i][j] - s + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                teklev = s;
                sdvig = true;


            } else if (teklev < s - stro[i][j]) {
                if ((prav[i][j] - (s - stro[i][j]) + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                teklev = s - stro[i][j];
                sdvig = true;


            }
        }

        while ((jpn[i][s] == pusto) && (s <= teklev + stro[i][j]) && (s <= prav[i][j]))
            s++;

    }

    if (teklev2 < teklev + stro[i][j] + 1) {
        if ((prav[i][j + 1] - (teklev + stro[i][j] + 1) + 1) < (stro[i][j + 1])) {
            er = false;
            return;
        }

        teklev2 = teklev + stro[i][j] + 1;
        sdvig2 = true;

    }

    while ((s < teklev2) && (s <= prav[i][j])) {

        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s <= prav[i][j])) {
                s++;
                t++;
            }

            if (t > stro[i][j]) {
                if ((prav[i][j] - s + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                teklev = s;
                sdvig = true;
                goto label;
            }

            if (teklev < s - stro[i][j]) {

                if ((prav[i][j] - (s - stro[i][j]) + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }


                teklev = s - stro[i][j];
                sdvig = true;
                s = teklev;
                p = s - 1;
                q = s - 2;
                goto label;


            }

            if (teklev2 < teklev + stro[i][j] + 1) {
                if ((prav[i][j + 1] - (teklev + stro[i][j] + 1) + 1) < (stro[i][j + 1])) {
                    er = false;
                    return;
                }

                teklev2 = teklev + stro[i][j] + 1;
                sdvig2 = true;

            }
        }

        if (jpn[i][s] == no) {
            if (s - p - 1 < stro[i][j]) {
                if (q > p) {
                    while ((jpn[i][s] == no) && (s <= prav[i][j]))
                        s++;
                    if ((prav[i][j] - s + 1) < (stro[i][j])) {
                        er = false;
                        return;
                    }


                    teklev = s;
                    sdvig = true;

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
        while ((jpn[i][s] == pusto) && (s < teklev2) && (s <= prav[i][j]))
            s++;
    }

    if ((lev[i][kolstr[i] + 1] != (width + 3))) {
        er = false;
        return;
    }

    if (sdvig) {
        if ((prav[i][j] - teklev + 1) < (stro[i][j])) {
            er = false;
            return;
        }
        if ((prav[i][j] - teklev + 1) < (2 * stro[i][j])) {
            for (s = (prav[i][j] - stro[i][j] + 1); s < (teklev + stro[i][j]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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

        if (teklev - prav[i][j - 1] > 1)
            for (s = prav[i][j - 1] + 1; s < teklev; s++) {
                if (jpn[i][s] == yes) {

                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {

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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
    if (sdvig2) {
        if ((prav[i][j + 1] - teklev2 + 1) < (stro[i][j + 1])) {
            er = false;
            return;
        }
        if ((prav[i][j + 1] - teklev2 + 1) < (2 * stro[i][j + 1])) {
            for (s = (prav[i][j + 1] - stro[i][j + 1] + 1); s < (teklev2 + stro[i][j + 1]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
        if (teklev2 - prav[i][j] > 1)
            for (s = prav[i][j] + 1; s < teklev2; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {

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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
    if (sdvig) {
        lev[i][j] = teklev;
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
    if (sdvig2) {
        lev[i][j + 1] = teklev2;
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
    int s, p, q, t, c, a, b, j1, tekprav = prav[i][j], tekprav2 = prav[i][j - 1];
    bool sdvig = false, sdvig2 = false;
    struct kluch *g;
    s = tekprav;
    p = s + 1;
    q = s + 2;
    label:
    while ((s > tekprav - stro[i][j]) && (s >= lev[i][j])) {

        if (jpn[i][s] == no) {
            while (jpn[i][s] == no)
                s--;
            if ((s - lev[i][j] + 1) < (stro[i][j])) {
                er = false;
                return;
            }

            tekprav = s;
            sdvig = true;

            p = s + 1;
        }

        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s >= lev[i][j])) {
                s--;
                t++;
            }
            if (t > stro[i][j]) {
                if ((s - lev[i][j] + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                tekprav = s;
                sdvig = true;


            } else if (tekprav > s + stro[i][j]) {
                if (((s + stro[i][j]) - lev[i][j] + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                tekprav = s + stro[i][j];
                sdvig = true;

            }

        }
        while ((jpn[i][s] == pusto) && (s >= tekprav - stro[i][j]) && (s >= lev[i][j]))
            s--;
    }

    if (tekprav2 > tekprav - stro[i][j] - 1) {
        if (((tekprav - stro[i][j] - 1) - lev[i][j - 1] + 1) < (stro[i][j - 1])) {
            er = false;
            return;
        }

        tekprav2 = tekprav - stro[i][j] - 1;
        sdvig2 = true;

    }

    while ((s > tekprav2) && (s >= lev[i][j])) {
        if (jpn[i][s] == yes) {
            t = 0;
            q = s;
            while ((jpn[i][s] == yes) && (s >= lev[i][j])) {
                s--;
                t++;
            }
            if (t > stro[i][j]) {
                if ((s - lev[i][j] + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                tekprav = s;
                sdvig = true;

                goto label;
            }
            if (tekprav > s + stro[i][j]) {
                if (((s + stro[i][j]) - lev[i][j] + 1) < (stro[i][j])) {
                    er = false;
                    return;
                }

                tekprav = s + stro[i][j];
                sdvig = true;
                s = tekprav;
                p = s + 1;
                q = s + 2;

                goto label;


            }
            if (tekprav2 > tekprav - stro[i][j] - 1) {
                if (((tekprav - stro[i][j] - 1) - lev[i][j - 1] + 1) < (stro[i][j - 1])) {
                    er = false;
                    return;
                }

                tekprav2 = tekprav - stro[i][j] - 1;
                sdvig2 = true;

            }
        }

        if (jpn[i][s] == no) {
            if (p - s - 1 < stro[i][j]) {
                if (q < p) {
                    while ((jpn[i][s] == no) && (s >= lev[i][j]))
                        s--;
                    if ((s - lev[i][j] + 1) < (stro[i][j])) {
                        er = false;
                        return;
                    }

                    tekprav = s;
                    sdvig = true;

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

        while ((jpn[i][s] == pusto) && (s > tekprav2) && (s >= lev[i][j]))
            s--;
    }
    if ((prav[i][0] != 0)) {
        er = false;
        return;
    }

    if (sdvig) {
        if ((tekprav - lev[i][j] + 1) < (stro[i][j])) {
            er = false;
            return;
        }
        if ((tekprav - lev[i][j] + 1) < (2 * stro[i][j])) {
            for (s = (tekprav - stro[i][j] + 1); s < (lev[i][j] + stro[i][j]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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

        if (lev[i][j + 1] - tekprav > 1)
            for (s = tekprav + 1; s < lev[i][j + 1]; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {

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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
    if (sdvig2) {
        if ((tekprav2 - lev[i][j - 1] + 1) < (stro[i][j - 1])) {
            er = false;
            return;
        }
        if ((tekprav2 - lev[i][j - 1] + 1) < (2 * stro[i][j - 1])) {
            for (s = (tekprav2 - stro[i][j - 1] + 1); s < (lev[i][j - 1] + stro[i][j - 1]); s++) {
                if (jpn[i][s] == no) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
        if (lev[i][j] - tekprav2 > 1)
            for (s = tekprav2 + 1; s < lev[i][j]; s++) {
                if (jpn[i][s] == yes) {
                    er = false;
                    return;
                }
                if (jpn[i][s] == pusto) {

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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
    if (sdvig) {
        prav[i][j] = tekprav;
        if ((j < kolstr[i]) && (kluchprav[i][j + 1] == false)) {
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
    if (sdvig2) {
        prav[i][j - 1] = tekprav2;
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
    int s, p, q, t, a, b, c, j1, tekverh = verh[i][j], tekverh2 = verh[i][j + 1];
    bool sdvig = false, sdvig2 = false;
    struct kluch *g;
    s = tekverh;
    p = s - 1;
    q = s - 2;
    label:
    while ((s < tekverh + sto[i][j]) && (s <= niz[i][j])) {
        if (jpn[s][i] == no) {

            while (jpn[s][i] == no)
                s++;

            if ((niz[i][j] - s + 1) < (sto[i][j])) {
                er = false;
                return;
            }

            tekverh = s;
            sdvig = true;

            p = s - 1;
        }

        if (jpn[s][i] == yes) {
            t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s <= niz[i][j])) {
                s++;
                t++;
            }
            if (t > sto[i][j]) {
                if ((niz[i][j] - s + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekverh = s;
                sdvig = true;

            } else if (tekverh < s - sto[i][j]) {
                if ((niz[i][j] - (s - sto[i][j]) + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekverh = s - sto[i][j];
                sdvig = true;

            }
        }
        while ((jpn[s][i] == pusto) && (s <= tekverh + sto[i][j]) && (s <= niz[i][j]))
            s++;
    }

    if (tekverh2 < tekverh + sto[i][j] + 1) {
        if ((niz[i][j + 1] - (tekverh + sto[i][j] + 1) + 1) < (sto[i][j + 1])) {
            er = false;
            return;
        }
        tekverh2 = tekverh + sto[i][j] + 1;
        sdvig2 = true;
    }

    while ((s < tekverh2) && (s <= niz[i][j])) {
        if (jpn[s][i] == yes) {
            t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s <= niz[i][j])) {
                s++;
                t++;
            }

            if (t > sto[i][j]) {
                if ((niz[i][j] - s + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekverh = s;
                sdvig = true;

                goto label;
            }

            if (tekverh < s - sto[i][j]) {
                if ((niz[i][j] - (s - sto[i][j]) + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekverh = s - sto[i][j];
                sdvig = true;
                s = tekverh;
                p = s - 1;
                q = s - 2;

                goto label;

            }

            if (tekverh2 < tekverh + sto[i][j] + 1) {
                if ((niz[i][j + 1] - (tekverh + sto[i][j] + 1) + 1) < (sto[i][j + 1])) {
                    er = false;
                    return;
                }

                tekverh2 = tekverh + sto[i][j] + 1;
                sdvig2 = true;
            }
        }

        if (jpn[s][i] == no) {
            if (s - p - 1 < sto[i][j]) {
                if (q > p) {
                    while ((jpn[s][i] == no) && (s <= niz[i][j]))
                        s++;
                    if ((niz[i][j] - s + 1) < (sto[i][j])) {
                        er = false;
                        return;
                    }

                    tekverh = s;
                    sdvig = true;
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
        while ((jpn[s][i] == pusto) && (s < tekverh2) && (s <= niz[i][j]))
            s++;
    }

    if ((verh[i][kolsto[i] + 1] != (height + 3))) {
        er = false;
        return;
    }

    if (sdvig) {
        if ((niz[i][j] - tekverh + 1) < (sto[i][j])) {
            er = false;
            return;
        }
        if ((niz[i][j] - tekverh + 1) < (2 * sto[i][j])) {
            for (s = (niz[i][j] - sto[i][j] + 1); s < (tekverh + sto[i][j]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {
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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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

        if (tekverh - niz[i][j - 1] > 1)
            for (s = niz[i][j - 1] + 1; s < tekverh; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
    if (sdvig2) {
        if ((niz[i][j + 1] - tekverh2 + 1) < (sto[i][j + 1])) {
            er = false;
            return;
        }
        if ((niz[i][j + 1] - tekverh2 + 1) < (2 * sto[i][j + 1])) {
            for (s = (niz[i][j + 1] - sto[i][j + 1] + 1); s < (tekverh2 + sto[i][j + 1]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {
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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
        if (tekverh2 - niz[i][j] > 1)
            for (s = niz[i][j] + 1; s < tekverh2; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
    if (sdvig) {
        verh[i][j] = tekverh;
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
    if (sdvig2) {
        verh[i][j + 1] = tekverh2;
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
    int s, p, q, t, c, a, b, j1, tekniz = niz[i][j], tekniz2 = niz[i][j - 1];
    bool sdvig = false, sdvig2 = false;
    struct kluch *g, *g1;
    s = tekniz;
    p = s + 1;
    q = s + 2;
    label:
    while ((s > tekniz - sto[i][j]) && (s >= verh[i][j])) {
        if (jpn[s][i] == no) {
            while (jpn[s][i] == no)
                s--;
            if ((s - verh[i][j] + 1) < (sto[i][j])) {
                er = false;
                return;
            }

            tekniz = s;
            sdvig = true;

            p = s + 1;
        }

        if (jpn[s][i] == yes) {
            t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s >= verh[i][j])) {
                s--;
                t++;
            }
            if (t > sto[i][j]) {
                if ((s - verh[i][j] + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekniz = s;
                sdvig = true;

            } else if (tekniz > s + sto[i][j]) {
                if (((s + sto[i][j]) - verh[i][j] + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekniz = s + sto[i][j];
                sdvig = true;
            }
        }

        while ((jpn[s][i] == pusto) && (s >= tekniz - sto[i][j]) && (s >= verh[i][j]))
            s--;

    }

    if (tekniz2 > tekniz - sto[i][j] - 1) {

        if (((tekniz - sto[i][j] - 1) - verh[i][j - 1] + 1) < (sto[i][j - 1])) {
            er = false;
            return;
        }

        tekniz2 = tekniz - sto[i][j] - 1;
        sdvig2 = true;

    }

    while ((s > tekniz2) && (s >= verh[i][j])) {

        if (jpn[s][i] == yes) {
            t = 0;
            q = s;
            while ((jpn[s][i] == yes) && (s >= verh[i][j])) {
                s--;
                t++;
            }
            if (t > sto[i][j]) {
                if ((s - verh[i][j] + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekniz = s;
                sdvig = true;

                goto label;
            }
            if (tekniz > s + sto[i][j]) {
                if (((s + sto[i][j]) - verh[i][j] + 1) < (sto[i][j])) {
                    er = false;
                    return;
                }

                tekniz = s + sto[i][j];
                sdvig = true;

                s = tekniz;
                p = s + 1;
                q = s + 2;

                goto label;


            }
            if (tekniz2 > tekniz - sto[i][j] - 1) {
                if (((tekniz - sto[i][j] - 1) - verh[i][j - 1] + 1) < (sto[i][j - 1])) {
                    er = false;
                    return;
                }

                tekniz2 = tekniz - sto[i][j] - 1;
                sdvig2 = true;

            }
        }

        if (jpn[s][i] == no) {
            if (p - s - 1 < sto[i][j]) {
                if (q < p) {
                    while ((jpn[s][i] == no) && (s >= verh[i][j]))
                        s--;
                    if ((s - verh[i][j] + 1) < (sto[i][j])) {
                        er = false;
                        return;
                    }

                    tekniz = s;
                    sdvig = true;

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

        while ((jpn[s][i] == pusto) && (s > tekniz2) && (s >= verh[i][j]))
            s--;
    }
    if ((niz[i][0] != 0)) {
        er = false;
        return;
    }

    if (sdvig) {
        if ((tekniz - verh[i][j] + 1) < (sto[i][j])) {
            er = false;
            return;
        }
        if ((tekniz - verh[i][j] + 1) < (2 * sto[i][j])) {
            for (s = (tekniz - sto[i][j] + 1); s < (verh[i][j] + sto[i][j]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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

        if (verh[i][j + 1] - tekniz > 1)
            for (s = tekniz + 1; s < verh[i][j + 1]; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
    if (sdvig2) {
        if ((tekniz2 - verh[i][j - 1] + 1) < (sto[i][j - 1])) {
            er = false;
            return;
        }
        if ((tekniz2 - verh[i][j - 1] + 1) < (2 * sto[i][j - 1])) {
            for (s = (tekniz2 - sto[i][j - 1] + 1); s < (verh[i][j - 1] + sto[i][j - 1]); s++) {
                if (jpn[s][i] == no) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
        if (verh[i][j] - tekniz2 > 1)
            for (s = tekniz2 + 1; s < verh[i][j]; s++) {
                if (jpn[s][i] == yes) {
                    er = false;
                    return;
                }
                if (jpn[s][i] == pusto) {

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
                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
    if (sdvig) {
        niz[i][j] = tekniz;
        if ((j < kolsto[i]) && (kluchniz[i][j + 1] == false)) {
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
    if (sdvig2) {
        niz[i][j - 1] = tekniz2;
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
                while ((prav[i][a] >= s - 1) && (stro[i][a] > (s - (*p) - 1)))
                    a--;
                if ((prav[i][a] < s - 1) && (stro[i][a + 1] > (s - (*p) - 1)))
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
                        j1 = kolstr[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                        j1 = kolsto[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
                max = stro[i][j];
                a = j - 1;
                while (prav[i][a] >= s - 1) {
                    if (stro[i][a] > max)
                        max = stro[i][a];
                    a--;
                }
                if (s - max - 2 - (*p) >= 0) {
                    min = 0;
                    if (prav[i][j - 1] >= (*q) - 2)
                        min = stro[i][j - 1];
                    a = j - 2;
                    while (prav[i][a] > (*p)) {
                        if ((stro[i][a] < min) && (stro[i][a + 1] >= s - (*q)))
                            min = stro[i][a];
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
                            j1 = kolstr[i] + 1;
                            while (b <= prav[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                            j1 = kolsto[b] + 1;
                            while (i <= niz[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
                min = stro[i][j];
                a = j - 1;
                while (prav[i][a] >= s - 1) {
                    if ((stro[i][a] < min) && (stro[i][a] >= s - (*q)))
                        min = stro[i][a];
                    a--;
                }
                for (b = s; b < (*p) + min + 1; b++) {
                    if (jpn[i][b] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[i][b] == pusto) {
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
                        j1 = kolstr[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                        j1 = kolsto[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
                k = (*q);
                while (jpn[i][k] == yes)
                    k++;
                if ((k == s - 1) && (jpn[i][k] == pusto)) {
                    k = s;
                    while (jpn[i][k] == yes)
                        k++;
                    if (lev[i][j] <= (*q))
                        a = j;
                    else
                        a = j - 1;
                    while ((prav[i][a] >= (*q)) && ((*q) + stro[i][a] < k - 1))
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
                        j1 = kolstr[i] + 1;
                        while (s - 1 <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                        j1 = kolsto[s - 1] + 1;
                        while (i <= niz[s - 1][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[s - 1] + 1) && (kluchniz[s - 1][j1] == false)) {
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
            if ((t == stro[i][j]) && (jpn[i][s] == pusto)) {
                a = j;
                while ((prav[i][a] >= s - 1) &&
                       ((stro[i][a] <= stro[i][j]) || (prav[i][a] == s - 1)))
                    a--;
                if ((prav[i][a] < s - 1) &&
                    ((stro[i][a + 1] <= stro[i][j]) || (prav[i][a + 1] == s - 1))) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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

        while ((jpn[i][s] == pusto) && (s < lev[i][j + 1]) && (s <= prav[i][j]))
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
                while ((lev[i][a] <= s + 1) && (stro[i][a] > ((*p) - s - 1)))
                    a++;
                if ((lev[i][a] > s + 1) && (stro[i][a - 1] > ((*p) - s - 1)))
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
                        j1 = kolstr[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                        j1 = kolsto[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
                max = stro[i][j];
                a = j + 1;
                while (lev[i][a] <= s + 1) {
                    if (stro[i][a] > max)
                        max = stro[i][a];
                    a++;
                }
                if ((*p) - s - max - 2 >= 0) {
                    min = 0;
                    if (lev[i][j + 1] <= (*q) + 2)
                        min = stro[i][j + 1];
                    a = j + 2;
                    while (lev[i][a] < (*p)) {
                        if ((stro[i][a] < min) && (stro[i][a - 1] >= (*q) - s))
                            min = stro[i][a];
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
                            j1 = kolstr[i] + 1;
                            while (b <= prav[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                            j1 = kolsto[b] + 1;
                            while (i <= niz[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
                min = stro[i][j];
                a = j + 1;
                while (lev[i][a] <= s + 1) {
                    if ((stro[i][a] < min) && (stro[i][a] >= (*q) - s))
                        min = stro[i][a];
                    a++;
                }
                for (b = s; b > (*p) - min - 1; b--) {
                    if (jpn[i][b] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[i][b] == pusto) {
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
                        j1 = kolstr[i] + 1;
                        while (b <= prav[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                        j1 = kolsto[b] + 1;
                        while (i <= niz[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[b] + 1) && (kluchniz[b][j1] == false)) {
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
            if ((t == stro[i][j]) && (jpn[i][s] == pusto)) {
                a = j;
                while ((lev[i][a] <= s + 1) && ((stro[i][a] <= stro[i][j]) || (lev[i][a] == s + 1)))
                    a++;
                if ((lev[i][a] > s + 1) &&
                    ((stro[i][a - 1] <= stro[i][j]) || (lev[i][a - 1] == s + 1))) {
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
                    j1 = kolstr[i] + 1;
                    while (s <= prav[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[i] + 1) && (kluchprav[i][j1] == false)) {
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
                    j1 = kolsto[s] + 1;
                    while (i <= niz[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[s] + 1) && (kluchniz[s][j1] == false)) {
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
        while ((jpn[i][s] == pusto) && (s > prav[i][j - 1]) && (s >= lev[i][j]))
            s--;
    }
}

void reshstrdva(int i) {
    int j, p, q;
    q = 0;
    for (j = 1; j < kolstr[i] + 1; j++) {
        if ((prav[i][j] - lev[i][j] + 1) > (stro[i][j]))
            reshstr3(i, j, &p, &q);
        else
            q = lev[i][j];
        if (er == false)
            return;
    }
    q = width + 3;
    for (j = kolstr[i]; j > 0; j--) {
        if ((prav[i][j] - lev[i][j] + 1) > (stro[i][j]))
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
                while ((niz[ri][a] >= s - 1) && (sto[ri][a] > (s - (*p) - 1)))
                    a--;
                if ((niz[ri][a] < s - 1) && (sto[ri][a + 1] > (s - (*p) - 1)))
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

                        j1 = kolsto[ri] + 1;
                        while (b <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[ri] + 1) && (kluchniz[ri][j1] == false)) {
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
                        j1 = kolstr[b] + 1;
                        while (ri <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
                max = sto[ri][rj];
                a = rj - 1;
                while (niz[ri][a] >= s - 1) {
                    if (sto[ri][a] > max)
                        max = sto[ri][a];
                    a--;
                }
                if (s - max - 2 - (*p) >= 0) {
                    min = 0;
                    if (niz[ri][rj - 1] >= (*q) - 2)
                        min = sto[ri][rj - 1];
                    a = rj - 2;
                    /* Original no condition that a >= 0 but it causes a crash otherwise */
                    while (a >= 0 && niz[ri][a] > (*p)) {
                        if ((sto[ri][a] < min) && (sto[ri][a + 1] >= s - (*q)))
                            min = sto[ri][a];
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

                            j1 = kolsto[ri] + 1;
                            while (b <= niz[ri][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolsto[ri] + 1) && (kluchniz[ri][j1] == false)) {
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
                            j1 = kolstr[b] + 1;
                            while (ri <= prav[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
                min = sto[ri][rj];
                a = rj - 1;
                while (niz[ri][a] >= s - 1) {
                    if ((sto[ri][a] < min) && (sto[ri][a] >= s - (*q)))
                        min = sto[ri][a];
                    a--;
                }
                for (b = s; b < (*p) + min + 1; b++) {
                    if (jpn[b][ri] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[b][ri] == pusto) {
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

                        j1 = kolsto[ri] + 1;
                        while (b <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[ri] + 1) && (kluchniz[ri][j1] == false)) {
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
                        j1 = kolstr[b] + 1;
                        while (ri <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
                k = (*q);
                while (jpn[k][ri] == yes)
                    k++;
                if ((k == s - 1) && (jpn[k][ri] == pusto)) {
                    k = s;
                    while (jpn[k][ri] == yes)
                        k++;
                    if (verh[ri][rj] <= (*q))
                        a = rj;
                    else
                        a = rj - 1;
                    while ((niz[ri][a] >= (*q)) && ((*q) + sto[ri][a] < k - 1))
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

                        j1 = kolsto[ri] + 1;
                        while (s - 1 <= niz[ri][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[ri] + 1) && (kluchniz[ri][j1] == false)) {
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
                        j1 = kolstr[s - 1] + 1;
                        while (ri <= prav[s - 1][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[s - 1] + 1) && (kluchprav[s - 1][j1] == false)) {
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
            if ((t == sto[ri][rj]) && (jpn[s][ri] == pusto)) {
                a = rj;
                while ((niz[ri][a] >= s - 1) &&
                       ((sto[ri][a] <= sto[ri][rj]) || (niz[ri][a] == s - 1)))
                    a--;
                if ((niz[ri][a] < s - 1) &&
                    ((sto[ri][a + 1] <= sto[ri][rj]) || (niz[ri][a + 1] == s - 1))) {
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

                    j1 = kolsto[ri] + 1;
                    while (s <= niz[ri][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[ri] + 1) && (kluchniz[ri][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (ri <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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

        while ((jpn[s][ri] == pusto) && (s < verh[ri][rj + 1]) && (s <= niz[ri][rj]))
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
                while ((verh[i][a] <= s + 1) && (sto[i][a] > ((*p) - s - 1)))
                    a++;
                if ((verh[i][a] > s + 1) && (sto[i][a - 1] > ((*p) - s - 1)))
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

                        j1 = kolsto[i] + 1;
                        while (b <= niz[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                        j1 = kolstr[b] + 1;
                        while (i <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
                max = sto[i][j];
                a = j + 1;
                while (verh[i][a] <= s + 1) {
                    if (sto[i][a] > max)
                        max = sto[i][a];
                    a++;
                }
                if ((*p) - s - max - 2 >= 0) {
                    min = 0;
                    if (verh[i][j + 1] <= (*q) + 2)
                        min = sto[i][j + 1];
                    a = j + 2;
                    //char asffd[100];
                    //sprintf(asffd, "%p %d", &(verh[i][a]), a );
                    //sprintf(asffd, "%d %d", a, kolsto[i] + 2);
                    //__android_log_write(ANDROID_LOG_DEBUG,"qwWer", asffd);
                    //verh[i][kolsto[i]+1] = 22;
                    /* Original no condition that a < kolsto[i] but it causes a crash otherwise */
                    while (a < kolsto[i] + 2 && verh[i][a] < (*p)) {
                        if ((sto[i][a] < min) && (sto[i][a - 1] >= (*q) - s))
                            min = sto[i][a];
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

                            j1 = kolsto[i] + 1;
                            while (b <= niz[i][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                            j1 = kolstr[b] + 1;
                            while (i <= prav[b][j1])
                                j1--;
                            j1++;
                            if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
                min = sto[i][j];
                a = j + 1;
                while (verh[i][a] <= s + 1) {
                    if ((sto[i][a] < min) && (sto[i][a] >= (*q) - s))
                        min = sto[i][a];
                    a++;
                }
                for (b = s; b > (*p) - min - 1; b--) {
                    if (jpn[b][i] == no) {
                        er = false;
                        return;
                    }
                    if (jpn[b][i] == pusto) {
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

                        j1 = kolsto[i] + 1;
                        while (b <= niz[i][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                        j1 = kolstr[b] + 1;
                        while (i <= prav[b][j1])
                            j1--;
                        j1++;
                        if ((j1 < kolstr[b] + 1) && (kluchprav[b][j1] == false)) {
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
            if ((t == sto[i][j]) && (jpn[s][i] == pusto)) {
                a = j;
                while ((verh[i][a] <= s + 1) && ((sto[i][a] <= sto[i][j]) || (verh[i][a] == s + 1)))
                    a++;
                if ((verh[i][a] > s + 1) &&
                    ((sto[i][a - 1] <= sto[i][j]) || (verh[i][a - 1] == s + 1))) {
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

                    j1 = kolsto[i] + 1;
                    while (s <= niz[i][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolsto[i] + 1) && (kluchniz[i][j1] == false)) {
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
                    j1 = kolstr[s] + 1;
                    while (i <= prav[s][j1])
                        j1--;
                    j1++;
                    if ((j1 < kolstr[s] + 1) && (kluchprav[s][j1] == false)) {
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
        while ((jpn[s][i] == pusto) && (s > niz[i][j - 1]) && (s >= verh[i][j]))
            s--;
    }
}

void reshstodva(int i) {
    int j, p, q;
    q = 0;
    for (j = 1; j < kolsto[i] + 1; j++) {
        if ((niz[i][j] - verh[i][j] + 1) > (sto[i][j]))
            reshsto3(i, j, &p, &q);
        else
            q = verh[i][j];
        if (er == false)
            return;
    }
    q = height + 3;
    for (j = kolsto[i]; j > 0; j--) {
        if ((niz[i][j] - verh[i][j] + 1) > (sto[i][j]))
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
        j1 = kolstr[i0] + 1;
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
        j1 = kolsto[j0] + 1;
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
            j1 = kolstr[(*c).m1] + 1;
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
            j1 = kolsto[(*c).m2] + 1;
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
            if (all == false && to == 0)
                return;
            else {
                kol = kol + 1;
                if (to == kol)
                    return;
                /*if (xpm == false)
					postroenie(jpn);
				else
					postroenie1(jpn);*/
                er = false;

            }
        }
        if (er == false) {

            for (k = kolic1[cis - 1] + 1; k < kolic + 1; k++) {
                jpn[m12[k]][m22[k]] = pusto;
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
            j1 = kolstr[i0] + 1;
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
            j1 = kolsto[j0] + 1;
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

            if ((er) && (netu == 0) && (all || to > 0)) {
                kol = kol + 1;
                if (to == kol)
                    return;
                /*if (xpm)
					postroenie1(jpn);
				else
					postroenie(jpn);*/
                er = false;
            }
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

                            if ((mo == 1) && (jpn4[(*d).m1][(*d).m2].start[(*d).m3].next[2] ==
                                              &(jpn4[(*d).m1][(*d).m2].finish[(*d).m3])) &&
                                (jpn[(*d).m1][(*d).m2] == pusto)) {
                                b = (struct active *) malloc(sizeof(struct active));
                                (*b).cis = cis;
                                (*b).m1 = (*d).m1;
                                (*b).m2 = (*d).m2;
                                (*b).m3 = (*d).m3;
                                (*b).netu = 0;
                                (*b).notchoise = false;
                                (*b).next[0] = activenew;
                                (*b).prev[0] = (*activenew).prev[0];
                                (*(*activenew).prev[0]).next[0] = b;
                                (*activenew).prev[0] = b;
                                (*b).next[1] = &finish;
                                (*b).prev[1] = finish.prev[1];
                                (*finish.prev[1]).next[1] = b;
                                finish.prev[1] = b;
                                jpn4[(*d).m1][(*d).m2].element[(*d).m3] = b;

                            }

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
                    if (mo == 0 || cond == false) {

                        d = jpn4[i0][j0].start[j].next[2];
                        while (d != &(jpn4[i0][j0].finish[j])) {

                            c = (*d).next[2];
                            if (jpn[(*d).m12][(*d).m22] == pusto) {
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

                }
                if (jpn[i0 + 1][j0] == pusto && jpn4[i0 + 1][j0].notchoise) {
                    jpn4[i0 + 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 + 1;
                    m22[k] = j0;
                }
                if (jpn[i0 - 1][j0] == pusto && jpn4[i0 - 1][j0].notchoise) {
                    jpn4[i0 - 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 - 1;
                    m22[k] = j0;
                }
                if (jpn[i0][j0 + 1] == pusto && jpn4[i0][j0 + 1].notchoise) {
                    jpn4[i0][j0 + 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 + 1;
                }
                if (jpn[i0][j0 - 1] == pusto && jpn4[i0][j0 - 1].notchoise) {
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
                    if (mo == 0 || cond == false) {

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

                if (mo == 1 && jpn4[(*c).m1][(*c).m2].element[(*c).m3] != NULL) {
                    b = jpn4[(*c).m1][(*c).m2].element[(*c).m3];


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
                    jpn4[(*c).m1][(*c).m2].element[(*c).m3] = NULL;

                    d = jpn4[(*c).m1][(*c).m2].start[(*c).m3].next[1];
                    while (d != &(jpn4[(*c).m1][(*c).m2].finish[(*c).m3])) {

                        (*(*d).prev[1]).next[1] = (*d).next[1];
                        (*(*d).next[1]).prev[1] = (*d).prev[1];
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

                        d = jpn4[(*c).m1][(*c).m2].start[(*c).m3].next[1];

                    }


                }

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
                jpn[m12[k]][m22[k]] = pusto;
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
    if (mo == 1) {
        netu4 = 0;
        a = start1.next[1];
        while (a != &finish1) {
            if ((*a).netu > netu4) {
                netu4 = (*a).netu;
                m13 = (*a).m1;
                m23 = (*a).m2;
                m33 = (*a).m3;
            }
            a = (*a).next[1];
        }
    } else {
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
    j1 = kolstr[m13] + 1;
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
    j1 = kolsto[m23] + 1;
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
            if (all || to > 0) {
                kol = kol + 1;
                if (to == kol)
                    return;
                /*if (xpm)
					postroenie1(jpn);
				else
					postroenie(jpn);*/
                er = false;
            } else
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

                        if ((mo == 1) && (jpn4[(*d).m1][(*d).m2].start[(*d).m3].next[2] ==
                                          &(jpn4[(*d).m1][(*d).m2].finish[(*d).m3])) &&
                            (jpn[(*d).m1][(*d).m2] == pusto)) {
                            b = (struct active *) malloc(sizeof(struct active));
                            (*b).cis = cis + 1;
                            (*b).m1 = (*d).m1;
                            (*b).m2 = (*d).m2;
                            (*b).m3 = (*d).m3;
                            (*b).netu = 0;
                            (*b).notchoise = false;
                            (*b).next[0] = activenew2;
                            (*b).prev[0] = (*activenew2).prev[0];
                            (*(*activenew2).prev[0]).next[0] = b;
                            (*activenew2).prev[0] = b;
                            (*b).next[1] = &finish;
                            (*b).prev[1] = finish.prev[1];
                            (*finish.prev[1]).next[1] = b;
                            finish.prev[1] = b;
                            jpn4[(*d).m1][(*d).m2].element[(*d).m3] = b;

                        }

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
                if (mo == 0 || cond == false) {

                    d = jpn4[i0][j0].start[j].next[2];

                    while (d != &(jpn4[i0][j0].finish[j])) {
                        c = (*d).next[2];
                        if (jpn[(*d).m12][(*d).m22] == pusto) {
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

            }
            if (jpn[i0 + 1][j0] == pusto && jpn4[i0 + 1][j0].notchoise) {
                jpn4[i0 + 1][j0].notchoise = false;
                k++;
                m12[k] = i0 + 1;
                m22[k] = j0;
            }
            if (jpn[i0 - 1][j0] == pusto && jpn4[i0 - 1][j0].notchoise) {
                jpn4[i0 - 1][j0].notchoise = false;
                k++;
                m12[k] = i0 - 1;
                m22[k] = j0;
            }
            if (jpn[i0][j0 + 1] == pusto && jpn4[i0][j0 + 1].notchoise) {
                jpn4[i0][j0 + 1].notchoise = false;
                k++;
                m12[k] = i0;
                m22[k] = j0 + 1;
            }
            if (jpn[i0][j0 - 1] == pusto && jpn4[i0][j0 - 1].notchoise) {
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
                if (mo == 0 || cond == false) {

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
//				(*f).prev[1] = (*lev2new).prev[1];
//				(*f).next[1]=lev2new;
//				(*(*lev2new).prev[1]).next[1]=f;
//				(*lev2new).prev[1]=f;
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
            jpn[m12[k]][m22[k]] = pusto;
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
        j1 = kolstr[m13] + 1;
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
        j1 = kolsto[m23] + 1;
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
                if (all || to > 0) {
                    kol = kol + 1;
                    if (to == kol)
                        return;
                    /*if (xpm)
						postroenie1(jpn);
					else
						postroenie(jpn);*/
                    er = false;
                } else
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

                            if ((mo == 1) && (jpn4[(*d).m1][(*d).m2].start[(*d).m3].next[2] ==
                                              &(jpn4[(*d).m1][(*d).m2].finish[(*d).m3])) &&
                                (jpn[(*d).m1][(*d).m2] == pusto)) {
                                b = (struct active *) malloc(sizeof(struct active));
                                (*b).cis = cis + 1;
                                (*b).m1 = (*d).m1;
                                (*b).m2 = (*d).m2;
                                (*b).m3 = (*d).m3;
                                (*b).netu = 0;
                                (*b).notchoise = false;
                                (*b).next[0] = activenew2;
                                (*b).prev[0] = (*activenew2).prev[0];
                                (*(*activenew2).prev[0]).next[0] = b;
                                (*activenew2).prev[0] = b;
                                (*b).next[1] = &finish;
                                (*b).prev[1] = finish.prev[1];
                                (*finish.prev[1]).next[1] = b;
                                finish.prev[1] = b;
                                jpn4[(*d).m1][(*d).m2].element[(*d).m3] = b;

                            }

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
                    if (mo == 0 || cond == false) {

                        d = jpn4[i0][j0].start[j].next[2];
                        while (d != &(jpn4[i0][j0].finish[j])) {
                            c = (*d).next[2];
                            if (jpn[(*d).m12][(*d).m22] == pusto) {
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

                }
                if (jpn[i0 + 1][j0] == pusto && jpn4[i0 + 1][j0].notchoise) {
                    jpn4[i0 + 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 + 1;
                    m22[k] = j0;
                }
                if (jpn[i0 - 1][j0] == pusto && jpn4[i0 - 1][j0].notchoise) {
                    jpn4[i0 - 1][j0].notchoise = false;
                    k++;
                    m12[k] = i0 - 1;
                    m22[k] = j0;
                }
                if (jpn[i0][j0 + 1] == pusto && jpn4[i0][j0 + 1].notchoise) {
                    jpn4[i0][j0 + 1].notchoise = false;
                    k++;
                    m12[k] = i0;
                    m22[k] = j0 + 1;
                }
                if (jpn[i0][j0 - 1] == pusto && jpn4[i0][j0 - 1].notchoise) {
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
                    if (mo == 0 || cond == false) {

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
//				(*f).prev[1] = (*lev2new).prev[1];
//				(*f).next[1]=lev2new;
//				(*(*lev2new).prev[1]).next[1]=f;
//				(*lev2new).prev[1]=f;
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
        jobject this /* this */,
        jstring string) {
    kol = 0;
    name = (*env)->GetStringUTFChars(env, string, NULL);
    //__android_log_write(ANDROID_LOG_DEBUG, "Grid", name);
    if (mainy() == 0) {
        char str[10];
        sprintf(str, "%d", kol);
        __android_log_write(ANDROID_LOG_DEBUG, "Solutions", str);
        time2 = clock() - time1;
        sprintf(str, "%f", (double) time2 / (double) CLOCKS_PER_SEC);
        __android_log_write(ANDROID_LOG_DEBUG, "TimeTaken", str);
        deallocate();
        return kol;
    } else {
        deallocate();
        return 0;
    }
}