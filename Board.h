#ifndef BOARD_H
#define BOARD_H

typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned __int64 Key64;

const unsigned char NOCHESS     = 0 ;
const unsigned char B_KING      = 1;  //��˧        15-  B_KING = 14
const unsigned char B_ROOK      = 2;  //�ڳ�                      13
const unsigned char B_HORSE     = 3;  //����                      12
const unsigned char B_CANNON     = 4;  //����                     11
const unsigned char B_BISHOP    = 5 ; //��ʿ                      10
const unsigned char B_ELEPHANT  = 6 ; //����                      9
const unsigned char B_PAWN      = 7 ; //����                      8

const unsigned char R_KING      = 8;   //��˧         15- R_KING =7
const unsigned char R_ROOK       = 9;   //�쳵                    6
const unsigned char R_HORSE      =10;  //����                     5
const unsigned char R_CANNON    =  11;  //����                    4
const unsigned char R_BISHOP    = 12;  //��ʿ                     3
const unsigned char R_ELEPHANT  = 13;  //����                     2
const unsigned char R_PAWN      = 14;  //���                     1

#define IsBlack(x)   (x>=B_KING&&x<=B_PAWN)
#define IsRed(x)     (x>=R_KING&&x<=R_PAWN)

//�ŷ��࣬������һ���ŷ�����ַ����ַ������
struct CChessMove
{
    unsigned char       m_From;            //��ַ����16*16��ʾ����
    unsigned char       m_To;              //��ַ����16*16��ʾ����
   // unsigned char       m_KillID;          //���ӣ��� enum Chessman ��ʾ��
};

class CBoard
{
const static int MAXTOTALSTEPS      = 500;

public:
    CBoard();

    unsigned char       MakeMove(CChessMove* move);
    void                UnMakeMove(CChessMove* move,unsigned char killid);
    void                ChangeSide(unsigned char m_position[256]);
    void                LeftToRight(unsigned char m_position[256]);
    Key64               CalcIndexAndKey(unsigned char CurPosition[256]);

    unsigned char       m_Position[256];
    bool                m_Player;
    unsigned int        m_TotalSteps;
    CChessMove          m_StepRecords[MAXTOTALSTEPS];
    Key64               m_HashKey64;

private:
    void        Print(void);

    Key64       m_zobSideToMove;
    Key64       m_ulHashKey64[15][256];
};


#endif
