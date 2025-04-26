#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "errors.h"


static const char* DEFAULT_OUTPUT = "graf.out";


// parses argument that isn't flag - doesn't start with '-'
static void parseArg(Arguments* args, char** argv, int i, int* argNum);

// parses flag - argv[i][0] should equal '-'
static void parseFlag(Arguments* args, int argc, char** argv, int* i);

// shows help message and exits with 0
static void showHelp(void);

// shows error message and exit because of invalid user input
static void argError(const char* message, int line);


Arguments* arguments_parse(int argc, char** argv)
{
	// initialize array to default arguments
	Arguments* args = malloc(sizeof(Arguments));
	args->inputFile = NULL;
	args->outputFile = DEFAULT_OUTPUT;
	args->divisions = 1;
	args->maxDiff = 10;
	args->useBinaryMode = false;

	// check for "--help" or "-h"
	if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
	{
		if (argc != 2) { argError("Podano dodatkowe argumenty po prośbie o pomoc!", __LINE__); }
		showHelp(); // it calls exit, so it ends here
	}

	// loop over argv
	int argNum = 0;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') { parseFlag(args, argc, argv, &i); }
		else { parseArg(args, argv, i, &argNum); }
	}

	return args;
}

void parseArg(Arguments* args, char** argv, int i, int* argNum)
{
	switch (*argNum)
	{
		case 0:
			args->inputFile = argv[i];
			break;

		case 1:
			args->divisions = strtoll(argv[i], NULL, 10);
			if (args->divisions < 0) { argError("Podana liczba podziałów jest ujemna!", __LINE__); }
			break;

		case 2:
			args->maxDiff = strtod(argv[i], NULL);
			if (args->maxDiff < 0.0) { argError("Podana różnica procentowa jest ujemna!", __LINE__); }
			break;

		default:
			argError("Podano za dużo argumentów!", __LINE__);
	}

	(*argNum)++;
}

void parseFlag(Arguments* args, int argc, char** argv, int* i)
{
	char flag = argv[*i][1];

	switch (flag)
	{
		case 'o':
			if ((*i) + 1 >= argc)
			{
				argError("Podano flagę -o bez podania nazwy pliku wyjściowego!", __LINE__);
			}
			else if (args->outputFile != DEFAULT_OUTPUT)
			{
				argError((args->outputFile != NULL) ? "Próba użycia -o dwukrotnie!" : "Próba użycia -o i -t jednocześnie!", __LINE__);
			}

			args->outputFile = argv[(*i) + 1];
			(*i)++;
			break;

		case 't':
			if (args->outputFile != DEFAULT_OUTPUT)
			{
				argError((args->outputFile == NULL) ? "Próba użycia -t dwukrotnie!" : "Próba użycia -o i -t jednocześnie!", __LINE__);
			}

			args->outputFile = NULL;
			break;

		case 'b':
			if (args->useBinaryMode) { argError("Próba użycia -b dwukrotnie!", __LINE__); }
			args->useBinaryMode = true;
			break;

		case 'm':
			if ((*i) + 1 >= argc) { argError("Podano flagę -m bez podania numeru metody!", __LINE__); }
			else if (args->method != 0) { argError("Próba użycia -m dwukrotnie!", __LINE__); }

			args->method = strtoll(argv[(*i) + 1], NULL, 10);
			if (args->method < 1 || args->method > 3) { argError("Podano niepoprawny identyfikator metody przy fladze -m!", __LINE__); }
			(*i)++;
			break;

		case 's':
			if ((*i) + 1 >= argc) { argError("Podano flagę -s bez podania nasienia!", __LINE__); }
			else if (args->definedSeed) { argError("Próba użycia -s dwukrotnie!", __LINE__); }

			args->definedSeed = true;
			args->seed = strtoll(argv[(*i) + 1], NULL, 10);
			(*i)++;
			break;

		case 'i':
			if ((*i) + 1 >= argc) { argError("Podano flagę -i bez podania ilości iteracji!", __LINE__); }
			else if (args->iterations > 0) { argError("Próba użycia -i dwukrotnie!", __LINE__); }

			args->iterations = strtoll(argv[(*i) + 1], NULL, 10);
			if (args->iterations < 1) { argError("Podano niepoprawną ilość iteracji przy fladze -i!", __LINE__); }
			(*i)++;
			break;

		default:
			argError("Próba użycia nieznanej flagi!", __LINE__);
	}

	// check if both seed is defined and method is set to simple method (2 or 3)
	if (args->definedSeed && args->method > 1)
	{
		argError("Zdefiniowano nasienie przy jednoczesnym określeniu prostej metody cięcia", __LINE__);
	}
	else if (args->iterations > 0 && args->method > 1) // same check but for iteration count
	{
		argError("Zdefiniowano ilość iteracji przy jednoczesnym określeniu prostej metody cięcia", __LINE__);
	}
}

