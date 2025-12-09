#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <stdint.h>
#include <time.h>

#define tmax 5000	//max number of tests
#define rmax 100000	//max number of rounds in 1 test

/*
 
An example of the test round description for a testee:

Let TB be given board with chips or the board with chips after your move(s), TB[address] be an integer assigned to a cell with the address: zero mean an empty cell, non-zero mean a chip marked with the integer on the cell.

Let EM be a relocation of a chip to a horizontally or vertically adjacent zero cell. As a result of this relocation, the values ​​of TB[from] and TB[to] should swap.

Every test move must be either an EM or a sequence of several EMs. Let the move start address be SA and the move finish address be FA. Please note: FA must not be equal to SA. Let move number be MN.
After a valid move, you will be given a verified TB based on your SA, FA, which you must use to make your next move.

The goal of the test round is to complete it in the fewest number of moves. The test round ends after 2 step(s). A step ends when YOU by your last move, complete a formation of a straight horizontal or vertical or diagonal line of five or more chips with the same marking. After completing the step, you will receive and must use TB board with these updates: five successive chips are removed from the line (for cases with long or/and several line(s) it is done randomly), then five previously presented chips are randomly added to the free cells of the board.

For the sake of certainty, let in an cell address [i,j], i be the row number, j be the column number and both be numbered from zero.

The board with chips is:

2,1,2,2,2,1,1,1
2,1,2,2,1,2,1,1
1,1,2,1,2,1,1,1
2,2,1,2,2,0,2,2
0,0,1,1,0,2,2,0
2,1,0,0,2,2,2,2
1,2,2,0,1,2,1,2
1,0,0,1,1,2,2,1

The next five chips: 2,1,1,1,2

*/

//all input is 7 parametres
//
uint32_t
tsts,	// 1) the number of tests up to tmax
rounds,	// 2) the number of rounds in 1 test up to rmax
stp;	// 3) steps in 1 round
unsigned char
hm,		// 4) size of a side of square board
kcb,	// 5) how many of types of chips are placed on the board
nub,	// 6) number of chips on the board
dbg,	// 7) print debug info on console

emb;    // the number of empty sells on the board

unsigned char Csb[16][16],	//TB
	n5x, n5[5],		//next five chips
	TC[16][16], ina, jna, hmb, iko, jko, kp, ox5=0, bc, ca, uu=0, n=0;

signed char x0, y00, //SA
			x1, y01, //FA
		y2;
uint32_t
icr,
qn=0,			//number of finished lines >4 chips long
l, im1, im2=0, med;
float
mn=0.0,			//moves number
tb_mn[tmax*rmax],	//tb of mn i.e. moves made in every round
tst_mn=0.0,
tb_av[tmax];	//tb of means of tests
//tb_med[tmax];	//tb of medians of tests

