#include <stdio.h>
#include <math.h>
#include <conio.h> 

/*
21. В одномерном массиве B из 20 элементов найти разность сумм элементов с 
четными и нечетными индексами.
*/

#define ARRAY_SIZE	10

int main() 
{ 			
	int b[ARRAY_SIZE]; // Массив с исходными данными
	
	int a, y; // a - Сумма нечетных элементов | y - Сумма четных элементов
	
	clrscr(); 
	
	printf("Введите значения для массива B, размер массива = %d\n",ARRAY_SIZE); 
	for(int i = 0; i < ARRAY_SIZE; i++)
	{ 
		// Запись значений в массив B
		printf("b[%d]=", i);
		scanf("%d", &b[i]);
	} 					
	printf("Массив b\n"); 
	for(int i=0;i<ARRAY_SIZE;i++)
	{ 
		// Перечисляем 
		if(i%2 == 1) //нечетный индекс
		{
			a = a + b[i]; // Суммируем нечетные элементы
		}
		else
		{
			y = y + b[i]; // Суммируем четные элементы
		}
	} 
	printf("Сумма нечетных элементов равна: %d", a);
	printf("Сумма четных элементов равна: %d", y);	
	printf("разность сумм элементов с четными и нечетными индексами равна: %d",a-y);
	
	getch();
	return 0; 
}
