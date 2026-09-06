#include "../core/value/global_value.h"
#include "../core/function/function.h"


int my_memcmp(const void *a, const uint8_t *b, int n)
{
	const unsigned char *p1 = a;
	const unsigned char *p2 = b;

	for (int i = 0; i < n; i += 16)
	{
		if (p1[i] != p2[i]){return 1;}
	}
	
	return 0;
}

struct RSDP {
	char signature[8];
	uint8_t cheksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_adress;

	uint32_t length;
	uint64_t xsdt_adress;
	uint8_t extended_checksum;
	uint8_t reserved[3];
};

uint32_t find_RSDP(void)
{
	cursor.position = 160;
	print("[ ... ] chercher RSDP!\n");
	uint8_t signature[8] = {0x52, 0x53, 0x44, 0x20, 0x50, 0x54, 0x52, 0x20};
	for (uint32_t adresse = 0x000E0000; adresse < 0x00100000; adresse += 16)
	{
		if (my_memcmp((void*)adresse, signature, 8) == 0)
		{
			cursor.position = 160;
			print("[ OK ] RSDP trouver!\n");
			return adresse;
		}
	}
	cursor.position = 160;
	print("[ X ] RSDP trouver!\n");
	return 0xFFFFFFFF;
}

void main_shutdown (void)
{
	// Recherche de la signature en hex 52 53 44 20 50 54 52 20
	clear();
	cursor.position = 0;
	print("[ ... ] exec shutdown");
	uint32_t adresse = find_RSDP();
	if (adresse == 0xFFFFFFFF)
	{
		cursor.position = 0;
		print("[ X ] exec shutdown");
		return;
	}
	struct RSDP *rsdp = (struct RSDP*)adresse;
}
