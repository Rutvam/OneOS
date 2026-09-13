#include "../data/global_value.h"
#include "../core/function/function.h"

void main_shutdown (void)
{
	// Recherche de la signature en hex 52 53 44 20 50 54 52 20
	my.clear();
	var.cursor_position = 0;
	my.print.basic("[ ... ] exec shutdown\n");
	var.status.problem = "ERROR: Funktion shutdown not finich\n";
	var.status.value = 1;
	if (var.status.value == 1)
	{
		var.cursor_position = 0;
		my.print.basic("[ X ] exec shutdown  \n");
		my.print.basic(var.status.problem);
		return;
	}
}
