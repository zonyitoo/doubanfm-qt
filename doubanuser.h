#ifndef DOUBANUSER_H
#define DOUBANUSER_H

#include <QString>

class DoubanUser
{
public:
    DoubanUser();

    QString getUserId() const;
    QString getExpire() const;
    QString getToken() const;

    void login();

private:
    QString user_id;
    QString expire;
    QString token;
};

#endif // DOUBANUSER_H
