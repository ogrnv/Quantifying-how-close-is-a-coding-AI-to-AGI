
'''
Please change TTSR function in the following subprogram TTSR.py to achieve the best test results i.e minimal number of moves in all tests of pv4i.py: python3 pv4i.py ...
See the definitions of moves, test... below in the pv4i.py program.

It is necessary to exclude the occurrence of any software loops, for example, when a chip moves along a closed path because of changing of the TTSR function.

pv4i.py:
'''
#!/usr/bin/env python3
import sys
import time
import struct
import random
import os

# constants
tmax = 5000
rmax = 100000
'''
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

all input is 7 parametres:
'''

tsts = 0	#the number of tests up to tmax
rounds = 0	#the number of rounds in 1 test up to rmax
stp = 0		#steps in 1 round
hm = 0		#size of a side of square board
kcb = 0		#how many of types of chips are placed on the board
nub = 0		#number of chips on the board
dbg = 0		#print debug info on console

emb = 0		#the number of empty sells on the board

Csb = [[0]*16 for _ in range(16)]	#TB
n5x = 0
n5 = [0]*5	#next five chips
TC = [[0]*16 for _ in range(16)]
ina = jna = hmb = iko = jko = kp = ox5 = 0
bc = ca = uu = n = 0

x0 = y00 = 0	#SA
x1 = y01 = 0	#FA
y2 = 0

icr = 0
qn = 0
l = im1 = im2 = med = 0
mn = 0.0
tb_mn = [0.0]*(tmax*rmax)
tst_mn = 0.0
tb_av = [0.0]*tmax
v = [0]*4
CT = [[0]*16 for _ in range(16)]
N5 = [0]*5
dat = [0]*16
import TTSR
def random_range(minv, maxv):#random_range(0, 8) gives {0...8}
    return random.randint(minv, maxv)

def dbgp():
    for i in range(hm):
        for j in range(hm):
            print(Csb[i][j], end='')
        print()

def fz(fzx, fzy):
    if (fzx>0) and (Csb[fzx-1][fzy]==0) and (TC[fzx-1][fzy]==0):
        TC[fzx-1][fzy]=1
        fz(fzx-1, fzy)
    if (fzx<hm-1) and (Csb[fzx+1][fzy]==0) and (TC[fzx+1][fzy]==0):
        TC[fzx+1][fzy]=1
        fz(fzx+1, fzy)
    if (fzy>0) and (Csb[fzx][fzy-1]==0) and (TC[fzx][fzy-1]==0):
        TC[fzx][fzy-1]=1
        fz(fzx, fzy-1)
    if (fzy<hm-1) and (Csb[fzx][fzy+1]==0) and (TC[fzx][fzy+1]==0):
        TC[fzx][fzy+1]=1
        fz(fzx, fzy+1)

def ver(colr, i, j):
    global ina, jna, iko, hmb, ox5
    ina = i; jna = j
    while (ina-1) >= 0:
        ina -= 1
        if colr != Csb[ina][j]:
            ina += 1
            break
    iko = i
    while (iko+1) < hm:
        iko += 1
        if colr != Csb[iko][j]:
            iko -= 1
            break
    hmb = iko - ina + 1
    if hmb > 4:
        ox5 = 1

def hor(colr, i, j):
    global ina, jna, jko, hmb, ox5
    jna = j; ina = i
    while (jna-1) >= 0:
        jna -= 1
        if colr != Csb[i][jna]:
            jna += 1
            break
    jko = j
    while (jko+1) < hm:
        jko += 1
        if colr != Csb[i][jko]:
            jko -= 1
            break
    hmb = jko - jna + 1
    if hmb > 4:
        ox5 = 1

