#ifndef LTECELL_DATA_H
#define LTECELL_DATA_H

class LteCellData {

public:
    LteCellData(){}
    LteCellData(long data1, long data2){this->data1=data1; this->data2=data2;}
    ~LteCellData(){}
    void set(long data1, long data2){
        this->data1=data1;
        this->data2=data2;
    }
    void get(long &data1, long &data2){
        data1=this->data1;
        data2=this->data2;
    }
    volatile LteCellData & operator= (LteCellData &rValue) volatile
    {
        this->data1 = rValue.data1;
        this->data2 = rValue.data2;
        return *this;
    }
    LteCellData & operator= (volatile LteCellData &rValue)
    {
        this->data1 = rValue.data1;
        this->data2 = rValue.data2;
        return *this;
    }

public:
    volatile long data1;
    volatile long data2;

};

//thread safe 1to1 producer consumer globals
volatile int m_Read;
volatile int m_Write;
static const int Size = 10;
volatile LteCellData m_Data[Size];

bool PushElement(LteCellData &Element){
      int nextElement = (m_Write + 1) % Size;
      if(nextElement != m_Read){
            m_Data[m_Write] = Element;
            m_Write = nextElement;
            return true;
      }
      else
            return false;
}

bool PopElement(LteCellData &Element){
       if(m_Read == m_Write)
             return false;
       int nextElement = (m_Read + 1) % Size;
       Element = m_Data[m_Read];
       m_Read = nextElement;
       return true;
}

#endif // LTECELL_DATA_H
