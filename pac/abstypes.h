#ifndef ABSTYPES_H
#define ABSTYPES_H

#include <QString>

class Rat
{
public:
    Rat();
    Rat(int val);
    Rat(double val);
    Rat(int num, int denom);
    Rat(QString val);

    double toDouble();
    QString toRatString();
    QString toFloatString();
    void simplify();


private:
    void fromString(QString val);

    qint64 numerator;
    qint64 denominator;
};

#endif // ABSTYPES_H