def nak(colr, i, j):
    global ina, jna, iko, jko, hmb, ox5
    ina = i; jna = j
    while (ina-1) >= 0 and (jna-1) >= 0:
        ina -= 1; jna -= 1
        if colr != Csb[ina][jna]:
            ina += 1; jna += 1
            break
    iko = i; jko = j
    while (iko+1) < hm and (jko+1) < hm:
        iko += 1; jko += 1
        if colr != Csb[iko][jko]:
            iko -= 1; jko -= 1
            break
    hmb = jko - jna + 1
    if hmb > 4:
        ox5 = 1

def nkl(colr, i, j):
    global ina, jna, iko, jko, hmb, ox5
    ina = i; jna = j
    while (ina+1) < hm and (jna-1) >= 0:
        ina += 1; jna -= 1
        if colr != Csb[ina][jna]:
            ina -= 1; jna += 1
            break
    iko = i; jko = j
    while (iko-1) >= 0 and (jko+1) < hm:
        iko -= 1; jko += 1
        if colr != Csb[iko][jko]:
            iko += 1; jko -= 1
            break
    hmb = ina - iko + 1
    if hmb > 4:
        ox5 = 1

def ValuX(colr, i, j):
    global ox5, qn, dbg
    global ina, jna, iko, jko, hmb, bc
    ox5 = 0
    # trying to find a line in random order
    tt = [-1]*4
    e = 0
    while e < 4:
        an = random.getrandbits(32)
        n = 0
        while n < 16:
            b = an & 3
            if tt[b] == -1:
                tt[b] = e
                e += 1
                if e == 4:
                    break
            an = an >> 2
            n += 1
    for e in range(4):
        b = 1
        n = 0
        while n < 4:
            if tt[n] == e:
                if n == 0:
                    ver(colr, i, j)
                elif n == 1:
                    hor(colr, i, j)
                elif n == 2:
                    nak(colr, i, j)
                elif n == 3:
                    nkl(colr, i, j)
            if ox5 > 0 and (qn+1) < stp:
                if hmb > 4:
                    if hmb == 5:
                        bc = 0
                    else:
                        bc = random_range(0, hmb - 5)
                    if dbg == 1:
                        print(f"the number {{0,1,2,3}} of del type | - \\ / is {n}\nbefore del:")
                        dbgp()
                        print()
                    k = 0
                    while k < 5:
                        if n == 0:
                            Csb[ina+bc+k][jna] = 0
                        if n == 1:
                            Csb[ina][jna+bc+k] = 0
                        if n == 2:
                            Csb[ina+bc+k][jna+bc+k] = 0
                        if n == 3:
                            Csb[ina-bc-k][jna+bc+k] = 0
                        k += 1
                    hmze = 0
                    fri = [0]*(hm*hm)
                    frj = [0]*(hm*hm)
                    for ii in range(hm):
                        for jj in range(hm):
                            if Csb[ii][jj] == 0:
                                fri[hmze] = ii
                                frj[hmze] = jj
                                hmze += 1
                    for ii in range(5):
                        bc = random_range(0, hmze - 1)
                        ic = fri[bc]; jc = frj[bc]
                        Csb[ic][jc] = n5[ii]
                        fri[bc] = fri[hmze-1]; frj[bc] = frj[hmze-1]; hmze -= 1
                n = 4
            else:
                if ox5 > 0:
                    n = 4
                else:
                    n += 1
        if ox5 == 1:
            break

def newdesk():
    global Csb, TC, n5, emb, dbg
    for i in range(hm):
        for j in range(hm):
            TC[i][j] = 0
            Csb[i][j] = random_range(1, kcb)
    n = 0
    while n < emb:
        i = random_range(0, hm-1)
        j = random_range(0, hm-1)
        if Csb[i][j] > 0:
            Csb[i][j] = 0
            n += 1
    for i in range(5):
        n5[i] = random_range(1, kcb)
    if dbg == 1:
        print("new")
        dbgp()

def vals():
    global tb_mn, l
    for ll in range(rounds):
        tb_mn[ll] = 0.0
    l = 0

