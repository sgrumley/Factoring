#define 	SLOT_SIZE 	10
#define  	WAITING  	0
#define  	FILLED     1
#define  	TAKEN      -1
#define 	QUERY_FIN -2


struct Memory {
	int  status;
	int  data[4];
};
