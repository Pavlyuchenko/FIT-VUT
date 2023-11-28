#include <stdio.h>
#include <stdlib.h>

/**
 * Datovy typ pro vektor celych cisel, slozky:
 * pole celych cisel nezname delky
 * aktualni pocet prvku vektru
 */
typedef struct
{
    int *arr;
    unsigned int size;
} vector_t;

/// @brief Konstruktor datoveho typu vector_t.
/// @return Ukazatel na alokovany vektor. Pole celych cisel bude ukazovat na NULL, velikost bude 0.
vector_t *vector_ctor()
{
    // alokace pameti pro vektor_t
    vector_t *v = malloc(sizeof(vector_t));

    // pokud se podarila, nastaveni vychozich hodnot
    if (v != NULL)
    {
        v->arr = malloc(0 * sizeof(int));
        v->size = 0;
    }

    return v;
}

/// @brief Funkce provede realokaci pameti pro vektor celych cisel.
/// @param arr Pole, pro ktere budeme provadet realokaci
/// @param new_size Velikost, na kterou budeme chtit pole zvetsit.
/// @return Ukazatel na zvetsene pole, nebo NULL, pokud se zmena velikosti nepodarila.
int *resize(int *arr, unsigned int new_size)
{
    // ukazatel na nove vytvorene pole (pouzije funkci realloc)
    int *new_arr = realloc(arr, new_size * sizeof(int));

    // pokud se alokace nepodarila
    // uvolnete nove vytvorene pole z pameti
    if (new_arr == NULL)
    {
        free(new_arr);
        return NULL;
    }

    return new_arr;
}
/// @brief Funkce prida do vektoru vec jeden prvek o hodnote value.
/// @param vec Vektor, do ktereho budeme pridavat prvky.
/// @param value Hodnota noveho prvku.
/// @return 1, pokud se vlozeni povedlo, 0 jinak
int vector_add(vector_t *vec, int value)
{
    // realokace vektoru tak, aby se do nej veslo o jeden prvek vic
    // pouzijte funkci resize
    resize(vec->arr, vec->size + 1);

    // pokud se alokace nepodarila, funkce vrati 0
    if (vec->arr == NULL)
    {
        return 0;
    }

    // pokud se alokace podarila
    // value se vlozi na posledni pozici ve vektoru
    // velikost se inkrementuje o 1
    vec->size++;
    vec->arr[vec->size - 1] = value;

    return 1;
}

/// @brief Destruktor pro vektor. Uvolni pamet pro jeho prvky a pote pro vektor.
/// @param v Vektor, ktery budeme uvolnovat.
void vector_dtor(vector_t *v)
{
    free(v->arr);
    v->size = 0;
    free(v);
}

/// @brief Funkce vypise obsah vektoru na stdout.
/// @param v Vektor, jehoz obsah se bude vypisovat.
void print_vector(vector_t *v)
{
    printf("Vektor o velikosti %d:\n\t (  ", v->size);
    for (int i = 0; i < v->size; i++)
    {
        printf("%d  ", v->arr[i]);
    }
    printf(")\n");
}

int main()
{
    vector_t *v1 = vector_ctor();

    int num;

    while (scanf("%d", &num) == 1)
    {
        vector_add(v1, num);
    }

    print_vector(v1);
    vector_dtor(v1);
    return 0;
}