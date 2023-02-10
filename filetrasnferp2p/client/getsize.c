#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp;
    char filename[] = "test.jpeg";
    char new_filename[100];
    int i = 1;

    // Kiểm tra tồn tại của tập tin
    fp = fopen(filename, "r");
    if (fp != NULL)
    {
        fclose(fp);
        // Tạo tập tin với tên khác
        sprintf(new_filename, "%s%d", filename, i);
        while ((fp = fopen(new_filename, "r")) != NULL)
        {
            fclose(fp);
            i++;
            sprintf(new_filename, "%s%d", filename, i);
        }
        
        // Tạo tập tin mới
        fp = fopen(new_filename, "w");
        if (fp == NULL)
        {
            printf("Không thể tạo tập tin %s\n", new_filename);
            return 1;
        }

        printf("Đã tạo tập tin %s\n", new_filename);
        fclose(fp);
    }
    else
    {
        // Tạo tập tin gốc
        fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf("Không thể tạo tập tin %s\n", filename);
            return 1;
        }

        printf("Đã tạo tập tin %s\n", filename);
        fclose(fp);
    }

    return 0;
}