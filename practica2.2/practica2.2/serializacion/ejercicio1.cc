#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME 16

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
        name[MAX_NAME - 1] = 0;
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        alloc_data(MAX_NAME * sizeof(char) + sizeof(int) + sizeof(int));
        char* aux = _data;
    
        // nombre
        memcpy(aux, name, MAX_NAME * sizeof(char));
        aux += MAX_NAME * sizeof(char);
        
        // posX posY
        memcpy(aux, &x, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(aux, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {    
        if(data == nullptr) return -1;
        
        alloc_data(MAX_NAME * sizeof(char) + sizeof(int) + sizeof(int));
        if(strlen(data) >= _size)
        {
            std::cout << "Invalid data\n";
            return -1;
        }

        char* aux = data;

        memcpy(name, aux, MAX_NAME * sizeof(char));
        name[MAX_NAME-1] = 0;
        aux += MAX_NAME * sizeof(char);

        memcpy(&x, aux, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(&y, aux, sizeof(int16_t));

        return 0;
    }


public:
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("FicheroJug1", O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if(fd == -1) {
        std::cout << "ERROR OPENING OR CREATING FILE\n";
        return -1;
    }
    if(write(fd, one_w.data(), one_w.size()) == -1) {
        std::cout << "ERROR WRITING ON FILE\n";
        return -1;
    }
    close(fd);

    // 3. Leer el fichero
    fd = open("FicheroJug1", O_RDONLY, 0666);
    if(fd == -1) {
        std::cout << "ERROR READING FILE\n";
        return -1;
    }
    char* buff = (char*) malloc(one_r.size() + 1);
    char* buffCpy = buff;
    int i = 0;
    while(i < one_r.size() && read(fd, buff, 1) == 1) {
        i++;
        buff++;
    }
    buff[i] = 0;

    // 4. "Deserializar" en one_r
    one_r.from_bin(buffCpy);

    // 5. Mostrar el contenido de one_r
    std::cout << "Player name: " << one_r.name << "     x: " << one_r.x << "    y: " << one_r.y << "\n";

    free(buffCpy);
    close(fd);

    return 0;
}

