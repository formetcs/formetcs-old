#include "abstypes.h"

Rat::Rat()
{
    numerator = 0;
    denominator = 1;
}

Rat::Rat(int val)
{
    numerator = val;
    denominator = 1;
}

Rat::Rat(double val)
{
    fromString(QString::number(val));
}

Rat::Rat(QString val)
{
    fromString(val);
}

Rat::Rat(int num, int denom)
{
    numerator = num;
    denominator = denom;
}

double Rat::toDouble()
{
    return ((double) numerator / (double) denominator);
}

QString Rat::toRatString()
{
    return QString("%1/%2").arg(numerator).arg(denominator);
}

QString Rat::toFloatString()
{
    return QString::number(toDouble());

}

void Rat::fromString(QString val)
{
    int index = val.indexOf(".");
    if(index < 0) // integer value
    {
        numerator = val.toLongLong();
        denominator = 1;
    }
    else // floating point value
    {
        QString leftPart = val.left(index);
        QString rightPart = val.mid(index + 1);
        qint64 leftNum = leftPart.toLongLong();
        qint64 rightNum = rightPart.toLongLong();
        if(leftNum < 0)
        {
            rightNum *= -1;
        }
        int digitCount = rightPart.size();
        qint64 factor = 1;
        for(int i = 0; i < digitCount; i++)
        {
            factor *= 10;
        }

        numerator = leftNum * factor + rightNum;
        denominator = factor;

        simplify();
    }
}

void Rat::simplify()
{
    for (int i = 2; i <= denominator; i++)
    {
        while(numerator % i == 0 && denominator % i == 0)
        {
            numerator /= i;
            denominator /= i;
        }
    }
}
