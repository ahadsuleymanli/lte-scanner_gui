#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H
#include "lte-cell_data.h"
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
#endif // PRODUCER_CONSUMER_H
