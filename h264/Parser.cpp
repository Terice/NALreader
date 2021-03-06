#include "Parser.h"
#include "cabac.h"
#include "Debug.h"
#include "Decoder.h"

bool Parser::read_bi(){return bitstream->bread();}
bool Parser::read_al(){return bitstream->bread_al();}
bool Parser::algi(){return bitstream->balgi();}

uint64_t Parser::next(uint32_t size){return bitstream->bnext(size);}
uint64_t Parser::read_un(uchar size){return bitstream->bread_n(size);}
int64_t  Parser::read_sn(uchar size){return bitstream->bread_n(size);}
uint64_t Parser::read_ue(){return bitstream->bread_ue();}
int64_t  Parser::read_se(){return bitstream->bread_se();}
uint64_t Parser::read_me(uint16_t ChromaArrayType, uint32_t mb_type){return bitstream->bread_me(ChromaArrayType, mb_type);}
uint64_t Parser::read_te(uint32_t range){return bitstream->bread_te(range);}
uint64_t Parser::read_ce(){return 0;}
uint64_t Parser::read_ae(int syntax){return cabac_core->cread_ae(syntax);}
uint16_t Parser::read_12(){return 0;}
short    Parser::read_ch(){return bitstream->bread_ch();};
bool Parser::find_nextNAL()
{
    // 0 表示没有找到，1 表示找到第一个00， 2 表示找到第二个00 4 表示找到 第三个00 5 表示找到第三个之后的01   6表示找到第二个之后的01
    uint8_t state = 0;
    //强制对齐到当前未对齐索引后面第一个对齐位
    read_al();
    while(state != 7)
    {
        //这里只是找数据，所以可以用short类型
        short tmp = read_ch();
        if(tmp == -1) return false;
        /*                            1---------[s6]----------\       .__________.
                                      |                        \----->|  finded  |
            [s0] --0--> [s1] --0--> [s2] --0--> [s4] --1--> [s5]----->|__________|
              |           |           |           |           |             |
              |<----------/ other     |           |           |             |
              |<----------------------/ other     |           |             |
              |<----------------------------------/ other     |             |
              |<----------------------------------------------/other        |
              \<------------------------------------------------------------/
        */
        switch (state)
        {
        case 0:if(tmp == 0) state = 1; break;
        case 1:if(tmp == 0) state = 2; else state = 0; break;
        case 2:if(tmp == 0) state = 4; else if(tmp == 1) state = 6; else state = 0; break;
        case 4:if(tmp == 1) state = 5; else state = 0;break;
        default:state = 0;break;
        }
        if(state == 5 || state == 6) {state = 0; break;}
    }
    return true;
}
void Parser::rfsh(){bitstream->bfrsh();}

Parser::Parser(FILE* datares, Debug* debug)
{
    this->debug = debug;

    bitstream = new Bitsbuf(datares);
    cabac_core = new cabac(this);
    pS = new ParametersS();
    pV = new ParametersV();
    pS->pps = new PPS_data();
    pS->sps = new SPS_data();
    slice_idx = 0;
    
}
Parser::~Parser()
{
    delete bitstream;
    delete cabac_core;
    delete pS->pps;
    delete pS->sps;
    delete pS;
    delete pV;
}