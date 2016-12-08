#include "buffer.h"

Buffer::Buffer(int nbUnits, int unitSize)
{
    this->unitSize = unitSize;
    this->nbUnits = nbUnits;
    this->bufferArray = new float[unitSize*nbUnits];
    this->bufferFull = new bool[nbUnits];

    for(int i=0;i<nbUnits;i++){
        bufferFull[i]=false;
    }
}

bool Buffer::isFull(int unit){
    return bufferFull[unit];
}

float* Buffer::getUnit(int unit){
    return bufferArray+unit*unitSize;
}


void Buffer::setFull(int unit){
    bufferFull[unit] = true;
}

void Buffer::setEmpty(int unit){
    bufferFull[unit] = false;
}

int Buffer::getNbUnits(){
    return nbUnits;
}

int Buffer::getUnitSize(){
    return unitSize;
}
