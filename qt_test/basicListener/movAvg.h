#ifndef MOVAVG_H
#define MOVAVG_H

#define m_maxLength 32

class MovAvg
{
    public:
        MovAvg(int len);
        void reset();
        void setFilterLen(int len);
        double execute(double new_input);

    private:
        int m_len;
        int m_index;
        double m_sum;
        bool m_stable;
        double m_dataArray[m_maxLength];
};

#endif // MOVAVG_H
