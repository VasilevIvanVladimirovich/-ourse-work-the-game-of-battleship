#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

#define Size_Field 10
#define Edges_Field ((Row+What_Boat<Size_Field) && (Row-What_Boat>0)&& (Col+What_Boat<Size_Field) && (Col-What_Boat>0))

struct data
{
	int sock;
};

void Clear_Field(char Field[Size_Field][Size_Field])
{
	for(int i=0;i<Size_Field;i++)
	{
		for(int j=0;j<Size_Field;j++)
		{
		Field[i][j]=' ';

		}
	}
}
void Hint_Clear_Field(char User_Field[Size_Field][Size_Field])
{
	for(int i=0;i<Size_Field;i++)
	{
		for(int j=0;j<Size_Field;j++)
		{ 
			if(User_Field[i][j]=='^'||User_Field[i][j]=='>'||User_Field[i][j]=='v'||User_Field[i][j]=='<') User_Field[i][j]=' ';
		}
	}
}

void Clear_User_Field_On_Start_Game(char User_Field[Size_Field][Size_Field])
{
	for(int i=0;i<Size_Field;i++)
	{
		for(int j=0;j<Size_Field;j++)
		{ 
			if(User_Field[i][j]=='.') User_Field[i][j]=' ';
		}
	}
}

void Show_Game_Field_User(char User_Field[Size_Field][Size_Field])
{
	printf("\n");
	printf("               Игрок \n");
	printf("  |A||B||C||D||E||F||G||H||I||J|\n");
	for(int i=0;i<Size_Field;i++)
	{
		printf("%d-",i);
		for(int j=0;j<Size_Field;j++)
		{ 
			printf("|%c|",User_Field[i][j]);
		}
		printf("\n");
	}
}

void Show_Game_Field_test(char User_Field[Size_Field][Size_Field])
{
	printf("\n");
	printf("              test \n");
	printf("  |A||B||C||D||E||F||G||H||I||J|\n");
	for(int i=0;i<Size_Field;i++)
	{
		printf("%d-",i);
		for(int j=0;j<Size_Field;j++)
		{ 
			printf("|%c|",User_Field[i][j]);
		}
		printf("\n");
	}
}
void Move_Hint(char User_Field[Size_Field][Size_Field], int Row,int Col,int What_Boat)
{
	int Free_Place;
	if(Row-What_Boat+1>=0)
		{
			Free_Place=1;
			for(int i=1;i<=What_Boat-1;i++) 
			{
				if(User_Field[Row-i][Col]=='H'||User_Field[Row-i][Col]=='.')
				{
					Free_Place=0;
				}
			}	
			if (Free_Place==1) User_Field[Row-1][Col]='^';
		}
		if(Row+What_Boat-1<10){
			Free_Place=1;
			for(int i=1;i<=What_Boat;i++) 
			{
				if(User_Field[Row+i][Col]=='H'||User_Field[Row+i][Col]=='.')
				{
					Free_Place=0;
				}
			}
			if (Free_Place==1) User_Field[Row+1][Col]='v';
		} 
		if(Col+What_Boat-1<10){
			Free_Place=1;
			for(int i=1;i<=What_Boat-1;i++) 
			{
				if(User_Field[Row][Col+i]=='H'||User_Field[Row][Col+i]=='.')
				{
					Free_Place=0;
				}
			}
			if (Free_Place==1) User_Field[Row][Col+1]='>';
		}
		if(Col-What_Boat+1>=0){
			Free_Place=1;
			for(int i=1;i<=What_Boat-1;i++) 
			{
				if(User_Field[Row][Col-i]=='H'||User_Field[Row][Col-i]=='.')
				{
					Free_Place=0;
				}
			}
			if (Free_Place==1) User_Field[Row][Col-1]='<';
		}
}

int Check_Q(char User_Field[Size_Field][Size_Field])
{
	for(int i=0;i<Size_Field;i++)
	{
		for(int j=0;j<Size_Field;j++)
		{ 
			if(User_Field[i][j]=='^'||User_Field[i][j]=='>'||User_Field[i][j]=='v'||User_Field[i][j]=='<') return 1;
		}
	}
	return 0;
}
int Check_The_Field(char User_Field[Size_Field][Size_Field],int Row,int Col,int What_Boat)
{
	if(User_Field[Row][Col]=='H') return 0;
	if(User_Field[Row][Col]=='.') return 0;
	if(Check_Q(User_Field)==1) 
	{
	 if(User_Field[Row][Col]=='^'||User_Field[Row][Col]=='>'||User_Field[Row][Col]=='v'||User_Field[Row][Col]=='<') return 1;
	 else return 0;
	}else return 2;
	
}
void Spawn_Full_Board(char User_Field[Size_Field][Size_Field],int Row,int Col,int What_Boat)
{
	char Direction=User_Field[Row][Col];
		if(Direction=='^')
		{
				for(int i=0;i<What_Boat-1;i++) 
				{
					User_Field[Row-i][Col]='H';
				}
		}	
		if(Direction=='v')
		{
				for(int i=0;i<What_Boat-1;i++) 
				{
					User_Field[Row+i][Col]='H';
				}	
		}
		if(Direction=='>')
		{
				for(int i=0;i<What_Boat-1;i++) 
				{
					User_Field[Row][Col+i]='H';
				}	
		}
		if(Direction=='<')
		{
				for(int i=0;i<What_Boat-1;i++) 
				{
					User_Field[Row][Col-i]='H';
				}	
		}
}