def main():
    global tsts, rounds, stp, hm, kcb, nub, dbg, emb
    global dat, Csb, N5, CT, v
    global mn, tst_mn, tb_av, tb_mn, l, qn

    if len(sys.argv) < 8:
        print(f"Error: must be 7 arguments, not {len(sys.argv)-1}")
        return 1
    tsts = int(sys.argv[1]); rounds = int(sys.argv[2])
    stp = int(sys.argv[3]); hm = int(sys.argv[4])
    kcb = int(sys.argv[5]); nub = int(sys.argv[6])
    dbg = int(sys.argv[7])
    dat[0]=tsts;
    dat[1]=rounds;
    dat[2]=stp; 
    dat[3]=hm;
    dat[4]=kcb;
    dat[5]=nub;
    dat[6]=dbg

    print(tsts, rounds, stp, hm, kcb, nub, dbg)

    emb = hm*hm - nub

    seconds = int(time.time())
    nmf = f"raw_{tsts}x{rounds}_{stp}-{hm}x{hm}-{kcb}-{nub}_{seconds}"
    f = open(nmf, "wb")
    sizd = struct.calcsize("f")

    newdesk()
    for tsg in range(tsts):
        vals()
        mn = 0.0
        for tst in range(rounds):
            while qn < stp:
                # get a right move
                global uu
                uu = 0
                while uu == 0:
                    dat[7] = tsg	#finished tests from tsts
                    dat[8] = tst	#finished rounds in a test
                    dat[9] = qn		#finished steps in a round
                    dat[10] = mn	#moves made in a test
                    dat[11] = tst_mn	#moves made in a round	
                    for i in range(hm):
                        for j in range(hm):
                            CT[i][j] = Csb[i][j]
                    for i in range(5):
                        N5[i] = n5[i]
                    TTSR.TTSR(v, dat, CT, N5)
#See TTSR below in TTSR.py
#TTSR IS A TEST TASKS SOLVER for setting x0,y00 and x1,y01 i.e. SA&FA
                    x0 = v[0]; y00 = v[1]; x1 = v[2]; y01 = v[3]
                    #print(x0, y00, x1, y01, mn)
                    if Csb[x0][y00] > 0 and Csb[x1][y01] == 0:
                        for aa in range(hm):
                            for bb in range(hm):
                                TC[aa][bb] = 0
                        fz(x0, y00)
                        if TC[x1][y01] == 1:
                            uu = 1
                Csb[x1][y01] = Csb[x0][y00]
                Csb[x0][y00] = 0
                ValuX(Csb[x1][y01], x1, y01)
                mn += 1.0
                tst_mn += 1.0
                if dbg == 1:
                    print(x0, y00, x1, y01, mn, n5)
                if ox5 > 0:
                    qn += 1
                    if qn < stp:
                        for idx in range(5):
                            n5[idx] = random_range(1, kcb)
                if dbg == 1:
                    dbgp()
            qn = 0
            newdesk()
            tb_mn[l] = tst_mn
            l += 1
            tst_mn = 0.0
        # write tb_mn as floats (rounds floats)
        # pack and write
        data_bytes = b''.join(struct.pack("f", tb_mn[i]) for i in range(rounds))
        f.write(data_bytes)
        tb_av[tsg] = mn / (float(rounds*stp))
    print(tb_av[0], end='')
    for i in range(1, tsts):
        print(f",{tb_av[i]}", end='')
    print()
    f.close()
    return 0

if __name__ == "__main__":
    main()
'''
# TTSR.py
import random
def TTSR(v, dat, CT, N5):
	#INSERT HERE YOUR CODE OF A TEST TASKS SOLVER that
	#sets v  i.e. x0,y00,x1,y01 to achieve the best test results	
	#Next five lines are a solver based on the Monte Carlo method:
    hml = dat[3]    
    v[0] = random.randrange(0, hml) #random.randrange(0, 8) gives {0...7}
    v[1] = random.randrange(0, hml)
    v[2] = random.randrange(0, hml)
    v[3] = random.randrange(0, hml)	
    #print(v[0], v[1], v[2], v[3], hml)
'''