int main(int argc, char** argv)
{
// Check the number of passed arguments
if(argc<8)
    {
    printf("Error: must be 7 arguments, not %d\n", argc - 1);
    return(1);
    }
tsts=atoi(argv[1]); rounds=atoi(argv[2]); stp=atoi(argv[3]);
hm=atoi(argv[4]); kcb=atoi(argv[5]); nub=atoi(argv[6]);
dbg=atoi(argv[7]);
printf("%d %d %d %d %d %d %d\n", tsts, rounds, stp, hm, kcb, nub, dbg);

emb=hm*hm - nub;

    time_t seconds; seconds = time(NULL);

	char nmf[100];
	sprintf(nmf, "raw_%dx%d_%d-%dx%d-%d-%d_%ld\0", tsts, rounds, stp, hm,hm,kcb,nub,seconds);
	FILE* f=fopen(nmf,"wb");
//the number of moves in each round is written to this file,
//i.e. the total moves of all steps of  each round.
//example: if in a two step round 1th step - 2moves, 2th step - 3moves, then will be written 5 (in 4 bytes)
//this is NOT a mean file
// IMPORTANT!!! Files to merge must have the same stp, hm, kcb, nub
	int sizd = sizeof(float);

uint32_t random(uint32_t min, uint32_t max) {
    return arc4random_uniform(max - min + 1) + min;
}
void dbgp(){
int8_t i, j;
for (i = 0; i < hm; i += 1){
	for (j = 0; j < hm; j += 1)	printf("%d", Csb[i][j]);
printf("\n");
}
}
void fz(int8_t fzx, int8_t fzy){
 if((fzx>0)&&(Csb[fzx-1][fzy]==0)&&(TC[fzx-1][fzy]==0))
   {TC[fzx-1][fzy]=1; fz((fzx-1),fzy);}
 if((fzx<hm-1)&&(Csb[fzx+1][fzy]==0)&&(TC[fzx+1][fzy]==0))
   {TC[fzx+1][fzy]=1; fz((fzx+1),fzy);}
 if((fzy>0)&&(Csb[fzx][fzy-1]==0)&&(TC[fzx][fzy-1]==0))
   {TC[fzx][fzy-1]=1; fz(fzx,(fzy-1));}
 if((fzy<hm-1)&&(Csb[fzx][fzy+1]==0)&&(TC[fzx][fzy+1]==0))
   {TC[fzx][fzy+1]=1; fz(fzx,(fzy+1));}
}

void ver(int8_t colr, int8_t i, int8_t j){
ina=i; jna=j;
while((ina-1) >= 0){
ina--;
if(colr != Csb[ina][j]){ina++; break;}
}
iko=i;
while((iko+1) < hm){
iko++;
if(colr != Csb[iko][j]){iko--; break;}
}
hmb=iko-ina+1; // line length
if (hmb>4) {ox5=1;}
}
//-----------------------------------------
void hor(int8_t colr, int8_t i, int8_t j){
jna=j; ina=i;
while((jna-1) >= 0){
jna--;
if(colr != Csb[i][jna]){jna++; break;}
}
jko=j;
while((jko+1) < hm){
jko++;
if(colr != Csb[i][jko]){jko--; break;}
}
hmb=jko-jna+1;
if (hmb>4) {ox5=1;}
}
//-----------------------------------------
void nak(int8_t colr, int8_t i, int8_t j){
ina=i; jna=j;
while((ina-1) >= 0 && (jna-1) >= 0){
ina--; jna--;
if(colr != Csb[ina][jna]){ina++; jna++; break;}
}
iko=i; jko=j;
while((iko+1) < hm && (jko+1) < hm){
iko++; jko++;
if(colr != Csb[iko][jko]){iko--; jko--; break;}
}
hmb=jko-jna+1;
if (hmb>4) {ox5=1;}
}
//-----------------------------------------
void nkl(int8_t colr, int8_t i, int8_t j){
ina=i; jna=j;
while((ina+1) < hm && (jna-1) >= 0){
ina++; jna--;
if(colr != Csb[ina][jna]){ina--; jna++; break;}
}
iko=i; jko=j;
while((iko-1) >= 0 && (jko+1) < hm){
iko--; jko++;
if(colr != Csb[iko][jko]){iko++; jko--; break;}
}
hmb=ina-iko+1;
if (hmb>4) {ox5=1;}
}
void ValuX(int8_t colr, int8_t i, int8_t j)
{

int b, e, n;

int32_t an;
int tt[4];
for (e = 0; e < 4; e++) tt[e] = -1;
    e = 0;
while (e < 4) {
		an=arc4random();
        n = 0;
        while (n < 16) {
            b = 3 & an;  //select 2 bits from the right
            if (tt[b] == -1) {
                tt[b] = e;
                e++;
                if (e == 4) break;
            }
            an = an >> 2;  //shift 2 bits to the right
            n++;
}
}
//trying to find a line in the order defined in tt
for (e=0; e<4; e++){ // e - order number
b=1;
n=0;
while(n<4){
/*looking for order number e
n - type number | - \ /
look for a certain type n 5 in a line*/
	if(tt[n]==e) {
	switch (n) {
  	case 0: ver(colr, i, j); break;
	case 1: hor(colr, i, j); break;
	case 2: nak(colr, i, j); break;
	case 3: nkl(colr, i, j); break;
	}
	}
	if(ox5>0 && (qn+1)<stp) {// not needed before newdesk
	if(hmb>4){//how many options for removal from a point with a min value of x
		if(hmb==5){bc=0;}
		else{
		bc=random(0, hmb - 5); //offset from point with min value of x
		}
	// delete 5 then add 5
	if(dbg==1) {printf("the number {0,1,2,3} of del type | - \\ / is %d\nbefore del:\n", n); dbgp(); printf("\n");}
	int8_t k=0;
	while(k<5){
	// zeroes 5 cells
	if(n==0){Csb[ina+bc+k][jna]=0;}
	if(n==1){Csb[ina][jna+bc+k]=0;}
	if(n==2){Csb[ina+bc+k][jna+bc+k]=0;}
	if(n==3){Csb[ina-bc-k][jna+bc+k]=0;}
	k=k+1;
	}
int hmze=0, fri[hm*hm], frj[hm*hm];
for (i = 0; i < hm; i += 1){
	for (j = 0; j < hm; j += 1){
		if(Csb[i][j] == 0) {fri[hmze]=i; frj[hmze]=j; hmze++;}
	}
}
// add 5 chips
for (i = 0; i < 5; i++){
		bc=random(0, hmze - 1); // take an empty cell
		int ic, jc;
		ic=fri[bc]; jc=frj[bc];
		Csb[ic][jc]=n5[i];
//change table of free cells last--> in place of bc and hmze--
		fri[bc]=fri[hmze-1]; frj[bc]=frj[hmze-1]; hmze = hmze - 1;
}
		} //if(hmb>4
		n=4; // was found > 4
	}
	else {if(ox5>0) n=4; else n++;}
}
if(ox5==1) e=4;
}
}//ValuX  end
void newdesk(){
int8_t i, j;
for (i = 0; i < hm; i += 1){
	for (j = 0; j < hm; j += 1){
		TC[i][j] = 0;
		Csb[i][j]=random(1, kcb);
	}
}
//empty emb cells
n=0;
while(n<emb){
	i=random(0, hm - 1);
	j=random(0, hm - 1);
	if(Csb[i][j]>0){Csb[i][j] = 0; n++;}
}
for (i = 0; i < 5; i++){
	n5[i]=random(1, kcb);
}
if(dbg==1) {printf("new\n"); dbgp();}
}
void vals(){
for (l = 0; l < rounds; l++) tb_mn[l]=0.0;
l=0;
}
void TTSR(){
	// INSERT HERE YOUR CODE OF A TEST TASKS SOLVER that
	// sets x0,y00 and x1,y01 i.e. SA&FA to achieve the best test results,
	// but don't change other global variables.
	
	//Next two lines are a solver based on the Monte Carlo method:
	//x0=random(0, hm-1); y00=random(0, hm-1);
	//x1=random(0, hm-1);y01=random(0, hm-1);
}

newdesk();

uint32_t tsg, tst;
for(tsg=0; tsg<tsts; tsg++){
	vals();	mn=0.0;
for(tst=0; tst<rounds; tst++){
while(qn<stp){
uu=0, n=0; ox5=0;
while(uu==0){//get the right move
 
TTSR(); //A TEST TASKS SOLVER that sets x0,y00 and x1,y01

if(Csb[x0][y00]>0&&Csb[x1][y01]==0){
//check accessibility FA from SA
for (int8_t aa = 0; aa < hm; aa += 1)
    for (int8_t bb = 0; bb < hm; bb += 1)TC[aa][bb] = 0;
fz(x0,y00);
if(TC[x1][y01]==1){uu=1;}
}
}
Csb[x1][y01]=Csb[x0][y00]; Csb[x0][y00]=0;
ValuX(Csb[x1][y01],x1,y01);
mn++; tst_mn++;
if(dbg==1) printf("%d %d %d %d %f\n",x0,y00,x1,y01,mn);
if(ox5>0)
{
	qn++;
	if(qn<stp) {
		n=0;
		while(n<5){n5[n]=random(1, kcb);n++;}
		}
}
if(dbg==1) dbgp();
}//here a round ends
qn=0; newdesk();
tb_mn[l]=tst_mn; l++;
tst_mn=0.0;
} //rounds

fwrite(tb_mn,rounds,sizd,f); // write rounds*sizd bytes from the buffer

tb_av[tsg]=mn/((float)(rounds*stp));

} //tsts
printf("%f", tb_av[0]);
for (uint32_t i = 1; i < tsts; i++){ printf(",%f", tb_av[i]);}
printf("\n");
fclose(f);
    return 0;
}
/*
An example using the Monte Carlo method:
./a.out 1 2 1 8 3 59 1
1 2 1 8 3 59 1
new
23222022
11122320
11122233
11201313
13133121
33223132
33032320
11222123
5 7 6 7 1.000000
23222022
11122320
11122233
11201313
13133121
33223130
33032322
11222123
5 6 5 7 2.000000
23222022
11122320
11122233
11201313
13133121
33223103
33032322
11222123
5 7 5 6 3.000000
23222022
11122320
11122233
11201313
13133121
33223130
33032322
11222123
3 4 3 3 4.000000
23222022
11122320
11122233
11210313
13133121
33223130
33032322
11222123
5 6 5 7 5.000000
23222022
11122320
11122233
11210313
13133121
33223103
33032322
11222123
5 2 6 2 6.000000
23222022
11122320
11122233
11210313
13133121
33023103
33232322
11222123
2 7 1 7 7.000000
23222022
11122323
11122230
11210313
13133121
33023103
33232322
11222123
5 5 5 6 8.000000
23222022
11122323
11122230
11210313
13133121
33023013
33232322
11222123
0 6 0 5 9.000000
23222202
11122323
11122230
11210313
13133121
33023013
33232322
11222123
4 4 3 4 10.000000
23222202
11122323
11122230
11213313
13130121
33023013
33232322
11222123
2 6 2 7 11.000000
23222202
11122323
11122203
11213313
13130121
33023013
33232322
11222123
5 3 5 2 12.000000
23222202
11122323
11122203
11213313
13130121
33203013
33232322
11222123
4 3 5 3 13.000000
23222202
11122323
11122203
11213313
13100121
33233013
33232322
11222123
3 3 4 3 14.000000
23222202
11122323
11122203
11203313
13110121
33233013
33232322
11222123
1 6 0 6 15.000000
23222222
11122303
11122203
11203313
13110121
33233013
33232322
11222123
new
32311020
33113330
31121233
32111132
22233013
32023312
21132311
23313222
1 6 1 7 16.000000
32311020
33113303
31121233
32111132
22233013
32023312
21132311
23313222
0 6 0 7 17.000000
32311002
33113303
31121233
32111132
22233013
32023312
21132311
23313222
0 7 1 6 18.000000
32311000
33113323
31121233
32111132
22233013
32023312
21132311
23313222
1 5 0 6 19.000000
32311030
33113023
31121233
32111132
22233013
32023312
21132311
23313222
0 6 1 5 20.000000
32311000
33113323
31121233
32111132
22233013
32023312
21132311
23313222
1 6 0 7 21.000000
32311002
33113303
31121233
32111132
22233013
32023312
21132311
23313222
1 7 0 6 22.000000
32311032
33113300
31121233
32111132
22233013
32023312
21132311
23313222
5 5 4 5 23.000000
32311032
33113300
31121233
32111132
22233313
32023012
21132311
23313222
new
21232311
11102301
11311332
31221131
31301022
21312112
31121211
20221312
11.500000

*/