void showHelp(void)
{
	puts("\nArgumenty:");
	puts(" Pierwszy argument - nazwa pliku zawierający graf zapisany w formacie tekstowym. Format tego argumentu to napis.");
	puts(" Drugi argument -  ile razy ma zostać wykonanane dzielenie grafu."
		 " W przypadku podania wartości 0, graf wejściowy zostanie podany na wyjście bez zmian."
		 " Formatem tego argumentu jest liczba naturalna wraz z zerem. (opcjonalne, domyślnie 1)");
	puts(" Trzeci argument -  o jaki maksymalny margines wyrażony w procentach mają różnić się ilości wierzchołków w podzielonych."
		 " Formatem tego argumentu jest liczba nieujemna (nie musi być całkowita) zapisana w formie dziesiętnej. (opcjonalne, domyślnie 10)");

	puts("\nFlagi:");
	puts(" -o plik.out - argument zawierający ścieżkę do pliku wyjściowego, do którego ma zostać zapisany graf wyjściowy."
		  " Domyślnie ustawiona jest flaga -o z plikiem wyjściowym \"graf.out\", chyba że pojawi się flaga -t.");
	puts(" -t - flaga określająca, że graf wyjściowy ma być wypisany w terminalu.");
	puts(" -b - flaga zmieniająca format wypisywania grafu z tekstowego na binarny.");
	puts(" -m - flaga określa metodę (algorytm) dzielenia grafu - dostępne metody zostały opisane w następnej sekcji.");
	puts(" -s - flaga określa nasienie (seed) funkcji losującej dla algorytmu Kargera. Domyślnie nasienie jest pobierane z systemowego zegara.");
	puts(" -i - flaga określa ilość iteracji dla algorytmu Kargera. Domyślnie wartość wynosi co najmniej 50"
		 " i jest obliczana ze wzoru [50 * (n/10)^2] gdzie n jest liczbą wierzchołków."
		 " Wartość argumentu podana przez użytkownika nie może być mniejsza od 1.");

	puts("\nMetody dzielenia:");
	puts(" 1 - Algorytm Kargera - metoda ta wykorzystuje algorytm Kargera, który jest algorytmem probabilistycznym,"
		 " co oznacza że nie ma stuprocentowej szansy na powodzenie. Prawdopodobieństwo na poprawny wynik przy pojedyńczym wykonaniu"
		 " jest znikome i spada wraz z większymi grafami, dlatego domyślna ilość iteracji jest spora i rośnie wraz z ilością"
		 " wierzchołków w grafie. Użytkownik może zdefiniować własną ilość iteracji jeżeli zależy mu na lepszym wyniku lub przeciwnie,"
		 " na większej wydajności.");
	puts(" 2 lub 3 - Prosty algorytm - jest to prosty algorytm brute force sprawdzający każdą możliwą kombinację wierzchołków."
		 " Dzięki temu zagwarantowany jest najlepsze możliwe rozwiązanie, lecz kosztem bardzo słabej wydajności."
		 " Złożoność tej metody to O(2^n) gdzie n to ilość wierzchołków w grafie. Metoda ta ma dwa warianty."
		 " Przy wariancie 2 będzie ona także sprawdzała czy grafy po podzieleniu są spójne i nie dokona podziału jeżeli takowe"
		 " nie są (podobnie jak algorytm Kargera). Przy wariancie 3 nie będzie dokonywała takiego sprawdzenia przez co wynikiem"
		 " podziału mogą być więcej niż 2 grafy. Po podziale grafy te będą traktowane jako oddzielne spójne grafy, a nie jako"
		 " jeden niespójny (na którym podział nie byłby możliwy, gdyż sam w sobie byłby podzielony).");

	exit(0);
}

void argError(const char* message, int line)
{
	error("Błąd wczytywania argumentów", message, "arguments.c", line);
}
