#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

#include "Display.h"


int hexcolor(int r, int g, int b)
{
    return (r<<16) | (g<<8) | b;
}

void sort(int a[], int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    int t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                }
        }
    }
}

class Box{
    int data_size=1;
    int *data = new int[data_size];

public:
    Box(int );
    void arr_resize(size_t);
    void set_pos(int []);
    void add_box(int *[]);
    void show();

};

Box::Box(int size_){
    int data[size_];
    data_size = size_;
    //data = &data[size_];
    cout << "size" << data_size << endl;
}

void Box::arr_resize(size_t newsize){
    int *newArr = new int[newsize];
    memcpy(newArr, data, data_size*sizeof(int));
    data_size = newsize;
    delete [] data;
    data = newArr;
    cout << "sizeII" << data_size << endl;
}

void Box::set_pos(int data_in[]){
    data = data_in;
    sort(data,4);
    cout << "DATA:" << data << endl;
}

void Box::add_box(int *data_in[] ){
    // Increase box size
    size_t new_size = sizeof(data_in);
    cout << "new size" << new_size << endl;
    if(new_size > data_size ){
        arr_resize(new_size);
    }

    for(int i=0;i<new_size;i++){
        data[*data_in[i]] = *data_in[i];
    }

}
void Box::show(){
    for(int i=0; i<data_size;i++){
        cout << data[i];
    }
}


int main(){
    int displayBlock[16] = {};
    int transformer[16] = {};
    for(int i=0;i<16;i++){
        displayBlock[i] = hexcolor(i*16,i*16,i*16);
    }

    for(int i=0;i<16;i++){
            cout << displayBlock[i] << endl;
    }


    // Box pattern
    int pos[4] = {0,1,4,2}; // horizon line 4x1
    int pos_new[5] = {0,1,4,2,6};

    int p = 3;
    size_t j = p;
    Box b = Box(4);

    cout << endl << sizeof(pos) << endl;
    b.set_pos(pos);
    b.show();
    b.add_box(pos_new);
    //b.arr_resize(6);
    for(int i=0;i<4;i++){
        cout << pos[i];
    }





    return 0;
}