void Around_Point(char User_Field[Size_Field][Size_Field],int Row,int Col,int What_Boat,char Draw)
{
	int Up=0,Down=0,Right=0,Left=0,i=0;
	char Positions;
	if(User_Field[Row-1][Col]=='H' || User_Field[Row+1][Col]=='H'||User_Field[Row-1][Col]=='*'||User_Field[Row+1][Col]=='*' || User_Field[Row+1][Col]=='!'||   User_Field[Row-1][Col]=='!'|| User_Field[Row+1][Col]=='X'||   User_Field[Row-1][Col]=='X') Positions='|';
	else if(User_Field[Row][Col-1]=='H' || User_Field[Row][Col+1]=='H'||User_Field[Row][Col-1]=='*' ||User_Field[Row][Col+1]=='*'|| User_Field[Row][Col+1]=='!'||   User_Field[Row][Col-1]=='!'|| User_Field[Row][Col+1]=='X'||   User_Field[Row][Col-1]=='X') Positions='-';
	else if(User_Field[Row][Col]=='X'||User_Field[Row][Col]=='H')
	{
		Positions='*';
		if (Row+1<=9 && User_Field[Row+1][Col]!='o') User_Field[Row+1][Col]='.';
		if (Row-1>=0 && User_Field[Row-1][Col]!='o') User_Field[Row-1][Col]='.';
		if (Col+1<=9 && User_Field[Row][Col+1]!='o') User_Field[Row][Col+1]='.';
		if (Col-1>=0 && User_Field[Row][Col-1]!='o') User_Field[Row][Col-1]='.';
	}
	if(Positions=='|')
	{
		while(User_Field[Row+i][Col]=='H'||User_Field[Row+i][Col]=='*'||User_Field[Row+i][Col]=='!'||User_Field[Row+i][Col]=='X')
		{
			i++;
			continue;
		}
		if(Row+i >=0 && Row+i <=9 && Col>=0 && Col<=9&&User_Field[Row+i][Col]!='o') User_Field[Row+i][Col]=Draw;
		i=0;
		while(User_Field[Row+i][Col]=='H'||User_Field[Row+i][Col]=='*'||User_Field[Row+i][Col]=='!'||User_Field[Row+i][Col]=='X')
		{
			i--;
			continue;
		}
		if(Row+i >=0 && Row+i <=9 && Col>=0 && Col<=9&&User_Field[Row+i][Col]!='o') User_Field[Row+i][Col]=Draw;
		
			while(1) 
			{
				if(User_Field[Row+Down][Col]=='H'||User_Field[Row+Down][Col]=='*'||User_Field[Row+Down][Col]=='!'||User_Field[Row+Down][Col]=='X')
				{
					if(Row+Down >=0 && Row+Down <=9 && Col+1>=0 && Col+1<=9 && User_Field[Row+Down][Col+1]!='o') User_Field[Row+Down][Col+1]=Draw;
					Down++;
					if(Row+Down >=0 && Row+Down <=9 && Col+1>=0 && Col+1<=9 && User_Field[Row+Down][Col+1]!='o') User_Field[Row+Down][Col+1]=Draw;
					continue; 
				}else break;
			}
			Down=0;
			while(1) 
			{
				if(User_Field[Row+Down][Col]=='H'||User_Field[Row+Down][Col]=='*'||User_Field[Row+Down][Col]=='!'||User_Field[Row+Down][Col]=='X')
				{
					if(Row+Down >=0 && Row+Down <=9 && Col-1>=0 && Col-1<=9&&User_Field[Row+Down][Col-1]!='o') User_Field[Row+Down][Col-1]=Draw;
					Down++;
					if(Row+Down >=0 && Row+Down <=9 && Col-1>=0 && Col-1<=9&&User_Field[Row+Down][Col-1]!='o') User_Field[Row+Down][Col-1]=Draw;
					continue;
				}else break;
			}
			Down=0;
			while(1) 
			{
				if(User_Field[Row-Up][Col]=='H'||User_Field[Row-Up][Col]=='*'||User_Field[Row-Up][Col]=='!'||User_Field[Row-Up][Col]=='X')
				{
					if(Row-Up >=0 && Row-Up <=9 && Col+1>=0 && Col+1<=9&&User_Field[Row-Up][Col+1]!='o') User_Field[Row-Up][Col+1]=Draw;
					Up++;
					if(Row-Up >=0 && Row-Up <=9 && Col+1>=0 && Col+1<=9&&User_Field[Row-Up][Col+1]!='o') User_Field[Row-Up][Col+1]=Draw;
					continue;
				}else break;
			}
			Up=0;
			while(1) 
			{
				if(User_Field[Row-Up][Col]=='H'||User_Field[Row-Up][Col]=='*'||User_Field[Row-Up][Col]=='!'||User_Field[Row-Up][Col]=='X')
				{
					if(Row-Up >=0 && Row-Up <=9 && Col-1>=0 && Col-1<=9&&User_Field[Row-Up][Col-1]!='o') User_Field[Row-Up][Col-1]=Draw;
					Up++;
					if(Row-Up >=0 && Row-Up <=9 && Col-1>=0 && Col-1<=9&&User_Field[Row-Up][Col-1]!='o') User_Field[Row-Up][Col-1]=Draw;
					continue;
				}else break;
			}
			Up=0;
		}else
		{
		i=0;
			while(User_Field[Row][Col+i]=='H'||User_Field[Row][Col+i]=='*'||User_Field[Row][Col+i]=='!'||User_Field[Row][Col+i]=='X')
		{
			i++;
			continue;
		}
		if(Row >=0 && Row <=9 && Col+i>=0 && Col+i<=9&&User_Field[Row][Col+i]!='o') User_Field[Row][Col+i]=Draw;
		i=0;
		while(User_Field[Row][Col+i]=='H'||User_Field[Row][Col+i]=='*'||User_Field[Row][Col+i]=='!'||User_Field[Row][Col+i]=='X')
		{
			i--;
			continue;
		}
		if(Row >=0 && Row <=9 && Col+i>=0 && Col+i<=9&&User_Field[Row][Col+i]!='o') User_Field[Row][Col+i]=Draw;
		
			while(1) 
			{
				if(User_Field[Row][Col-Left]=='H'||User_Field[Row][Col-Left]=='*'||User_Field[Row][Col-Left]=='!'||User_Field[Row][Col-Left]=='X')
				{
					if(Row+1 >=0 && Row+1 <=9 && Col-Left>=0 && Col-Left<=9&&User_Field[Row+1][Col-Left]!='o') User_Field[Row+1][Col-Left]=Draw;
					Left++;
					if(Row+1 >=0 && Row+1 <=9 && Col-Left>=0 && Col-Left<=9&&User_Field[Row+1][Col-Left]!='o') User_Field[Row+1][Col-Left]=Draw;
					continue;
				}else break;
			}
			Left=0;
			while(1) 
			{
				if(User_Field[Row][Col-Right]=='H'||User_Field[Row][Col-Right]=='*'||User_Field[Row][Col-Right]=='!'||User_Field[Row][Col-Right]=='X')
				{
					if(Row-1 >=0 && Row-1 <=9 && Col-Right>=0 && Col-Right<=9&&User_Field[Row-1][Col-Right]!='o') User_Field[Row-1][Col-Right]=Draw;
					Right++;
					if(Row-1 >=0 && Row-1 <=9 && Col-Right>=0 && Col-Right<=9&&User_Field[Row-1][Col-Right]!='o') User_Field[Row-1][Col-Right]=Draw;
					continue;
				}else break;
			}
			Right=0;
			while(1) 
			{
				if(User_Field[Row][Col+Left]=='H'||User_Field[Row][Col+Left]=='*'||User_Field[Row][Col+Left]=='!'||User_Field[Row][Col+Left]=='X')
				{
					if(Row+1 >=0 && Row+1 <=9 && Col+Left>=0 && Col+Left<=9&&User_Field[Row+1][Col+Left]!='o') User_Field[Row+1][Col+Left]=Draw;
					Left++;
					if(Row+1 >=0 && Row+1 <=9 && Col+Left>=0 && Col+Left<=9&&User_Field[Row+1][Col+Left]!='o') User_Field[Row+1][Col+Left]=Draw;
					continue;
				}else break;
			}
			Left=0;
			while(1) 
			{
				if(User_Field[Row][Col+Right]=='H'||User_Field[Row][Col+Right]=='*'||User_Field[Row][Col+Right]=='!'||User_Field[Row][Col+Right]=='X')
				{
					if(Row-1 >=0 && Row-1 <=9 && Col+Right>=0 && Col+Right<=9&&User_Field[Row-1][Col+Right]!='o') User_Field[Row-1][Col+Right]=Draw;
					Right++;
					if(Row-1 >=0 && Row-1 <=9 && Col+Right>=0 && Col+Right<=9&&User_Field[Row-1][Col+Right]!='o') User_Field[Row-1][Col+Right]=Draw;
					continue;
				}else break;
			}
			Right=0;
		}
}

