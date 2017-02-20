#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Board.h"

// Random numbers from PolyGlot, used to compute book hash keys
extern const union {
              Key64 PolyGlotRandoms[3842];
              struct {
                       Key64 ulHashKey64[15][256];  //[PieceType][Square]
                       Key64 zobSideToMove;
                       Key64 zobExclusion;
                     } Zobrist;
} PG;

//#include "zobrist.cpp"

static const char ChessName[2][8][4]=
{ {"��","��","�e","��","��","��","��","��"},
  {"��","��","܇","�R","��","ʿ","��","��"}
};

static const char PostionName[2][9][4] =
{
	{"��","��","��","��","��","��","��","��","һ"},
	{"��","��","��","��","��","��","��","��","��"}
};

CBoard::CBoard()
{
	for (int i = 0; i < 15; i++)
          for (int j = 0; j < 256; j++)
          {
               m_ulHashKey64[i][j] = PG.Zobrist.ulHashKey64[i][j];
          }

        m_zobSideToMove ^= PG.Zobrist.zobSideToMove;
}

unsigned char CBoard::MakeMove(CChessMove* move)
{
    assert( move!=NULL );
    assert( ((move->m_From&0x0f) >= 0x3) && ((move->m_From&0x0f)<=0xb) );
    assert( ((move->m_To&0x0f) >= 0x3) && ((move->m_To&0x0f)<=0xb) );
    assert( ((move->m_From>>4)>=0x3) &&  ((move->m_From>>4)<=0xc) );
    assert( ((move->m_To>>4)>=0x3) &&  ((move->m_To>>4)<=0xc) );

    m_TotalSteps++;
    assert(m_TotalSteps<=MAXTOTALSTEPS);
    m_StepRecords[m_TotalSteps-1]=*move;

    unsigned char killid=  m_Position[move->m_To];

    //����
    m_Position[move->m_To] = m_Position[move->m_From];
    m_Position[move->m_From] = NOCHESS;
    m_Player = !m_Player;
    m_HashKey64 ^= m_zobSideToMove;

    //����HASH��
    if (killid != NOCHESS)
    {
        //move->m_KillID = killid;
        m_HashKey64 ^= m_ulHashKey64[15-killid][move->m_To];
    }
    m_HashKey64 ^= m_ulHashKey64[15-m_Position[move->m_To]][move->m_From];
    m_HashKey64 ^= m_ulHashKey64[15-m_Position[move->m_To]][move->m_To];

    return killid ;
}

void CBoard::UnMakeMove(CChessMove* move,unsigned char killid) //����ȥ���ڶ�������
{
    assert( move!=NULL );
    assert( ((move->m_From&0x0f) >= 0x3) && ((move->m_From&0x0f)<=0xb) );
    assert( ((move->m_To&0x0f) >= 0x3) && ((move->m_To&0x0f)<=0xb) );
    assert( ((move->m_From>>4)>=0x3) &&  ((move->m_From>>4)<=0xc) );
    assert( ((move->m_To>>4)>=0x3) &&  ((move->m_To>>4)<=0xc) );

    m_TotalSteps--;
    assert(m_TotalSteps<0xffffffff && m_TotalSteps<=MAXTOTALSTEPS);

    m_Position[move->m_From] = m_Position[move->m_To];
    m_Position[move->m_To]  =  killid ;

    m_Player  = !m_Player;
    m_HashKey64 ^= m_zobSideToMove;

    //����HASH��
    if (killid != NOCHESS)
    {
        m_HashKey64 ^= m_ulHashKey64[15-killid][move->m_To];
    }
    m_HashKey64 ^= m_ulHashKey64[15-m_Position[move->m_From]][move->m_To];
    m_HashKey64 ^= m_ulHashKey64[15-m_Position[move->m_From]][move->m_From];

}

Key64 CBoard::CalcIndexAndKey(unsigned char CurPosition[256])
{
	//m_HashKey32 = 0;
	Key64 key = 0;
	for (int i = 0; i < 256; i++)
	{
	        if((( i&0x0f)>=0x03)  && ( (i&0x0f)<=0x0b) && (((i&0xf0)>>4)>=0x03) && (((i&0xf0)>>4)<=0x0c) )
		{
                      if (CurPosition[i] != NOCHESS)
                      {
                              //m_HashKey32 = m_HashKey32 ^ m_nHashKey32[CurPosition[i]][i];
                              //int pt = CurPosition[i];
                              //pt= IsBlack(pt) ? (8-pt):(22-pt);
                              key = key ^ m_ulHashKey64[15-CurPosition[i]][i];

                      }
                }
	}

        if ( m_Player )
	        key ^= m_zobSideToMove;

        return key;
}

void CBoard::LeftToRight(unsigned char m_position[256])
{
    unsigned char temp[256];
    memset(temp,0,256*sizeof(unsigned char));
    int i,j ;
    for(i=0x33;i<=0xcb;i++)
    {
        temp[i&0xf0| (0x0c-(i&0x0f)+0x02)]=m_position[i];
    }
    memcpy(m_position,temp,256);
}

void CBoard::ChangeSide(unsigned char m_position[256])
{
    unsigned char temp[256];
    memset(temp,0,256*sizeof(unsigned char));
    int i,j ;
    for(i=0x33;i<=0xcb;i++)
     {
        temp[0xdc-i+0x22]=m_position[i];
        if(IsBlack(temp[0xdc-i ]))
        {
            temp[0xdc-i+0x22] += 7 ;
        }
        else if(IsRed(temp[0xdc-i]))
        {
            temp[0xdc-i+0x22] -= 7 ;
        }
     }
    memcpy(m_position,temp,256);
}

