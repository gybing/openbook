#ifndef OpenBk_H
#define OpenBk_H

#include <vector>
#include <algorithm>

#include "board.h"

//����sf50���ֿ��Ҹ�ʽ����
 struct Entry {
    uint64_t key;
    uint16_t move;
    uint16_t count;
    uint32_t learn;
    bool operator == (const Entry& obj);
    bool operator >(const Entry& obj) const;  //����������Ҫ���صĲ�����
    bool operator <(const Entry& obj) const;  //����������Ҫ���صĲ�����
  };

class COpenBook : public CBoard
{
public:
	COpenBook(char *fileout="book.bin");
	~COpenBook();

    bool            DataClear(void);
    bool            PutInMove(void);
    bool            WriteToBook(char* fileout);
    static bool     merge_file(char* file_in_name1, char* file_in_name2, char* file_out_name);  //�ϲ���������ļ�
    static bool     copy_file(char* dst, char* src);        //�򵥵Ŀ����ļ����롣
    bool            del_repeat(char *file_in_name1);             //ɾ���ظ�����

    unsigned long       totals;              //�ܾ�������
    unsigned long       join;                //���Ӿ�����
    unsigned long       repeat;
    unsigned long       duicheng;

private:
    std::vector<Entry> book_e;
};

#endif