int Spawn_One_Ship(char User_Field[Size_Field][Size_Field],int What_Boat)
{
	int Row=0;
	int Col;
	char s[2];
	printf("Entered: ");
	gets(s);
	Col=s[0]-65;
	Row=s[1]-48;
			
			if (Row<=9&&Row>=0&&Col<=9&&Col>=0) 
			{	
					if(Check_The_Field(User_Field,Row,Col,What_Boat)==1 && What_Boat!=1) 
					{
						Spawn_Full_Board(User_Field,Row,Col,What_Boat);
						Around_Point(User_Field,Row,Col,What_Boat,'.');
						Hint_Clear_Field(User_Field);
						
						
					}else if(Check_The_Field(User_Field,Row,Col,What_Boat)==2) 
					{
						User_Field[Row][Col]='H';
						if(What_Boat!=1) 
						{
							Move_Hint(User_Field,Row,Col,What_Boat);
						}else 
						{
							Around_Point(User_Field,Row,Col,What_Boat,'.');
						}
						
					}else if(Check_The_Field(User_Field,Row,Col,What_Boat)==0)
					{
					 printf("You cant put it here\n");
					return 0;
					}
			}else
			{	
				printf("You entered it incorrectly\n Example A0 or G4\n");
				return 0;
			}
	return 1;
}
void Spawn_Boat(char User_Field[Size_Field][Size_Field])
{
	printf("        Растановка Кораблей");
	sleep(1);
	int i=0;
	int What_Boat=4;
	while(i!=16){
			Show_Game_Field_User(User_Field); 
			if(i<2)
			{
			printf("4-х палубный корабль: \n");
				if (Spawn_One_Ship(User_Field,What_Boat))
				{
					i++; 
					continue; 
				}else continue;
			}
			if(i<6)
			{
				printf("3-х палубный корабль:\n");
				What_Boat=3;
				if (Spawn_One_Ship(User_Field,What_Boat))
				{
					i++;
					continue;
				}else 
				{
					continue;
				}
			}
			if(i<12)
			{
				printf("2-х палубный корабль:\n");
			
				What_Boat=2;
				if (Spawn_One_Ship(User_Field,What_Boat))
				{
					i++;
					continue;
				}else continue;
			}
			if(i<16)
			{
			
				printf("1-х палубный корабль:\n");
				What_Boat=1;
				if (Spawn_One_Ship(User_Field,What_Boat))
				{
					i++;
					continue;
				}else continue;
			}
	}
}

