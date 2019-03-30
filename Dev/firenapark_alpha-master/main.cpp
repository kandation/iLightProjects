#include <iostream>

using namespace std;
#include <stdio.h>



int hexcolor(int r, int g, int b)
{
    return (r<<16) | (g<<8) | b;
}

class Box{
    int *data;
public:
    Box(int );
    void set_pos(int []);
    void show();

};

Box::Box(int size_){
    data = &data[size_];

}

void Box::set_pos(int data_in[]){
    data = data_in;
}

void Box::show(){
    for(int i=0; i<3;i++){
        cout << data[i];
    }
}

v

int main(){
    /*int *data =  ;
    for(int i=0;i<sizeof(data);i++){
        data[i] = i;
    }

    cout << sizeof(data)/sizeof(*data);
    for(int i=0;i<sizeof(data);i++){
        cout << data[i];
    }
    int *p = new int[7];
std::cout << "Length of array = " << (sizeof(p)/sizeof(p[0])) << std::endl;
*/
    int p = 3;
    Box tertis_horizon_line = Box(3);
    int pos[3] = {1,2,3};
    tertis_horizon_line.set_pos(pos);
    tertis_horizon_line.show();

    Box *game = new Box[5];






    return 0;
}
