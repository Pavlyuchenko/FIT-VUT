#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initial string content
const char *str_init = "Hello WORLD";

// Find first substring occurrence in a string and return its starting position.
// Return -1 if the string does not contain substring.
int find_substr(char *str, char *substr)
{
    int substr_index = 0;
    int start_substr = -1;
    for (int i = 0; i < strlen(str); i++)
    {
        if (substr_index == strlen(substr) - 1)
        {
            return start_substr;
        }
        if (str[i] == substr[substr_index])
        {
            if (substr_index == 0)
            {
                start_substr = i;
            }
            substr_index++;
        }
        else
        {
            start_substr = -1;
            substr_index = 0;
        }
    }

    return start_substr;
}

// Replace same-length substrings in a string.
void replace_same_length(char *str, char *substr, char *new_substr)
{

    // pokud neni delka obou retezcu stejna
    if (strlen(substr) != strlen(new_substr))
    {
        return;
    }

    // pouze jeden retezec nahradime za druhy (znak po znaku)
    int start_sub = find_substr(str, substr);

    for (int i = 0; i < strlen(substr); i++)
    {
        str[i + start_sub] = new_substr[i];
    }
}

// Replace substring in a string with any new substring.
// Returns new string with replaced substring or NULL in case of any error.
char *replace(char *str, char *substr, char *new_substr)
{

    /// zacatek podretezce
    int start_sub = find_substr(str, substr);
    if (start_sub == -1)
    {
        return NULL;
    }

    /// delky (budeme potrebovat delku retezce a podretezcu)
    int delka_str = strlen(str);

    /// jaky je rozdil mezi delkami podretezcu?
    int rozdil_delek = strlen(substr) - strlen(new_substr);
    int nova_delka = delka_str - strlen(substr) + strlen(new_substr);

    /**
     * Dve moznosti:
     * novy podretezec je kratsi nez puvodni
     * novy podretezec je delsi nez puvodni
     *
     * Pro obe moznosti je potreba upravit pozice znaku v retezci tak,
     * abychom bud odstranili prebytecne znaky, nebo pridali misto na dalsi.
     */

    char *new_str = realloc(str, nova_delka * sizeof(char) + 1);
    if (rozdil_delek == 0)
    {
        replace_same_length(str, substr, new_substr);
        return str;
    }
    else if (strlen(new_substr) < strlen(substr))
    {

        for (int i = 0; i < strlen(new_substr); i++)
        {
            new_str[i + start_sub] = new_substr[i];
        }

        int after_substr = start_sub + strlen(substr);
        for (int i = after_substr; i < strlen(str); i++)
        {
            new_str[i - rozdil_delek] = str[i];
        }
    }
    else if (strlen(new_substr) > strlen(substr))
    {
        int after_str = start_sub + strlen(substr);
        char *endstr = malloc((strlen(str) - after_str) * sizeof(char));
        for (int i = after_str; i < strlen(str); i++)
        {
            endstr[i - after_str] = str[i];
        }

        for (int i = start_sub + strlen(new_substr); i < nova_delka; i++)
        {
            new_str[i] = endstr[i - start_sub - strlen(new_substr)];
        }

        for (int i = 0; i < strlen(new_substr); i++)
        {
            new_str[i + start_sub] = new_substr[i];
        }
    }
    new_str[nova_delka] = '\0';

    return new_str;
}

int main()
{
    // Allocate new string.

    char *str = (char *)malloc(strlen(str_init) + 1);

    if (str == NULL)
    {
        return 1;
    }

    // Set the src by copying src_init.
    strcpy(str, str_init);

    // Replace substring with a new same-length substring.
    replace_same_length(str, "WORLD", "World");

    printf("%s\n", str);

    // Replace substring with a new shorter substring.
    str = replace(str, "World", "IZP");

    if (str == NULL)
    {
        return 1;
    }
    printf("%s\n", str);

    // Replace substring with a new longer substring.
    str = replace(str, "IZP", "llalalalall");
    if (str == NULL)
    {
        return 1;
    }
    printf("%s\n", str);
    // Cleanup
    free(str);
    printf("Successully replaced all substrings!\n");

    // Allocate and initialize a new string.
    str = (char *)malloc(strlen(str_init) + 1);
    if (str == NULL)
    {
        return 1;
    }

    strcpy(str, str_init);

    // Try using replace with substring that is not in the string.
    str = replace(str, "worlds", "World!");
    if (str == NULL)
    {
        free(str);
        return 1;
    }
    printf("%s\n", str);
    return 0;
}