void Spawn_Enemy_Boat(char Enemy_Field[Size_Field][Size_Field])
{
	int key;
	int Col,Row,What_Boat,Num;
	srand(time(NULL));
	for(int i=0;i<1;i++)
	{
		key=1;
		What_Boat=4;
		do{
			Col=rand()%9;
			Row=rand()%9;
			printf("\nCoord:(%i,%i)\n",Row,Col);//
		}while((Enemy_Field[Row][Col]=='H') || (Enemy_Field[Row][Col]=='.') );
		Enemy_Field[Row][Col]='H';
		Move_Hint(Enemy_Field,Row,Col,What_Boat);
		while(key){
			Num=rand()%3;
			if(Num==0 && Enemy_Field[Row-1][Col]=='^'&&Row-4>=0) 
			{
			Spawn_Full_Board(Enemy_Field,Row-1,Col,What_Boat);
			Around_Point(Enemy_Field,Row-1,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			
			if(Num==1 && Enemy_Field[Row][Col+1]=='>'&& Col+4<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col+1,What_Boat);
			Around_Point(Enemy_Field,Row,Col+1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==2 && Enemy_Field[Row+1][Col]=='v'&&Row+4<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row+1,Col,What_Boat);
			Around_Point(Enemy_Field,Row,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==3 && Enemy_Field[Row][Col-1]=='<'&&Col-4>=0) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col-1,What_Boat);
			Around_Point(Enemy_Field,Row,Col-1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
		}
}
	Show_Game_Field_test(Enemy_Field);
	for(int i=0;i<2;i++)
	{
		key=1;
		What_Boat=3;
		do{
			Col=rand()%9;
			Row=rand()%9;
			printf("\nCoord:(%i,%i)\n",Row,Col);//
		}while((Enemy_Field[Row][Col]=='H') || (Enemy_Field[Row][Col]=='.'));
		Enemy_Field[Row][Col]='H';
		Move_Hint(Enemy_Field,Row,Col,What_Boat);
		while(key){
			Num=rand()%3;
			if(Num==0 && Enemy_Field[Row-1][Col]=='^'&&Row-3>=0)  
			{
			Spawn_Full_Board(Enemy_Field,Row-1,Col,What_Boat);
			Around_Point(Enemy_Field,Row-1,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			
			if(Num==1 && Enemy_Field[Row][Col+1]=='>'&&Col+3<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col+1,What_Boat);
			Around_Point(Enemy_Field,Row,Col+1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==2 && Enemy_Field[Row+1][Col]=='v'&&Row+3<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row+1,Col,What_Boat);
			Around_Point(Enemy_Field,Row,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==3 && Enemy_Field[Row][Col-1]=='<'&&Col-3>=0) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col-1,What_Boat);
			Around_Point(Enemy_Field,Row,Col-1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
		}
		
		
	}
	Show_Game_Field_test(Enemy_Field);
	for(int i=0;i<3;i++)				
	{
		key=1;
		What_Boat=2;
		do{
			Col=rand()%9;
			Row=rand()%9;
			printf("\nCoord:(%i,%i)\n",Row,Col);  //
		}while((Enemy_Field[Row][Col]=='H') || (Enemy_Field[Row][Col]=='.'));
		Enemy_Field[Row][Col]='H';
		Move_Hint(Enemy_Field,Row,Col,What_Boat);
		while(key){							
			Num=rand()%3;
			if(Num==0 && Enemy_Field[Row-1][Col]=='^'&&Row-2>=0) 
			{
			Spawn_Full_Board(Enemy_Field,Row-1,Col,What_Boat);
			Around_Point(Enemy_Field,Row-1,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			
			if(Num==1 && Enemy_Field[Row][Col+1]=='>'&&Col+2<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col+1,What_Boat);
			Around_Point(Enemy_Field,Row,Col+1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==2 && Enemy_Field[Row+1][Col]=='v'&&Row+2<=9) 
			{
			Spawn_Full_Board(Enemy_Field,Row+1,Col,What_Boat);
			Around_Point(Enemy_Field,Row,Col,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
			if(Num==3 && Enemy_Field[Row][Col-1]=='<'&&Col-2>=0) 
			{
			Spawn_Full_Board(Enemy_Field,Row,Col-1,What_Boat);
			Around_Point(Enemy_Field,Row,Col-1,What_Boat,'.');
			Hint_Clear_Field(Enemy_Field);
			key=0;
			}
		}
		
		
	}		
	Show_Game_Field_test(Enemy_Field);
	for(int i=0;i<4;i++)
	{
		key=1;
		What_Boat=1;
		do{
			Col=rand()%9;
			Row=rand()%9;
			printf("\nCoord:(%i,%i)\n",Row,Col);//
		}while((Enemy_Field[Row][Col]=='H') || (Enemy_Field[Row][Col]=='.') );
		Enemy_Field[Row][Col]='H';
		Around_Point(Enemy_Field,Row,Col,What_Boat,'.');
	}	
	Show_Game_Field_test(Enemy_Field);
}

int User_Move(char User_Field[Size_Field][Size_Field],char Enemy_Field[Size_Field][Size_Field],char Hidden_Field[Size_Field][Size_Field])
{
	int Col,Row,i=1,j=1,What_Boat;
	char s[2];
	while(1)
	{
		printf("Enter a strike: ");
		gets(s);
		Col=s[0]-65;
		Row=s[1]-48;
		if (Row<=9&&Row>=0&&Col<=9&&Col>=0&&Enemy_Field[Row][Col]!='X'&&Enemy_Field[Row][Col]!='o'&&Enemy_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='X'&&Hidden_Field[Row][Col]!='o'&&Hidden_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='.')
		{
			if(Enemy_Field[Row][Col]=='H') 
			{
				if(Enemy_Field[Row+1][Col]=='H' || Enemy_Field[Row-1][Col]=='H' || Enemy_Field[Row][Col+1]=='H' || Enemy_Field[Row][Col-1]=='H') //ПРОВЕРКА НА БОЛЕЕ ЧЕМ ОДНУ ПАЛУБУ
				{
					Hidden_Field[Row][Col]='*';
					if(Hidden_Field[Row+1][Col]=='*' || Hidden_Field[Row-1][Col]=='*')
					{
						while(Enemy_Field[Row+i][Col]!='.'&&Row+i<=9)
						{
							if(Enemy_Field[Row+i][Col]=='H'&& Hidden_Field[Row+i][Col]=='*') 
							{
								i++;
							}
							else return 1;  
						}
						
						while(Enemy_Field[Row-j][Col]!='.'&&Row-j>=0)
						{
							if(Enemy_Field[Row-j][Col]=='H'&& Hidden_Field[Row-j][Col]=='*') j++;
							else return 1; 
						}
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row+k][Col]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row-k][Col]='X';
						return 1;												
					}
					i=1;
					j=1;
					if(Hidden_Field[Row][Col+1]=='*' || Hidden_Field[Row][Col-1]=='*')
					{
						while(Enemy_Field[Row][Col+i]!='.'&&Col+i<=9)
						{
							if(Enemy_Field[Row][Col+i]=='H'&& Hidden_Field[Row][Col+i]=='*') i++;
							else return 1;
						}
						while(Enemy_Field[Row][Col-j]!='.'&&Col-j>=0)
						{
							if(Enemy_Field[Row][Col-j]=='H'&& Hidden_Field[Row][Col-j]=='*') j++;
							else return 1;
						}
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row][Col+k]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row][Col-k]='X';
						return 1;
					}
					return 1;
				}else 
				{
					Hidden_Field[Row][Col]='X';
					Around_Point(Hidden_Field,Row,Col,1,'.');
					return 1;
				}
				
			}else 
			{
				Hidden_Field[Row][Col]='o';
				return 0;
			}
		}else 
		{	
			printf("You entered error\n");
		}
	}
}

int Exclamation_Mark(char User_Field[Size_Field][Size_Field])
{
	int What_Boat=1,i,j,Row,Col,key=0;
	for(i=0;i<Size_Field;i++)
	{
		for(j=0;j<Size_Field;j++)
		{ 
			if(User_Field[i][j]=='!') 
				{
					key=1;
					What_Boat++;
				}
		}
	}
	for(i=0;i<Size_Field;i++)
	{
		for(j=0;j<Size_Field;j++)
		{ 
			
			if(User_Field[i][j]=='!') 
			{
				User_Field[i][j]='X';
				Row=i;
				Col=j;
			}
		}
	}
if(key)
	{	
		Around_Point(User_Field,Row,Col,What_Boat,'.');
		return What_Boat-1;
	}
	return 0;
}

int Create_Exclamation(char User_Field[Size_Field][Size_Field],int Row,int Col)
{
	int i=1,j=0;
	while(User_Field[Row-i][Col]=='H'&&Row-i>=0)
	{
		User_Field[Row-i][Col]='!';
		i++;
		j++;
	}
	i=1;
	while(User_Field[Row+i][Col]=='H'&&Row+i<=9)
	{
		User_Field[Row+i][Col]='!';
		i++;
		j++;
	}
	i=1;
	while(User_Field[Row][Col+i]=='H'&&Col<=9)
	{
		User_Field[Row][Col+i]='!';
		i++;
		j++;
	}
	i=1;
	while(User_Field[Row][Col-i]=='H'&&Col>=0)
	{
		User_Field[Row][Col-i]='!';
		i++;
		j++;
	}
	return j;
}

int CPU_Move(char User_Field[Size_Field][Size_Field])
{
	if(Exclamation_Mark(User_Field)) return 1;
	int Col,Row;
	int Chek,k=0,p=0;
	while(1)
	{
	srand(time(NULL));
	int *Empty_Cell=(int*)malloc(100-20);
	{
		for(int i=0;i<k;i++)  Empty_Cell[i]=0;
	}
		for(int i=0;i<Size_Field;i++)
	{
		for(int j=0;j<Size_Field;j++)
		{
			if(User_Field[i][j]==' '||User_Field[i][j]=='H')
			{
				Empty_Cell[k]=i+1;
				Empty_Cell[k+1]=j+1;
				k+=2;
			}
		}
	}
	p=k-1;
	k=rand() % p;
	if(k%2==0)
	{
		Row=Empty_Cell[k];
		Col=Empty_Cell[k+1];
		Row-=1;
		Col-=1;
	}else 
	{
		Col=Empty_Cell[k];
		Row=Empty_Cell[k-1];
		Row-=1;
		Col-=1;
	}
	free(Empty_Cell);
	if(User_Field[Row][Col]=='H')
	{
		User_Field[Row][Col]='X';
		Create_Exclamation(User_Field,Row,Col);
		if(User_Field[Row+1][Col]=='H'||User_Field[Row-1][Col]=='H'||User_Field[Row][Col+1]=='H'||User_Field[Row][Col-1]=='H')
		{
			while(1)
			{
				Chek=rand()%3;
				if(Chek==0)
				{
					if(Row-1>=0)
					{
						if(User_Field[Row-1][Col]==' '|| User_Field[Row-1][Col]=='o') 	
						{
							User_Field[Row-1][Col]='o';
							return 0;
						}
						if(User_Field[Row-1][Col]=='!') 
						{
							User_Field[Row-1][Col]='X';
							return 1;
						}
					}else continue;
				} 
				if(Chek==1)
				{	
					if(Col+1<=9)
					{
					if(User_Field[Row][Col+1]==' '||User_Field[Row][Col+1]=='o') 
					{
						User_Field[Row][Col+1]='o';
						return 0;
					}
					if(User_Field[Row][Col+1]=='!') 
					{
						User_Field[Row][Col+1]='X';
						return 1;
					}
				}else continue;
			}
				if(Chek==2)
				{
					if(Row+1<=9)
					{
					if(User_Field[Row+1][Col]==' '||User_Field[Row+1][Col]=='o') 
					{
						User_Field[Row+1][Col]='o';
						return 0;
					}
					if(User_Field[Row+1][Col]=='!') 
					{
						User_Field[Row+1][Col]='X';
						return 1;
					}
					}else continue;
				}
				if(Chek==3)
				{
					if(Col-1>=0)
					{
					if(User_Field[Row][Col-1]==' '||User_Field[Row][Col-1]=='o') 
					{
						User_Field[Row][Col-1]='o';
						return 0;
					}
					if(User_Field[Row][Col-1]=='!') 
					{
						User_Field[Row][Col-1]='X';
						return 1;
					}
					}else continue;
				}
			}
		}else 
		{
			Around_Point(User_Field,Row,Col,1,'.');
			return 1;   
			
		}
	}else if(User_Field[Row][Col]=='o') continue;
	else
	{
		User_Field[Row][Col]='o';
		return 0;
	}
	}
}

void Game_Show(char User_Field[Size_Field][Size_Field],char Hidden_Field[Size_Field][Size_Field],int Hit_User,int Hit_CPU,int Motion,int player_move)
{
	printf("Ход №%d\n",Motion);
	printf("Попадания: \n");
	printf("Игрок: %d\n",Hit_User);
	printf("Противник: %d\n",Hit_CPU);
	if(player_move==0)     printf("  @@@@@@@--> Противник <--@@@@@@                      Игрок \n");
	else if(player_move==1)printf("    	     Противник                    @@@@@@@@--> Игрок <--@@@@@@@ \n");
	printf("  |A||B||C||D||E||F||G||H||I||J|         |A||B||C||D||E||F||G||H||I||J|\n");
	for(int i=0;i<Size_Field;i++)
	{
		printf("%d-",i);
		for(int j=0;j<Size_Field;j++)
		{
		printf("|%c|",Hidden_Field[i][j]);
		}
		printf("         ");                                         
		for(int j=0;j<Size_Field;j++)
		{
			
		printf("|%c|",User_Field[i][j]);
		}
		printf("\n");
	} 
	
}

void Start_Game(char User_Field[Size_Field][Size_Field],char Enemy_Field[Size_Field][Size_Field],char Hidden_Field[Size_Field][Size_Field])
{
	srand(time(NULL));
	int player_move,Hit_User=0,Hit_CPU=0,Boat=0,Motion=1;
	printf("Жеребьёвка: \n");
	printf("\n........... \n");
	player_move=rand() % 2;
	sleep(3);
	if(player_move) printf("Первый ход противника\n");
	else printf("Первый ход игрока\n");
	while((Hit_User!=20 || Hit_CPU==20)&&(Hit_User==20 || Hit_CPU!=20))
	{
		Game_Show(User_Field,Enemy_Field,Hit_User,Hit_CPU,Motion,player_move);
		Game_Show(User_Field,Hidden_Field,Hit_User,Hit_CPU,Motion,player_move);
		sleep(2);
		Motion++;
		if(player_move ==0) 
		{
			if(User_Move(User_Field,Enemy_Field,Hidden_Field) ==1) 
			{
				player_move=0;
				Hit_User++;
			}
		else 
		{
			player_move=1;
		}
		}
		if(player_move ==1) 
		{
			Boat=0;
			if(CPU_Move(User_Field)) 
			{
				Boat=CPU_Move(User_Field);
				player_move=1;
				Hit_CPU+=Boat;
			}
			else 
			{
				player_move=0;
			}
		}
	}
	
	if (Hit_User==20) 
	{
	printf("\n\n\n@@@@@@@@    You Won!!!    @@@@@@@@\n\n\n");
	Game_Show(User_Field,Hidden_Field,Hit_User,Hit_CPU,Motion,2);
	
	}
	if (Hit_CPU==20) 
	{
	printf("\n\n\n@@@@@@@@    You lost(((    @@@@@@@@\n\n\n\n");
	Game_Show(User_Field,Hidden_Field,Hit_User,Hit_CPU,Motion,2);
	}
}

void Solo_Games_Mode()
{
	char User_Field[Size_Field][Size_Field];
	char Enemy_Field[Size_Field][Size_Field];
	char Hidden_Field[Size_Field][Size_Field];
	printf("     	  Одиночная игра\n");
	Clear_Field(Hidden_Field);
	Clear_Field(User_Field);
	Clear_Field(Enemy_Field);
	Spawn_Boat(User_Field);
	Clear_User_Field_On_Start_Game(User_Field);
	Spawn_Enemy_Boat(Enemy_Field);
	Start_Game(User_Field,Enemy_Field,Hidden_Field);
}

int Socket(int domain,int type,int protocol);

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Inet_pton(int af, const char *src,void *dst);

int Field_End(char Field[Size_Field][Size_Field])
{
	int Ret=0;
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(Field[i][j]=='X'||Field[i][j]=='*') Ret++;
		}
	}
		return Ret;
}



void *Start_Coop(void *data_input)
{
	struct data *input=(struct data *)data_input;
	char s[2];
	int Col,Row,What_Boat,i=1,j=1,k=0;
	char User_Field[Size_Field][Size_Field];
	char Enemy_Field[Size_Field][Size_Field];
	char Hidden_Field[Size_Field][Size_Field];
	char player_move;
	Clear_Field(Hidden_Field);
	Clear_Field(User_Field);
	Clear_Field(Enemy_Field);
	printf("Жеребьёвка: \n");
	printf("\nОжидание игрока... \n");
	read(input->sock,&player_move,sizeof(char));
	printf("Move: %c\n",player_move);
	int Your_Hit,Enemy_Hit,move=0,popal,exit=0;
	int Send_Coord[40];
	if(player_move =='1') 
	{
		printf("Вы ходите первым\n");
		Spawn_Boat(User_Field);
		Clear_Field(Hidden_Field);
		send(input->sock,User_Field,sizeof(char)*100,0);
		Clear_User_Field_On_Start_Game(User_Field);
		printf("Ожидание второго игрока\n");
		recv(input->sock,Enemy_Field,sizeof(char)*100,0);
		while(((Enemy_Hit=Field_End(User_Field))!=20)&&((Your_Hit=Field_End(Hidden_Field))!=20))
		{
			k=0;
			popal=1;
			move++;
	while(popal)
	{
		exit=0;
		i=1;
		j=1;
		Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,1);
		printf("Введите координаты выстрела: ");
		gets(s);
		Col=s[0]-65;
		Row=s[1]-48;
		if (Row<=9&&Row>=0&&Col<=9&&Col>=0&&Enemy_Field[Row][Col]!='X'&&Enemy_Field[Row][Col]!='o'&&Enemy_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='X'&&Hidden_Field[Row][Col]!='o'&&Hidden_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='.')
		{
		Send_Coord[k]=Row;
		Send_Coord[k+1]=Col;
		k+=2;
			if(Enemy_Field[Row][Col]=='H') 
			{
				popal=1;
				Your_Hit++;
				if(Enemy_Field[Row+1][Col]=='H' || Enemy_Field[Row-1][Col]=='H' || Enemy_Field[Row][Col+1]=='H' || Enemy_Field[Row][Col-1]=='H') //ПРОВЕРКА НА БОЛЕЕ ЧЕМ ОДНУ ПАЛУБУ
				{
					Hidden_Field[Row][Col]='*';
					if(Hidden_Field[Row+1][Col]=='*' || Hidden_Field[Row-1][Col]=='*')
					{
						while(Enemy_Field[Row+i][Col]!='.'&&Row+i<=9)
						{
							if(Enemy_Field[Row+i][Col]=='H'&& Hidden_Field[Row+i][Col]=='*') 
							{
								i++;
							}
							else 
							{	
								exit=1;							  
								break;
							}
						}
						if(exit==1) continue;
						
						while(Enemy_Field[Row-j][Col]!='.'&&Row-j>=0)
						{
							if(Enemy_Field[Row-j][Col]=='H'&& Hidden_Field[Row-j][Col]=='*') 
							{
								j++;
							}
							else
							{ 
								exit=1; 
								break;
							}
						}
						if(exit==1) continue;
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row+k][Col]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row-k][Col]='X';
						continue;												
					}
					i=1;
					j=1;
					if(Hidden_Field[Row][Col+1]=='*' || Hidden_Field[Row][Col-1]=='*')
					{
						while(Enemy_Field[Row][Col+i]!='.'&&Col+i<=9)
						{
							if(Enemy_Field[Row][Col+i]=='H'&& Hidden_Field[Row][Col+i]=='*') i++;
							else 
							{
								exit=1;
								break;
							}
						}
						if(exit==1) continue;
						while(Enemy_Field[Row][Col-j]!='.'&&Col-j>=0)
						{
							if(Enemy_Field[Row][Col-j]=='H'&& Hidden_Field[Row][Col-j]=='*') j++;
							else 
							{
								exit=1;
								break;
							}
						}
						if(exit==1) continue;
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row][Col+k]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row][Col-k]='X';
						continue;
					}
					continue;
				}else 
				{
					Hidden_Field[Row][Col]='X';
					Around_Point(Hidden_Field,Row,Col,1,'.');
					continue;
				}
				
			}else 
			{
				Hidden_Field[Row][Col]='o';
				Send_Coord[k]=10;
				popal=0;
				break;
			}
		}else 
		{	
			printf("Вы ввели неправильные координатыб пример: A4 или J9\n");
			continue;
		}
	}
			
			move++;
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,0);
			printf("Ожидание хода противника...\n");
			send(input->sock,Send_Coord,sizeof(int)*40,0);    
			recv(input->sock,Send_Coord,sizeof(int)*40,0);
			k=0;
			while(Send_Coord[k]!=10)
			{
				User_Field[Send_Coord[k]][Send_Coord[k+1]]='X';
				k+=2;
				Enemy_Hit++;
			}
				User_Field[Send_Coord[k-2]][Send_Coord[k-1]]='o';
		}
		if(Enemy_Hit==20)
		{
			printf("\n\n\n@@@@@@@@    You lost(((    @@@@@@@@\n\n\n\n");
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,2);
		}
		
		if(Your_Hit==20)
		{
			printf("\n\n\n@@@@@@@@    You Won!!!    @@@@@@@@\n\n\n");
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,2);
		}
		
	}
	else if(player_move =='0') 
	{
		printf("Вы ходите вторым\n");
		Spawn_Boat(User_Field);
		Clear_Field(Hidden_Field);
		printf("Ожидание второго игрока\n");
		recv(input->sock,Enemy_Field,sizeof(char)*100,0);
		Game_Show(User_Field,Enemy_Field,Your_Hit,Enemy_Hit,move,0);
		send(input->sock,User_Field,sizeof(char)*100,0);
		Clear_User_Field_On_Start_Game(User_Field);
		while(((Enemy_Hit=Field_End(User_Field))!=20)&&((Your_Hit=Field_End(Hidden_Field))!=20))
		{
			move++;
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,0);
			printf("Ожидание хода противника...\n");
			recv(input->sock,Send_Coord,sizeof(int)*40,0);
			k=0;
			while(Send_Coord[k]!=10)
			{
				User_Field[Send_Coord[k]][Send_Coord[k+1]]='X';
				k+=2;
				Enemy_Hit++;
			}
				User_Field[Send_Coord[k-2]][Send_Coord[k-1]]='o';

			move++;
			Enemy_Hit=Field_End(User_Field);
			Your_Hit=Field_End(Hidden_Field);
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,1);  
			popal=1;
			k=0;
		while(popal)
	{
		exit=0;
		i=1;
		j=1;
		Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,1);
		printf("Введите координаты выстрела: ");
		gets(s);
		Col=s[0]-65;
		Row=s[1]-48;
		if (Row<=9&&Row>=0&&Col<=9&&Col>=0&&Enemy_Field[Row][Col]!='X'&&Enemy_Field[Row][Col]!='o'&&Enemy_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='X'&&Hidden_Field[Row][Col]!='o'&&Hidden_Field[Row][Col]!='*'&&Hidden_Field[Row][Col]!='.')
		{
		Send_Coord[k]=Row;
		Send_Coord[k+1]=Col;
		k+=2;
			if(Enemy_Field[Row][Col]=='H') 
			{
				popal=1;
				Your_Hit++;
				if(Enemy_Field[Row+1][Col]=='H' || Enemy_Field[Row-1][Col]=='H' || Enemy_Field[Row][Col+1]=='H' || Enemy_Field[Row][Col-1]=='H') //ПРОВЕРКА НА БОЛЕЕ ЧЕМ ОДНУ ПАЛУБУ
				{
					Hidden_Field[Row][Col]='*';
					if(Hidden_Field[Row+1][Col]=='*' || Hidden_Field[Row-1][Col]=='*')
					{
						while(Enemy_Field[Row+i][Col]!='.'&&Row+i<=9)
						{
							if(Enemy_Field[Row+i][Col]=='H'&& Hidden_Field[Row+i][Col]=='*') 
							{
								i++;
							}
							else 
							{	
								exit=1;							  
								break;
							}
						}
						if(exit==1) continue;
						
						while(Enemy_Field[Row-j][Col]!='.'&&Row-j>=0)
						{
							if(Enemy_Field[Row-j][Col]=='H'&& Hidden_Field[Row-j][Col]=='*') 
							{
								j++;
							}
							else
							{ 
								exit=1; 
								break;
							}
						}
						if(exit==1) continue;
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row+k][Col]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row-k][Col]='X';
						continue;												
					}
					i=1;
					j=1;
					if(Hidden_Field[Row][Col+1]=='*' || Hidden_Field[Row][Col-1]=='*')
					{
						while(Enemy_Field[Row][Col+i]!='.'&&Col+i<=9)
						{
							if(Enemy_Field[Row][Col+i]=='H'&& Hidden_Field[Row][Col+i]=='*') i++;
							else 
							{
								exit=1;
								break;
							}
						}
						if(exit==1) continue;
						while(Enemy_Field[Row][Col-j]!='.'&&Col-j>=0)
						{
							if(Enemy_Field[Row][Col-j]=='H'&& Hidden_Field[Row][Col-j]=='*') j++;
							else 
							{
								exit=1;
								break;
							}
						}
						if(exit==1) continue;
						What_Boat=i+j;
						Around_Point(Hidden_Field,Row,Col,What_Boat,'.');
						for(int k=0;k<i;k++) Hidden_Field[Row][Col+k]='X';
						for(int k=0;k<j;k++) Hidden_Field[Row][Col-k]='X';
						continue;
					}
					continue;
				}else 
				{
					Hidden_Field[Row][Col]='X';
					Around_Point(Hidden_Field,Row,Col,1,'.');
					continue;
				}
				
			}else 
			{
				Hidden_Field[Row][Col]='o';
				Send_Coord[k]=10;
				popal=0;
				break;
			}
		}else 
		{	
			printf("Вы ввели неправильные координатыб пример: A4 или J9\n");
			continue;
		}
	}
			Game_Show(User_Field,Enemy_Field,Your_Hit,Enemy_Hit,move,0);
			printf("Ожидание хода противника...\n");
			send(input->sock,Send_Coord,sizeof(int)*40,0); 
		}	
		if(Enemy_Hit==20)
		{
			printf("\n\n\n@@@@@@@@    You lost(((    @@@@@@@@\n\n\n\n");
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,2);
		}
		
		if(Your_Hit==20)
		{
			printf("\n\n\n@@@@@@@@    You Won!!!    @@@@@@@@\n\n\n");
			Game_Show(User_Field,Hidden_Field,Your_Hit,Enemy_Hit,move,2);
		}
	}
	printf("End send");
	return (void *)(0);
}

