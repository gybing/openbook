//---------------------------------------------------------------------------

#include <vcl.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

//#include <winbase.h>
#pragma hdrstop

#include "BookForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CBLibXControl_OCX"
#pragma resource "*.dfm"

Tfrmbook *frmbook;
//---------------------------------------------------------------------------
__fastcall Tfrmbook::Tfrmbook(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall Tfrmbook::FormCreate(TObject *Sender)
{
        iCountCBL=0;
        iCountQiPu=0;
        iCountJuMian=0;
        iCalced = 66;
        curNode=NULL;
        CBLibX1->Visible=false;
        this->Caption="��������Ȥ���й��������濪�ֿ����ɹ��� by LiuJianhua";
        OpenDialog1->InitialDir = ExtractFilePath(Application->ExeName);
        OpenCBL("������Ȥ.cbl");
}
//---------------------------------------------------------------------------

void __fastcall Tfrmbook::btnMakeClick(TObject *Sender)
{

      if ( chkdel->Checked )
      {
           ob.DataClear();
      }

      Memo1->Lines->Add("\n����ת�����벻Ҫ���������������Ժ�......\n");
      iCountCBL=0;
      iCountQiPu=0;
      iCountJuMian=0;

      ob.totals =0;
      ob.duicheng=0;
      ob.repeat=0;
      ob.join=0;

      TSearchRec sr;
      int iAttributes = 0;
      iAttributes |= faAnyFile ;
      if (FindFirst(edtSource->Text, iAttributes, sr) == 0)
      {
          do
          {
                if ((sr.Attr & iAttributes) == sr.Attr)
                {

                      iCountCBL++;
                      OpenCBL(sr.Name.c_str()) ;
                      //Sleep(3000);
                      for(int i = 0;i< CBLibX1->RecordCount ;i++)
                      {
                           QiPuInfo(i);   //include create tree;

                           iCountQiPu++;

                           if(treeView1->Items->Count==0||CBLibX1->R_CGType==2||CBLibX1->R_CGType ==3)
                           {
                                continue;
                           }

                           curNode= treeView1->Items->GetFirstNode();
                           if(curNode!=NULL)
                           {
                                 do
                                 {
                                      if( curNode->Parent==NULL)
                                      {
                                            ob.m_HashKey64=ob.CalcIndexAndKey(ob.m_Position);
                                            ScanNode(curNode);            //ת��
                                      }
                                      curNode=curNode->GetNext();
                                 }while(curNode!=NULL);
                           }

                           ListBox1->ItemIndex = i;
                           frmbook->Update();
                           drawiconboard(ob.m_Position);
                           //Sleep(100);
                      }
                }
          } while (FindNext(sr) == 0);
          FindClose(sr);
      }

      char ibuffer[100];
      Memo1->Lines->Clear();
      Memo1->Lines->Add("");
      Memo1->Lines->Add("\nת�����,ת��������£�\n");
      Memo1->Lines->Add("");

      ob.WriteToBook(edtDest->Text.c_str());            //д���ļ��С�


      vsprintf(ibuffer, "ת�� %u ������ļ�",&iCountCBL );
      Memo1->Lines->Add(ibuffer);

      vsprintf(ibuffer, "ת�� %u ������",&iCountQiPu );
      Memo1->Lines->Add(ibuffer);

      vsprintf(ibuffer, "���ι�����ľ�������: %u �����棬���У�",&iCountJuMian);
      Memo1->Lines->Add(ibuffer);

      vsprintf(ibuffer, "1�����뿪�ֿ�ľ�������: %u ��",&ob.join);
      Memo1->Lines->Add(ibuffer);

      vsprintf(ibuffer, "2���Գƾ�������: %u ��",&ob.duicheng);
      Memo1->Lines->Add(ibuffer);

      vsprintf(ibuffer, "3���ظ��ľ�������: %u ��",&ob.repeat);
      Memo1->Lines->Add(ibuffer);

      //CBLibX1->AboutBox();
}

//---------------------------------------------------------------------------
void __fastcall Tfrmbook::btnLibClick(TObject *Sender)
{
    if (!OpenDialog1->Execute())
    {
        return;
    }
    edtSource->Text =  OpenDialog1->FileName;
    OpenCBL(OpenDialog1->FileName.c_str() ) ;
}

//---------------------------------------------------------------------------
void __fastcall Tfrmbook::ListBox1DblClick(TObject *Sender)
{
      Memo1->Clear();
      QiPuInfo(ListBox1->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall Tfrmbook::FormDestroy(TObject *Sender)
{
      if (CBLibX1->Active)
              CBLibX1->Close();
}
//---------------------------------------------------------------------------


bool Tfrmbook::ScanNode(TTreeNode *node)
{
       TTreeNode *cur;
       if(node==NULL )
                return false;

       AnsiString temp(node->Level+1) ;
       temp=temp+node->Text;
//       Memo1->Lines->Add( temp );

        iCountJuMian++;
        char * buffer;
        unsigned char from,to;

        ob.m_TotalSteps = (node->Text.SubString(0,5)).ToInt() ;
        buffer = (node->Text.SubString(6,4)).c_str();

        from = ((buffer[1]<<4)+0x30)|((buffer[0]&0xf)+ 0x03)  ;
        to = ((buffer[3]<<4)+0x30)|((buffer[2]&0xf)+ 0x03)    ;
        ob.m_StepRecords[ob.m_TotalSteps].m_From=from;
        ob.m_StepRecords[ob.m_TotalSteps].m_To=to;

        ob.PutInMove();


       cur = node->getFirstChild();
       if(cur==NULL)
                return false;
       do
       {
           unsigned char killid = ob.MakeMove(&ob.m_StepRecords[ob.m_TotalSteps]);
           ScanNode(cur);             //�ݹ����
           ob.UnMakeMove(&ob.m_StepRecords[ob.m_TotalSteps-1],killid);           //ԭ���Ǵ˴��ټ�1���Ҳ٣�
           cur = node->GetNextChild(cur);
           Application->ProcessMessages();
       } while(cur!=NULL);

       return true;
}

//---------------------------------------------------------------------------
void Tfrmbook::OpenCBL(char * filename)
{
        if (CBLibX1->Active)
        {
            CBLibX1->Close();
        }

        CBLibX1->LibName= filename;
        CBLibX1->Open();

        if(CBLibX1->Exists)
        {
                DisplayCBL();
        }
}

void Tfrmbook::QiPuInfo(int qipuno)
{

        CBLibX1->NeedBody=true;         //Ҫ��ȡ��������
        CBLibX1->RecNo=qipuno+1;        //ָ������
//
        Memo1->Clear();
        DisplayQiPu();

        SetOleStrings( mmo1->Lines ,(Stdvcl::_di_IStrings )CBLibX1->MoveList );
        AnsiString movelist = mmo1->Lines->Text;

       curNode= treeView1->Items->GetFirstNode();
       if(curNode!=NULL)
       {
             do
             {
                  if( curNode->Parent==NULL)
                  {
                        treeView1->Items->Delete(curNode);
                  }
                  curNode= treeView1->Items->GetFirstNode();
             }while(curNode!=NULL);
       }

        QiPu2Tree(WideCharToString(CBLibX1->R_Layout).c_str(),   CBLibX1->R_RedFirst, movelist);

        PrintBoard();
        drawiconboard(ob.m_Position);
         
}


bool Tfrmbook::QiPu2Tree(char * qipan ,bool redfirst, AnsiString qipu)
{
//        ��ʾ�����ģ�  0919293949596979891777062646668600102030405060708012720323436383
        //char* pchar="0919293949596979891777062646668600102030405060708012720323436383";
        //char * outfile = "book.bin";
        unsigned char ChessManValue[32]={
                R_ROOK,R_HORSE,R_ELEPHANT,R_BISHOP,R_KING,R_BISHOP,R_ELEPHANT,R_HORSE,R_ROOK,
                R_CANNON,R_CANNON,R_PAWN,R_PAWN,R_PAWN,R_PAWN,R_PAWN,
                B_ROOK,B_HORSE,B_ELEPHANT,B_BISHOP,B_KING,B_BISHOP,B_ELEPHANT,B_HORSE,B_ROOK,
                B_CANNON,B_CANNON,B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN };
        int join=0;
        unsigned char from,to;
        unsigned char x1,y1,x2,y2,x,y;
        AnsiString strline=qipu,step,stepname;
        int istart=0;
        int lenqipu=qipu.Length();
        unsigned char * buffer = NULL ;

         curNode= treeView1->Items->GetFirstNode();
         if(curNode!=NULL)
         {
               do
               {
                    if( curNode->Parent==NULL)
                    {
                          treeView1->Items->Delete(curNode);
                    }
                    curNode= treeView1->Items->GetFirstNode();
                    Application->ProcessMessages();
               }while(curNode!=NULL);
         }

        //set ob and treeView1
        //unsigned char board[256];
        memset(ob.m_Position,0,256);             //�߽��޸�Ϊ0xff;
        unsigned char *ucQiPan=NULL;
        ucQiPan = (unsigned char*)qipan;
       int i;
       for( i =0; i<64; i+=2)
       {
            if(ucQiPan[i]=='n'||ucQiPan[i+1]=='n')continue;
            x=ucQiPan[i];
            x=(x&0x0f) + 0x03;
            y=  ucQiPan[i+1];
            y=(y<<4)+0x30;
            ob.m_Position[(y|x)] = ChessManValue[i>>1];
       }
       ob.m_Player = !CBLibX1->R_RedFirst;
       ob.m_TotalSteps= CBLibX1->R_Start;

       buffer = (qipu.SubString(6,4)).c_str();
       from = ((buffer[1]<<4)+0x30)|((buffer[0]&0xf)+ 0x03)    ;
       to = ((buffer[3]<<4)+0x30)|((buffer[2]&0xf)+ 0x03)    ;
       ob.m_StepRecords[ob.m_TotalSteps].m_From =from;
       ob.m_StepRecords[ob.m_TotalSteps].m_To =to;

       //TTreeNode *pNode=NULL ;
       curNode =NULL;
       do
        {
              strline=strline.SubString(istart,lenqipu);  //
              istart= strline.Pos("\r\n")+2;
              step=  strline.SubString(0,9);
              buffer = step.c_str();

              if(istart<9||strline=="\r\n")
              {
                  break;
              }
              else
              {
                      join = (strline.SubString(0,9)).SubString(0,5).ToInt() ;
                      from = ((buffer[6]<<4)+0x30)|((buffer[5]&0xf)+ 0x03)    ;
                      to = ((buffer[8]<<4)+0x30)|((buffer[7]&0xf)+ 0x03)    ;
                      stepname =strline.SubString(10,8);

                      if(curNode==NULL||join==1)
                      {
                            curNode= treeView1->Items->Add(NULL,  (strline.SubString(0,17)).c_str());
                      }
                      else  if(join<= iCalced)
                      {
                            if(curNode->Level==join-2)
                            {
                                      curNode= treeView1->Items->AddChild(curNode, (strline.SubString(0,17)).c_str());
                            }
                            else
                            {
                                      TTreeNode * tmp;
                                      tmp=curNode;
                                      while(tmp!=NULL&&tmp->Parent!=NULL)
                                      {
                                              if(tmp->Parent->Level==join-2 && join)
                                              {
                                                   curNode = treeView1->Items->AddChild(tmp->Parent, (strline.SubString(0,17)).c_str());
                                              }
                                              tmp=tmp->Parent;
                                              Application->ProcessMessages();
                                      };
                            }
                      }
                      //treeView1->FullExpand();
              }
              Application->ProcessMessages();
        }while(strline!="\r\n");

        return true;
}


void Tfrmbook::PrintBoard(void)
{
      //���沿���ǶԳ�ʼͼ�ĵı任�����ַ���ת������ά����
      int qp[10][9];
      int i,j,x,y;
      char s[200];

      for( i=0 ;i<10;i++ )
        for( j=0 ;j< 9;j++)
            qp[i][j]=99;

      for (i=0 ;i< 32;i++)
      {
            x=CBLibX1->R_Layout[i*2+1];
            x=x-48;

            if ((x>=0) && (x<9) )
            {

              y=CBLibX1->R_Layout[i*2+2]-48;
              qp[y][x]=i;
            }
      }

}
void __fastcall Tfrmbook::Button1Click(TObject *Sender)
{
        ListBox1->Clear();              //�����Ϣ
        DisplayCBL();
}
//---------------------------------------------------------------------------

void __fastcall Tfrmbook::Button2Click(TObject *Sender)
{
        Memo1->Clear();
        DisplayQiPu();
}
//---------------------------------------------------------------------------


void Tfrmbook::DisplayCBL(void)
{
        Memo1->Clear();
        ListBox1->Clear();
        if (!CBLibX1->Active)
        {
              Memo1->Lines->Add("����ѡ����⣡");
              return;
        }
        Memo1->Lines->Add("-----------------------------------");
        Memo1->Lines->Add(CBLibX1->H_Name+"�Ѿ��򿪣�");
        Memo1->Lines->Add("����ļ���:"+CBLibX1->LibName);
        Memo1->Lines->Add("�����:"+CBLibX1->H_Name);
        Memo1->Lines->Add("�����Դ:"+CBLibX1->H_URL);
        Memo1->Lines->Add("������:"+CBLibX1->H_Creator);
        Memo1->Lines->Add("������Email:"+CBLibX1->H_CreatorEmail);
        Memo1->Lines->Add("�������ڣ�"+CBLibX1->H_CreateDate);
        Memo1->Lines->Add("��������:"+CBLibX1->H_LastUpdate);
        Memo1->Lines->Add("��������:"+IntToStr(CBLibX1->H_Count));
        Memo1->Lines->Add("���˵��:"+CBLibX1->H_Explanation);
        Memo1->Lines->Add("-----------------------------------");
        Memo1->Lines->Add("");

        CBLibX1->NeedBody=false;
        for (int i=1 ; i<=CBLibX1->RecordCount;i++)
        {
            CBLibX1->RecNo=i;
            ListBox1->Items->Add(IntToStr(i)+"-> "+CBLibX1->R_Name);
        }
}

void Tfrmbook::DisplayQiPu(void)
{
        char cgt[4][50];
        char jg[4][10];


        if (!CBLibX1->Active)
        {
              Memo1->Lines->Add("����ѡ����⣡");
              return;
        }

        strcpy( &cgt[0][0],"ʵս��ȫ��");
        strcpy( &cgt[1][0],"���׵�ȫ��");
        strcpy( &cgt[2][0],"ʵս�Ĳо�");
        strcpy( &cgt[3][0],"���׵Ĳо�");
        strcpy( &jg[0][0],"��ʤ");
        strcpy( &jg[1][0],"��ʤ");
        strcpy( &jg[2][0],"�;�");
        strcpy( &jg[3][0],"δ֪");
        char buffer[200]={""};
        char str[5];

        strcat(buffer, "R_Layout��");
        strcat(buffer, WideCharToString(CBLibX1->R_Layout).c_str() );
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "������:");
        strcat(buffer, WideCharToString(CBLibX1->R_Name).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�����Դ��");
        strcat(buffer, WideCharToString(CBLibX1->R_URL).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "������ͣ�");
        strcat(buffer, cgt[CBLibX1->R_CGType]);
//        strcat(buffer, itoa(CBLibX1->R_CGType,str,10));
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "��ʼ�غϣ�");
        strcat(buffer, itoa(CBLibX1->R_Start,str,10));
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�������ƣ�");
        strcat(buffer, WideCharToString(CBLibX1->R_MatchName).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�����ص㣺");
        strcat(buffer, WideCharToString(CBLibX1->R_MatchSite).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�������ڣ�");
        strcat(buffer, WideCharToString(CBLibX1->R_MatchDate).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�췽���֣�");
        strcat(buffer, WideCharToString(CBLibX1->R_Red).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�ڷ����֣�");
        strcat(buffer, WideCharToString(CBLibX1->R_Black).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�����");
        strcat(buffer, jg[CBLibX1->R_Result]);
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "�����ˣ�");
        strcat(buffer, WideCharToString(CBLibX1->R_Commentator).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "������Email��");
        strcat(buffer, WideCharToString(CBLibX1->R_CommentatorEmail).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "¼���ˣ�");
        strcat(buffer, WideCharToString(CBLibX1->R_Author).c_str());
        Memo1->Lines->Add(buffer);

        strcpy(buffer,"");
        strcat(buffer, "¼����Email��");
        strcat(buffer, WideCharToString(CBLibX1->R_AuthorEmail).c_str());
        Memo1->Lines->Add(buffer);


        if( CBLibX1->R_RedFirst )
            Memo1->Lines->Add("�췽���У���") ;
        else
            Memo1->Lines->Add("�췽���У���");

        Memo1->Lines->Add("�������ڣ�"+CBLibX1->R_CreateDate);
        Memo1->Lines->Add("�������ڣ�"+CBLibX1->R_LastUpdate);
        Memo1->Lines->Add("����˵��:"+CBLibX1->R_Explanation);
//        Memo1->Lines->Add("��ʼ��ͼ���ұ�");
//        Memo1->Lines->Add("�����г�����ȫ��������(��������)��");
}
void __fastcall Tfrmbook::btn1Click(TObject *Sender)
{
        treeView1->FullExpand();        
}
//---------------------------------------------------------------------------

void __fastcall Tfrmbook::Button3Click(TObject *Sender)
{
        treeView1->FullCollapse();        
}
//---------------------------------------------------------------------------

void __fastcall Tfrmbook::edtCalcedChange(TObject *Sender)
{
       iCalced=  edtCalced->Text.ToInt();
}
//---------------------------------------------------------------------------

void    Tfrmbook::drawiconboard(unsigned char cposition[256])
{
        unsigned char position[256];
        POINT pt1,pt2;
        int dx,dy;

        memcpy(position,cposition,256);

        dx=18;
        dy=18;

        pt1.x=imgboard->Left+3;
        pt1.y=imgboard->Top+33;

        TCanvas * MyCanvas=new TCanvas();
        HDC MyDC = GetWindowDC(this->Handle);
        MyCanvas->Handle = MyDC;

        frmbook->Refresh();
        for(int i =0;i<256; i++)
        {
             //if(position[i]!= NOCHESS && position[i] >= 0 && position[i] <= 14 )
             if((( i&0x0f)>=0x03)  && ( (i&0x0f)<=0x0b) && (((i&0xf0)>>4)>=0x03) && (((i&0xf0)>>4)<=0x0c) )
             {
                il1->Draw(MyCanvas,pt1.x+ ((i&0x0f)-3) *dx,pt1.y+ (((i&0xf0)>>4)-3)*dy,position[i], true);
             }
        }

      ReleaseDC(0,MyDC);
      MyCanvas->Handle = 0;
      delete MyCanvas;
}


void __fastcall Tfrmbook::chkdelClick(TObject *Sender)
{
      if(chkdel->Checked)
      {
            Memo1->Lines->Add("�����µĿ��ֿ�ǰ ��� ԭ�������ݣ�"+ edtDest->Text);
            Memo1->Lines->Add("�����˳�ǰ�����ԭʼ���ֿ��ļ��е����ݣ���ע�Ᵽ�渱����"+ edtDest->Text);
      }
      else
      {
            Memo1->Lines->Add("�����µĿ��ֿ�ǰ ���� ԭ�������ݣ�"+ edtDest->Text);
      }

}
//---------------------------------------------------------------------------

void __fastcall Tfrmbook::btnMergeClick(TObject *Sender)
{
      char ibuffer[100];
      Memo1->Lines->Clear();
      //Memo1->Lines->Add("");
      Memo1->Lines->Add("\n��ʼ�ϲ����ֿ��ļ�....................\n");
      //Memo1->Lines->Add("");

      if( (_access("�ϲ�ȥ��.bin", 0 )) != -1 )
      {
              remove( "�ϲ�ȥ��.bin");
               Memo1->Lines->Add("\n������ļ� ���ϲ�ȥ��.bin�� ��ɾ����\n");
      }

       bool dstOK=true;
        if( (_access(edtDest->Text.c_str(), 0 )) == -1 )
        {
            dstOK=false;
        }
        else
        {
                if(chkdel->Checked)
                {
                        Memo1->Lines->Add("\n������ļ� book.bin ! ����ɾ����\n");
                        Memo1->Lines->Add("\nɾ���� book.bin! \n");
                        remove(edtDest->Text.c_str());
                        dstOK=false;
                }
        }

      TSearchRec sr;
      int iAttributes = 0;
      iAttributes |= faAnyFile ;
      //AnsiString fn = OpenDialog1->InitialDir+"\\*.bin";

      if (FindFirst("*.bin", iAttributes, sr) == 0)
      {
          do
          {
                if ((sr.Attr & iAttributes) == sr.Attr && strcmp(sr.Name.c_str(),"book.bin")!=0)
                {
                      if(!dstOK)
                      {
                                COpenBook::copy_file(edtDest->Text.c_str(), sr.Name.c_str());
                                Memo1->Lines->Add("δ�ҵ�book.bin!");
                                Memo1->Lines->Add(sr.Name.c_str());
                                Memo1->Lines->Add("\n���ϵ�1���ļ�ֱ�Ӹ���Ϊbook.bin\n");
                                Memo1->Lines->Add("\n���¼����ϲ�\n\n");
                                dstOK =! dstOK;
                      }
                      else
                      {
                                if(COpenBook::merge_file(sr.Name.c_str(), edtDest->Text.c_str(),"book_merged.tmp"))
                                {
                                        Memo1->Lines->Add(sr.Name.c_str());
                                        COpenBook::copy_file(edtDest->Text.c_str(), "book_merged.tmp");
                                }

                      }
                }
          } while (FindNext(sr) == 0);
          FindClose(sr);
      }

      int sum = ob.del_repeat("book_merged.tmp");
      remove( "book.bin");
      remove( "book_merged.tmp");

      Memo1->Lines->Add("\n");
      Memo1->Lines->Add("\n\n\n�ϲ���ɣ�����������");
      Memo1->Lines->Add(ob.totals-1);
      Memo1->Lines->Add("\n�ظ����棺");
      Memo1->Lines->Add(ob.repeat-1);
      Memo1->Lines->Add("\n�ϲ�����¿��ֿ��������Ϊ��");
      Memo1->Lines->Add(ob.join);
}
//---------------------------------------------------------------------------

