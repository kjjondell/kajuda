#ifndef BUFFER_H
#define BUFFER_H


class Buffer
{
public:
    Buffer(int nbUnits, int unitSize);
    ~Buffer();
    float* getUnit(int unit);
    void setFull(int unit);
    void setEmpty(int unit);
    bool isFull(int unit);
    int getNbUnits();
    int getUnitSize();

private:
    int unitSize;
    int nbUnits;
    float *bufferArray;
    bool *bufferFull;

};

#endif // BUFFER_H