void Multiplayer_Games_Mode(int argv,char *argc[])
{
	pthread_t Thread_Wr,Thread_Re;
	printf("     	  Сетевая Игра\n");
	if (argv != 2)
	 {
		 perror("No Address given");
		 exit(EXIT_FAILURE);
	 }
	 int border=strstr(argc[1],":")-argc[1];
	 if(border+argc[1]==NULL)
	 {
		 perror("Not correct format adress");
		 exit(EXIT_FAILURE);
	 }
	
		char ip[border+1];
		strncpy(ip,argc[1],border);
		ip[border]='\0';
		int port = atoi(argc[1]+border+1);
	
	 int sock = Socket(AF_INET,SOCK_STREAM,0);
	 struct sockaddr_in adr = {0};
	 adr.sin_family=AF_INET;
	 adr.sin_port = htons(port);
	 Inet_pton(AF_INET,ip,&adr.sin_addr);
	 Connect(sock,(struct sockaddr *)&adr,sizeof adr); 
	 
	 struct data data_Thread;
	 data_Thread.sock=sock;
	
	 pthread_create(&Thread_Wr,NULL,Start_Coop,(void *)&data_Thread);
	 pthread_join(Thread_Wr,NULL);
	 
	 close(sock);
}

int Select_Game_mode()
{
	int Enter_Game_mode;
	printf("@@@@@@@@    Морской Бой    @@@@@@@@\n Введите режим игры:\n Нажмите (0): Сетевая игра \n Нажмите (1): Одиночная игра\n Ввод:");
	scanf("%d",&Enter_Game_mode);
	return Enter_Game_mode;
}

