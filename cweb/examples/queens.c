/*1:*/
#line 6 "./queens.w"

#include <stdio.h> 
#include <stdlib.h> 
/*3:*/
#line 30 "./queens.w"

int param;

/*:3*/
#line 9 "./queens.w"

/*5:*/
#line 46 "./queens.w"

char encode(x)
int x;
{
if(x<10)return'0'+x;
return'a'-10+x;
}

/*:5*/
#line 10 "./queens.w"
;

main(argc,argv)
int argc;
char*argv[];
{
register int j,k,n,nn,t;
/*2:*/
#line 22 "./queens.w"

if(argc!=2||sscanf(argv[1],"%d",&param)!=1){
fprintf(stderr,"Usage: %s n\n",argv[0]);
exit(-1);
}
n= param;
nn= n+n-2;

/*:2*/
#line 17 "./queens.w"
;
/*4:*/
#line 37 "./queens.w"

for(j= 0;j<n;j++){
t= (j&1?n-1-j:n+j)>>1;
printf("r%c c%c ",encode(t),encode(t));
}
printf("|");
for(j= 1;j<nn;j++)printf(" a%c b%c",encode(j),encode(j));
printf("\n");

/*:4*/
#line 18 "./queens.w"
;
/*6:*/
#line 54 "./queens.w"

for(j= 0;j<n;j++)for(k= 0;k<n;k++){
printf("r%c c%c",encode(j),encode(k));
t= j+k;
if(t&&(t<nn))printf(" a%c",encode(t));
t= n-1-j+k;
if(t&&(t<nn))printf(" b%c",encode(t));
printf("\n");
}

/*:6*/
#line 19 "./queens.w"
;
}

/*:1*/
