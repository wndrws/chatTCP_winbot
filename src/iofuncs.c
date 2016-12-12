//
// Created by WebSter on 28.11.2016.
//

#include <stdint.h>
#include "etcp.h"
#define RECLEN_TYPE uint16_t

// Прочесть len байтов из сокета sock и записать по указателю bp.
int readn(SOCKET sock, char* bp, int len) {
    int cnt, rc;

    cnt = len;
    while(cnt > 0) {
        rc = recv(sock, bp, cnt, 0);
        if(rc < 0) {                // Ошибка чтения?..
            if(errno == EINTR)      // Или вызов был прерван?
                continue;           // Повторить чтение
            return -1;              // Вернуть код ошибки
        }
        if(rc == 0) {               // Если ничего не принято,
            return len - cnt;       // вернуть неполный счетчик.
        }
        bp += rc;
        cnt -= rc;
    }
    return len;
}

// Прочесть из сокета sock запись переменной длины и записать по
// по указателю bp. Максимальный размер буфера для записи равен len.
// Максимальный размер записи определяется типом RECLEN_TYPE.
int readvrec(SOCKET sock, char* bp, int len) {
    RECLEN_TYPE reclen;
    int rc;

    // Прочитать длину записи:
    rc = readn(sock, (char*) &reclen, sizeof(RECLEN_TYPE));
    // Проверка, что считалось верное число байт:
    if(rc != sizeof(RECLEN_TYPE)) return rc < 0 ? -1 : 0;
    switch (sizeof(RECLEN_TYPE)) {
        case 2: reclen = ntohs(reclen); break; //uint16_t
        case 4: reclen = ntohl(reclen); break; //uint32_t
        default: break;
    }
    if(reclen > len) {
        // В буфере не хватает места для размещения данных -
        // сохраняем, что влезает, а остальное отбрасываем.
        while(reclen > 0) {
            rc = readn(sock, bp, len);
            if(rc != len) return rc < 0 ? -1 : 0;
            reclen -= len;
            if(reclen < len) len = reclen;
        }
        set_errno(EMSGSIZE);
        return -1;
    }
    // Если всё в порядке, читаем запись:
    rc = readn(sock, bp, reclen);
    if(rc != reclen) return rc < 0 ? -1 : 0;
    return rc;
}

// Прочесть из сокета sock одну строку (до '\n') по указателю bufptr.
// Максимальный размер буфера для записи равен len.
int readline(SOCKET sock, char* bufptr, int len) {
    char* bufx = bufptr;
    int cnt = 0;
    //static char* bp;
    //static int cnt = 0;
    //static char b[65536];
    char c;

    while (--len > 0) {
        cnt = recv(sock, &c, 1, 0);
        if (cnt < 0) {
            if (errno == EINTR) {
                len++; /* Уменьшим на 1 в заголовке while. */
                continue;
            }
            return -1;
        }
        if (cnt == 0) return 0;
        *bufptr++ = c;
        if (c == '\n') {
            *bufptr = '\0';
            return (int) (bufptr - bufx);
        }
    }

//    while (--len > 0) {
//        if(--cnt <= 0) {
//            cnt = recv(sock, b, sizeof(b), 0);
//            if(cnt < 0) {
//                if(errno == EINTR) {
//                    len++; /* Уменьшим на 1 в заголовке while. */
//                    continue;
//                }
//                return -1;
//            }
//            if(cnt == 0) return 0;
//            bp = b;
//        }
//        c = *bp++;
//        *bufptr++ = c;
//        if(c == '\n') {
//            *bufptr = '\0';
//            return (int) (bufptr - bufx);
//        }
//    }
    set_errno(EMSGSIZE);
    return -1;
}