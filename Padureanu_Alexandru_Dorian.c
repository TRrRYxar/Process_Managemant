#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int startVar = 0;
static int nrProc = 0;
static int planfVar = 1;



typedef struct proces {
	int idProc;		// Numar proces
	int lifeProc;	// procentul de finalizare
	struct proces* next;
}proces;

typedef struct waitingQ
{
	proces* head;
	proces* tail;
}waitingQ;

typedef struct run
{
	int durata;
	int	quant;
	int dMIN;
	int dMAX;
	int rMIN; 
	int rMAX;
}run;


int sim(struct run simulare);
waitingQ* init_wq();
int planf(waitingQ* wq, proces* runn);			// planificator
proces* make_proc(run simulare);		// crearea proces
int enQ(waitingQ* wq, proces* proc);
int deQ(waitingQ* wq);
int generateRandoms(int lower, int upper);
int nrElements(waitingQ* wq);


int main()
{
	srand(time(0));
	struct run siml;			// Structura care retine toate datele care se pot modifica pentru diferite simulari
	siml.durata = 1000000;		// n
	siml.dMAX = 400;			// delta - MAX viata maxima pe care o poate avea un proces
	siml.dMIN = 300;			// delta - MIN
	siml.quant = 100;			// quanta de timp
	siml.rMAX = 150;				// durata de timp minima pentru creearea unui proces
	siml.rMIN = 100;				// si cea maxima


	waitingQ* waiting;
	proces* running = (proces*)malloc(sizeof(proces));
	waiting = init_wq();
										// variabie pentru a da informatii despre simularile facute
	int arr[10];		// tablou de maxim de elemente pentru a salva numarul maxim de procese din coada de astaptare
	int nrSimulari = 5;			// se efectueaza 5 simulari datorita timpului de asteptare
	float sum = 0;			// suma elementelor tabloului
	float sum1 = 0;			// suma pentru a calcula varianta
	float mean = 0;			// variabila pentru medie
	float variance = 0;		// variabila pentru varianta
	float std_deviation = 0;		// variabila pentru deviatia standard


	for (int i = 0; i < nrSimulari; i++)
	{
		printf("Simularea NR: %d\n", i + 1);
		arr[i] = sim(siml, waiting, running);			// se efectueaza simularile si se slaveaza intr-un tablou de maxim 10 elemente
		sum = sum + arr[i];
		printf("\n");
	}

	printf("Informatii in legatura cu simularile: \n");

	mean = sum / nrSimulari;
	printf("Mean = %.2f\n", mean);				// se calculeaza media
	

	for (int i = 0; i < nrSimulari; i++)
	{
		sum1 = sum1 + pow((arr[i] - mean), 2);
	}

	variance = sum1 / (float)nrSimulari;			// se calculeaza varianta simularilor
	printf("Variance = %.2f\n", variance);

	std_deviation = sqrt(variance);					// se calculeaza deviatia standard
	printf("Standard deviation = %.2f\n", std_deviation);
	
	return 0;
}

int sim(run simulare, waitingQ* wq, proces* runn)
{
	int creationMoment = generateRandoms(simulare.rMIN, simulare.rMAX);
	int maxWQ = 0;					// variabila pentru a retine lungimea maxima a cozii
	int nrProceseTerminate = 0;		// variabila pentru a retine numarul de procese terminate
	int nrProceseCreate = 0;		// variabila pentru a retine numarul de procese create
	startVar = 0;					// se initializeaza variabila de timp pentru fiecare simulare

	while (startVar < simulare.durata)
	{
		if (runn != NULL)		// se verifica daca se afla un proces in starea running
		{
			runn->lifeProc = runn->lifeProc - 1;		// se scade o unitate din viata procesului
		}

		if ((startVar % simulare.quant) == 0)			// verificare planificator
		{
			planf(wq, runn);
		}

		if (startVar == creationMoment)			// se verifica daca trebuie sa se creeze un proces
		{
			enQ(wq, make_proc(simulare));		
			creationMoment = creationMoment + generateRandoms(simulare.rMIN, simulare.rMAX);		// se genereaza urmatorul moment in care o sa se creeze un nou proces
			if (maxWQ < nrElements(wq))
			{													// se salveaza numarul maxim de elemente din coada de asteptare
				maxWQ = nrElements(wq);
			}
			nrProceseCreate = nrProceseCreate + 1;

		}

		if (runn->lifeProc == 0)				// se verifica daca procesul curent este finalizat
		{										// se aplica planificatorul
			nrProceseTerminate = nrProceseTerminate + 1;
			planf(wq, runn);
			planfVar = 1;					// se modifica variabila planVar pentru a intra pe primul if din functia de planificare 
		}									// deoarece procesul prezent nu trebuie introdus in coada de asteptare

		startVar = startVar + 1;			// se incrementeaza variabila de timp cu o unitate

	}

	printf("Numarul maxim de elemente care s-au aflat in coada de asteptare este: %d\n", maxWQ);
	printf("Numarul de procese terminate este: %d\n", nrProceseTerminate);
	printf("Numarul de procese create este: %d\n", nrProceseCreate);

	return maxWQ;
}