int main(int argv,char *argc[])
{
	setlocale(LC_ALL,"rus");
	int Game_Mode,Error=1;
	while(Error)
	{
		Game_Mode = Select_Game_mode();
		if(Game_Mode == 1) Solo_Games_Mode();
		else if(Game_Mode == 0) Multiplayer_Games_Mode(argv,argc);
		else 
		{ 
			printf("You entered the wrong number\n");
			continue;
		}
	}
	
	return 0;
}  

int Socket(int domain,int type,int protocol)
{
	 int res = socket(domain,type,protocol);
	 if (res == -1){
		 printf("socket failed");
		 exit(EXIT_FAILURE);
	 }
	 return res;
}
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int res = bind(sockfd,addr,addrlen);
	if(res == -1){
		printf("Bind failed");
		exit(EXIT_FAILURE);
	}
}


void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int res = connect(sockfd,addr,addrlen);

	if (res == -1)
	{
		printf("Connect faild");
		exit(EXIT_FAILURE);
	}printf("Подключение успешно\n");
	
}

void Inet_pton(int af, const char *src,void *dst)
{
	int res = inet_pton(af,src,dst);
	if (res == 0){
		printf("inet_pton faild, valid network addres in the specified addres family\n");
		exit(EXIT_FAILURE);
	}
	if (res == -1)
	{
		printf("Inet_pton faild");
		exit(EXIT_FAILURE);
	}
}
