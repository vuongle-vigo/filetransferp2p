# include <stdio.h>
# include <string.h>

FILE* create_file_to_download(char *filename);
int main(){
    char filename[100]  = "test.txt";
    FILE *fp = create_file_to_download(filename);

}

FILE* create_file_to_download(char *filename){
    FILE *fp;
    char new_filename[100];
    int i = 1;
    fp = fopen(filename, "r");
    if (fp != NULL)
    {   
        fclose(fp);
        strcpy(new_filename, filename);
        char *p = strstr(new_filename, "."); // luu duoi file vao p : (.jpeg)
        char duoifile[100];
        strcpy(duoifile, p);
        char *p2 = strtok(new_filename, "."); //tenfile khong chua duoi
        char countstr[10];
        int n = sprintf(countstr, "%d", i);
        printf("%s\n", countstr);
        strcat(p2, countstr);
        printf("%s\n", p2);
        strcat(p2, duoifile);
        printf("%s\n", p2);
        strcpy(new_filename, p2);
        while ((fp = fopen(new_filename, "r")) != NULL)
        {   
            fclose(fp);
            i++;
            strcpy(new_filename, filename);
            char *p = strstr(new_filename, "."); // luu duoi file vao p : (.jpeg)
            char duoifile[100];
            strcpy(duoifile, p);
            char *p2 = strtok(new_filename, "."); //tenfile khong chua duoi
            char countstr[10];
            int n = sprintf(countstr, "%d", i);
            printf("%s\n", countstr);
            strcat(p2, countstr);
            printf("%s\n", p2);
            strcat(p2, duoifile);
            printf("%s\n", p2);
            strcpy(new_filename, p2);
        }
        fp = fopen(new_filename, "w");
        if (fp == NULL)
        {
            printf("Không thể tạo tập tin %s\n", new_filename);
            return fp;
        }
        printf("Đã tạo tập tin %s\n", new_filename);
    }
    else
    {
        // Tạo tập tin gốc
        fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf("Không thể tạo tập tin %s\n", filename);
            return fp;
        }
        printf("Đã tạo tập tin %s\n", filename);
    }
    return fp;
}