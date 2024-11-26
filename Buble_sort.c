#include <stdio.h>
#include <stdbool.h>

bool isgod(int ari[], int size) {
    for (int i = 1; i < size; i++) {
        if (ari[i] < ari[i-1]) {
            return false;
        }
    }
    return true;
}

int main() {
    int arr[] = {5, 3, 7};
    int sizea = sizeof(arr) / sizeof(arr[0]);
    int temp = 0;

    while (!isgod(arr, sizea)) {
        for (int i = 1; i < sizea; i++) {
            if (arr[i] < arr[i-1]) {
                
                temp = arr[i];
                arr[i] = arr[i-1];
                arr[i-1] = temp;
            }
        }
    }
  
    for (int i = 0; i < sizea; i++) {
        printf("%d ", arr[i]);
    }
    return 0;
}
