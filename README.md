Wstęp do programowania w C. - Projekt zaliczeniowy.
==============

Temat
--------------
Makrogenerator (preprocesor), który rozpoznaje i przetwarza jednowierszowe dyrektywy #include i #define, a w pozostałym tekście rozpoznaje makrowywołania i zastępuje je rozwinięciami makrodefinicji z #define. Uwzględnić należy makrodefinicje z parametrami i bez. Wynikiem programu powninien być przetworzony przez makrogenerator tekst (bez dyrektyw i z rozwiniętymi makrowywołaniami).

Pierwsze użycie
--------------
Zainstaluj program używając polecenia make.
Następnie wpisz w pliku include.txt listę ścieżek do katalogów, w których znajdują się biblioteki. 
Każda z nich powinna być w osobnej linii i zakończona znakiem '/'.

Użytkowanie
--------------
Po uruchomieniu programu podaj nazwę lub ścieżkę pliku wejściowego i wejściowego.

Opis działania i funkcji
--------------

1. Plik preprocesor.c zawiera funkcję main i główną funkcję przepisującą dany plik wejściowy.
2. Pliki define.h i define.c zawierają definicje struktury `_define` oraz funkcji umożliwiających jej obsługę:
   * Funkcje umożliwiające inicjalizaje pojedyńczego elelmentu lub całego drzewa struktury.
   * Funkcje umożliwiające usunięcie pojedyńczego elementu lub całego drzewa struktury.
   * Funkcje umożliwiające stworzenie nowego elementu w drzewie.
   * Funkcje umożliwiające przekształcenie danego tekstu przy pomocy podanych wcześniej definicji.
   * Inne funkcje pomocnicze.
3. Pliki define.h i define.c zawierają definicje struktury `_include` oraz funkcji umożliwiających jej obsługę:
   * Funkcje umożliwiające inicjalizaje pojedyńczego elelmentu lub całego drzewa struktury.
   * Funkcje umożliwiające usunięcie pojedyńczego elementu lub całego drzewa struktury.
   * Funkcje umożliwiające stworzenie nowego elementu w drzewie.
   * Funkcje umożliwiające wywołanie podanego w pliku wejściowym pliku nagłówkowego wywołanie na nim funkcji przepisującej.
   * Inne funkcje pomocnicze.
4. Pliki my_string.h i my_string.c zawierają funkcje pomocnicze ułatwiające obsługę napisów.
5. Pliki errors.h i errors.c zawierają definicje funkcji komunikatów błedów.
   


Autor
--------------
Krzysztof Bednarek