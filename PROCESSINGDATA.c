#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_ROWS 1000
#define MAX_FILENAME_LENGTH 20
#define MAX_PRICE 100
#define MINIMUM_UPTIME_HOURS_INDEX 4
#define MAXIMUM_UPTIME_HOURS 12

void correctDate(char [], char []);
void writeData(char [][MAX_LINE_LENGTH], FILE *);
void floatData(char [][MAX_LINE_LENGTH], float []);
float averagePrice(float []);
void bubble_sort(float [], char* []);
int price_limit_index(float []);
void output(char* [], char* [], float [], char []);


int main(){
    printf("Ootan kella 00:01");
   while (1){
      time_t now = time(NULL);
      struct tm *t = localtime(&now);
      if (t->tm_hour == 0 && t->tm_min == 1){
         printf("PROCESSING DATA ACTIVATED\n");
         FILE *fp;
         char data[MAX_NUM_ROWS][MAX_LINE_LENGTH] = {0};
         char filename[MAX_FILENAME_LENGTH];
         char new_filename[MAX_FILENAME_LENGTH];

         correctDate(filename, new_filename);

         fp = fopen(filename, "r");
         if (fp == NULL) {
            printf("Error opening file %s\n", filename);
            return -1;
         }

         writeData(data, fp);
         fclose(fp);
         float fdata[24];
         floatData(data, fdata);

         //ANALYSIS

         printf("AVERAGE PRICE: %f\n", averagePrice(fdata));
         char *timedata[] = {"00:00", "01:00", "02:00", "03:00", "04:00", "05:00", "06:00", "07:00", "08:00",
         "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00",
         "20:00", "21:00", "22:00", "23:00"};
         char *legacytimedata[] = {"00:00", "01:00", "02:00", "03:00", "04:00", "05:00", "06:00", "07:00", "08:00",
         "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00",
         "20:00", "21:00", "22:00", "23:00"};

         bubble_sort(fdata, timedata);

         output(timedata, legacytimedata, fdata, new_filename);
         Sleep(60000);
         printf("Ootan j2rgmist 00:01");
      }
      //Sleep(2000);
      //printf("WAITING...\n");
   }

   return 0;
}

void correctDate(char filename[], char new_filename[]){
   time_t t = time(NULL);
   strftime(filename, MAX_FILENAME_LENGTH, "NPS-%Y-%m-%d.csv", localtime(&t));
   strftime(new_filename, MAX_FILENAME_LENGTH, "NPS-%Y-%m-%d.txt", localtime(&t));
}

void writeData(char data[][MAX_LINE_LENGTH], FILE *fp){
   char line[MAX_LINE_LENGTH];
   char *token;
   char *delim = ";";
   int row = 0;
   int col = 0;

   printf("FILE SUCCESSFULLY FOUND\n");

   while (fgets(line, MAX_LINE_LENGTH, fp) != NULL){
      if (row > 0){
         token = strtok(line, delim);
         col = 0;
         while (token != NULL) {
               if (col == 2){
                  strcpy(data[row-1], token);
               }
               token = strtok(NULL, delim);
               col++;
         }
      }
      row++;
   }

}

void floatData(char data[][MAX_LINE_LENGTH], float fdata[]){
   int i;
	for (i = 0; i < 24; i++){
		char *x = data[i];
		char n[strlen(x)];
		int j, k;
		for (j = 0, k = 0; j < strlen(x); j++){
			if (x[j] >= '0' && x[j] <= '9'){
			   n[k++] = x[j];
			}
			else if(x[j] == ','){
			   n[k++] = '.';
			}

		}
      n[k] = '\0';
		float number = atof(n);
		fdata[i] = number;
	}
}

float averagePrice(float fdata[]){
   int i;
   float sum = 0;
   for (i = 0; i < 24; i++){
      sum += fdata[i];
   }
   return sum / 24;
}

void bubble_sort(float fdata[], char* timedata[]){
   int i, j;
   float temp;
   char *temp2;
   for (i = 0; i < 23; i++){
      for (j = 0; j < 24-i-1; j++){
         if (fdata[j] > fdata[j+1]){
            temp = fdata[j];
            temp2 = timedata[j];
            fdata[j] = fdata[j+1];
            timedata[j] = timedata[j+1];
            fdata[j+1] = temp;
            timedata[j+1] = temp2;
         }
      }
   }
}

int price_limit_index(float fdata[]){
   int i;
   for (i = MINIMUM_UPTIME_HOURS_INDEX; i < 12; i++){
      if (fdata[i] > MAX_PRICE){
         if (i > MINIMUM_UPTIME_HOURS_INDEX){
            return i-1;
         }
         return i;
      }
      else{
         continue;
      }

   }
   return 11;
}

void output(char* timedata[], char* legacytimedata[], float fdata[], char new_filename[]){
   int LAST_TIME = price_limit_index(fdata);
   int i, j;

   FILE *fp;
   fp = fopen(new_filename, "w");
   for (i = 0; i < 24; i++){
      int found = 0;
      for (j = 0; j < 24; j++){
         if (strcmp(legacytimedata[i], timedata[j]) == 0 && j < (LAST_TIME + 1)){
            fprintf(fp, "+ %s\n", legacytimedata[i]);
            found = 1;
            break;
         }
      }
      if (!found){
         fprintf(fp, "- %s\n", legacytimedata[i]);
      }
   }
   fclose(fp);
   printf("\nDATA ADDED TO %s SUCCESSFULLY\n", new_filename);
}