waitingQ* init_wq()			// se initializeaza coada de asteptare
{
	waitingQ* wq = (waitingQ*)malloc(sizeof(waitingQ));
	wq->head = NULL;
	wq->tail = NULL;
	return wq;
}

int enQ(waitingQ* wq, proces* proc)				// se adauga un proces in coada de asteptare
{												
	if (wq->tail != NULL)
	{
		wq->tail->next = proc;					// se adauga procesul nou in coada
	}
	wq->tail = proc;							
	if (wq->head == NULL)
	{
		wq->head = proc;						// daca nu sunt procese in coada modificam si capul cozii
	}
	return 0;
}

int deQ(waitingQ* wq)							// functie pentru scoaterea elementelor din coada
{
	if (wq->head == NULL)
	{	
		wq->tail = NULL;			// ma asigur ca nu ramane nimic in tail daca head este NULL
		return 0;
	}
	proces* temp = wq->head;
	wq->head = wq->head->next;
	if (wq->head == NULL)	// daca lista este goala modificam pointerul "tail"
	{
		wq->tail = NULL;
	}
	//free(temp);
	return 1;
}


int generateRandoms(int lower, int upper)				// functie pentru a genera o valoare random dintr-un interval
{
	int randNr = (rand() % (upper - lower + 1)) + lower;
	return randNr;
}


int planf(waitingQ* wq, proces* runn)					// functia de planificare care se ocupa de schimbarea sau interschimbarea proceselor  
{													// care au starea running si waiting

	if (wq->head != NULL)			// daca nu exista proces in asteptare functia nu are efect
	{			// verific daca se afla vrun proces in starea running
		if (planfVar == 1)				// pentru planfVar = 1  inseamna ca nu avem un proces in starea running
		{
			*runn = *wq->head;
			runn->next = NULL;
			deQ(wq);						// dupa schimbarea starii scot procesul din lista de asteptare
			planfVar = planfVar + 1;		// modific variabila planfVar pentru a retine ca avem un proces in running
			return 1;
		}
		if (planfVar > 1)				// pentru planfVar > 1 inseamna ca avem deja un proces in starea running si ca acesta nu este finalizat
		{
			proces* temp = (proces*)malloc(sizeof(proces));			// variabila temporara pentru a realiza interschimbarea

			temp->idProc = runn->idProc;				// se salveaza procesul din running pentru a fi adaugat in lista de asteptare
			temp->lifeProc = runn->lifeProc;
			temp->next = NULL;
			*runn = *wq->head;			// adaug noul proces din lista de asteptare in running
			runn->next = NULL;
			deQ(wq);			// scot procesul curent cu starea running din lista de asteptare
			enQ(wq, temp);		// adaug procesul nefinalizat in lista de asteptare
			return 1;
		}
	}

}

proces* make_proc(run simulare)				// functie de creeare proces
{
	nrProc = nrProc + 1;
	proces* newProces = malloc(sizeof(proces));				
	if (newProces == NULL)
	{
		return NULL;
	}
	newProces->idProc = nrProc;					// numarul procesului este dat de o variabila globala statica care creste la fiecare nou proces creeat
	newProces->lifeProc = generateRandoms(simulare.dMIN, simulare.dMAX);	// viata noului proces este o valoare aleatoare dintr-un interval de doua valori
	newProces->next = NULL;
	return newProces;
}

int nrElements(waitingQ* wq)			// functie pentru a calcula numarul de elemente din lista
{

	int counter = 0;
	if (wq->head == NULL)
	{
		return counter;			// daca lista este goala numarul de elemente returnat este 0
	}
	proces* temp = wq->head;
	while (temp != NULL)
	{
		temp = temp->next;
		counter = counter + 1;		// se parcurge lista de la inceput
	}
	return counter;			// se returneaza numarul de elemente din lista
}