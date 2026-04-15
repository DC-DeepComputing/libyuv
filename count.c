#include <stdio.h>
#include <string.h>

int main() {
    FILE* f = fopen("source/row_win.cc", "r");
    char line[1024];
    int cnt = 0;
    int line_num = 0;
    while(fgets(line, sizeof(line), f)) {
        line_num++;
        if(strncmp(line, "#if", 3) == 0 && strncmp(line, "#elif", 5) != 0) {
            cnt++;
        }
        if(strncmp(line, "#endif", 6) == 0) {
            cnt--;
            if (cnt < 0) { printf("Unmatched #endif at %d\n", line_num); break; }
        }
    }
    printf("Final count: %d\n", cnt);
    return 0;
}
