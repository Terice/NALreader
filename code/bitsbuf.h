#ifndef BITSBUF_H_
#define BITSBUF_H_

#include<iostream>
#include<stdio.h>
using std::endl;
using std::cout;
#define MAXBUFSIZE 32
typedef unsigned char uchar;

/*

*/
class Bitsbuf
{
private:
    uchar* buf_data;
    uchar buf1_data[MAXBUFSIZE];
    uchar* buf2_data;
    uint8_t state = 0;//防止竞争状态
    bool buf_state = 0;//0-empty 1-full
    bool buf2_state = 0;//0-empty 1-full
    int buf_index = 0;//the first bit
    uchar buf_errmsg;
    FILE* datares;
    void bseti(char indextoset);
    

    void bufin(FILE* fp);//take bitstream into buf
    void bufin(uchar* ch);
    void bufin();
    bool bempt();
public:

    Bitsbuf();
    Bitsbuf(FILE* datares);
    ~Bitsbuf();
    
    bool bread();
    bool balgi();
    uint64_t bread_n(uchar size);
    uint64_t bnext(uint32_t nextsize);

    int64_t  bread_se();//signed golomb
    uint64_t bread_ue();//unsigned golomb
    uint64_t bread_me(uint16_t ChromaArrayType, uint32_t mb_type);//映射指数
    uint64_t bread_te(uint32_t range);//截断指数

    uint64_t bread_ce();//上下文自适应的二进制算数熵编码 CAVLC
    uint64_t bread_ae();//上下文自适应的可变长熵编码    CABAC
    
    int64_t  bread_in(uint16_t n);
    uint64_t bread_un(uint16_t n);
    uint64_t bread_fn(uint16_t n);
    

    void bfrsh();

    uchar berror();
};





#endif