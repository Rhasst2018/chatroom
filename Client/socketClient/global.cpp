#include "global.h"

QString* getGlobalUsername() {
    static QString globalUsername;
    return &globalUsername;
}
