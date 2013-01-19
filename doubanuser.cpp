#include "doubanuser.h"

DoubanUser::DoubanUser() {

}

QString DoubanUser::getUserId() const {
    return user_id;
}

QString DoubanUser::getExpire() const {
    return expire;
}

QString DoubanUser::getToken() const {
    return token;
}

void DoubanUser::login() {

}
