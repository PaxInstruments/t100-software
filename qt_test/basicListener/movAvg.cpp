#include "movAvg.h"

MovAvg::MovAvg(int len)
{    
    m_sum = 0;
    m_index = 0;
    if(len <= 0)
    {
        len = 1;
    }
    else if(len > m_maxLength)
    {
        len = m_maxLength;
    }
    m_len = len;
    m_stable = false;
}

void MovAvg::reset()
{
    m_sum = 0;
    m_index = 0;
    m_stable = false;
    for(int i=0;i<m_maxLength;i++)
    {
        m_dataArray[i] = 0x00;
    }
}

double MovAvg::execute(double new_input)
{
    double filter_out;

    m_sum -= m_dataArray[m_index];
    m_sum += new_input;
    m_dataArray[m_index] = new_input;
    m_index = m_index + 1;

    if(m_index == m_len)
    {
        m_stable = true;
        m_index = 0;
    }

    filter_out = m_sum / m_len;

    if(m_stable)
    {
        return filter_out;
    }
    {
        return new_input;
    }
}

void MovAvg::setFilterLen(int len)
{
    m_sum = 0;
    m_index = 0;
    m_stable = false;

    for(int i=0;i<m_maxLength;i++)
    {
        m_dataArray[i] = 0x00;
    }

    if(len <= 0)
    {
        len = 1;
    }
    else if(len > m_maxLength)
    {
        len = m_maxLength;
    }

    m_len = len;
}
