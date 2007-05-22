#include <iostream>
#include <fstream>
#include "variables.h"
#include "init32_64.h"

using namespace std;

int main(int argc, char *argv[])
{
  ifstream fin;
  if (argc==1){
    cout<<"Укажите файл .gnv"<<endl;
    return 1;
  }
  else{
    fin.open(argv[1]);
    if (!fin){
      cout<<"Нет такого файла"<<endl;
      return 1;
    }
  }

  Variables vars;
  //-----подсчет переменных
  int i=0;
  char s[161840],c='0';
  while ( c!=';' ){
  	fin>>c;
	if (c==',' || c==';') {
		vars.add(s);
		s[0]='\0';
		i=0;
	}
	else {
		s[i]=c;
		i++;
		s[i]='\0';
	}
  }
  //-----конец подсчета
  fin.close();

  int bytes = (vars.dim()-1)/32;

  //switch (bytes){
  //  case 0:
      init32_64(argv[1]);
  //    break;
  //}

  return EXIT_SUCCESS;
}

/*
int main(int argc, char *argv[])
{
  ifstream fin("test");

  Variables vars;
  int i=0;
  char s[1024],c='0';
  while ( c!=';' ){
  	fin>>c;
	if (c==',' || c==';') {
		vars.add(s);
		s[0]='\0';
		i=0;
	}
	else {
		s[i]=c;
		i++;
		s[i]='\0';
	}
  }

  MonomInterface32_64 mInterface = MonomInterface32_64(&vars);

  Monom32_64 p1(&mInterface), p2(&mInterface), p3(&mInterface);

  fin>>p1>>p2;
  cout<<p1<<' '<<p2<<' '<<p3<<endl;
  cout<<(p1==p2)<<endl;


  return EXIT_SUCCESS;
}
*/
