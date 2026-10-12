// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include <stdexcept>
TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len = 1)// конструктор
{
    if (len <= 0)
        throw "Длина битового поля должна быть больше 0";
    BitLen = len;
    MemLen = (BitLen + 31) / (sizeof(TELEM) * 8);
    pMem = new TELEM[MemLen]();
    
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField() // деструктор
{
    delete [] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if (n < 0 || n>BitLen) throw "Неверное число";
    return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    if (n < 0 || n>BitLen) throw "Неверное число";
    else {
        return TELEM(1) << (n % (sizeof(TELEM) * 8));
    }
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >BitLen) throw "Неверное число";
    else {
        int field = GetMemIndex(n);
        TELEM mask = GetMemMask(n);
        pMem[field] |= mask;
    }
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0||n>BitLen) throw "Неверное число";
    else {
        int field = GetMemIndex(n);
        TELEM mask = GetMemMask(n);
        pMem[field] &= ~mask;
    }
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || n>BitLen) throw "Неверное число";
        return (pMem[GetMemIndex(n)] & GetMemMask(n))!=0;
}

// битовые операции
TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
    if (this != &bf)
    {
        BitLen = bf.BitLen;
        MemLen = bf.MemLen;
        delete[]pMem;
        pMem = new TELEM[MemLen];
        for (int i = 0; i < MemLen; i++)
            pMem[i] = bf.pMem[i];

    }
    return *this;
}
TBitField& TBitField::operator=(TBitField&& bf) // присваивание
{
    if (this != &bf)
    {
        BitLen = bf.BitLen;
        MemLen = bf.MemLen;
        delete[]pMem;
        pMem = bf.pMem;
        bf.BitLen = 0;
        bf.MemLen = 0;
        bf.pMem = nullptr;
    }
    return *this;
}

int TBitField::operator==(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen)return 0;
    for (int i = 0; i < MemLen-1; i++)
        if (pMem[i] != bf.pMem[i])return 0;
    for (int i = (MemLen - 1) * sizeof(TELEM) * 8; i < BitLen;i++)
        if (GetBit(i) != bf.GetBit(i))return 0;
    return 1;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen)return 1;
    for (int i = 0; i < MemLen-1; i++)
        if (pMem[i] != bf.pMem[i])return 1;
    for (int i = (MemLen - 1) * sizeof(TELEM) * 8; i < BitLen; i++)
        if (GetBit(i) != bf.GetBit(i))return 1;
    return 0;
}
void TBitField::ClearTail()
{
    const int usedInLast = BitLen % (sizeof(TELEM) * 8);
    if (usedInLast == 0)
        return;
    TELEM mask = (TELEM(1) << usedInLast) - 1;

    pMem[MemLen - 1] &= mask;
}
TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
    int max;
    
    if (BitLen > bf.BitLen)max=BitLen;
    else max=bf.BitLen;
    TBitField res(max);
    for (int i = 0; i < bf.MemLen; i++) {
        res.pMem[i] = bf.pMem[i];
    }
    for (int i = 0; i < MemLen; i++) {
        res.pMem[i] |= pMem[i];
    }
    
    return res;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
    TBitField cd(bf);
    cd.ClearTail();
    this->ClearTail();
    int max;
    if (BitLen > cd.BitLen)max = BitLen;
    else max = cd.BitLen;
    TBitField res(max);
    for (int i = 0; i < cd.MemLen; i++) {
        res.pMem[i] = cd.pMem[i];
    }
    for (int i = 0; i < MemLen; i++) {
        res.pMem[i] &= pMem[i];
    }
    res.ClearTail();
    return res;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField res (*this);
    for (int i = 0; i < MemLen; i++)
        res.pMem[i] = ~pMem[i];
    this->ClearTail();
    return res;
}

// ввод/вывод

istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    for (int i = 0; i < bf.BitLen; i++) {
        char tmp;
        istr >> tmp;
        if (tmp == '1')bf.SetBit(i);
        if (tmp == '0')bf.ClrBit(i);
        else return istr;
    }
    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++)
        ostr << bf.GetBit(i);
    return ostr;
}
