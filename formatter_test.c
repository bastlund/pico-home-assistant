// Test file för att jämföra formatters
#include <stdio.h>

int badly_formatted_function(int x, int y) {
    int result = x + y;
    if (result > 10) {
        printf("Result is greater than 10: %d\n", result);
    } else {
        printf("Result is: %d\n", result);
    }
    return result;
}
