# ACPI = Advanced Configuration and Power Interface
C'est une spécification qui permet notament de communiquer avec le firmware pour gérer:
- l'alimentation
- l'arrêt
- le redḿarage
- la veille
- certains péripheriques et informations matérielles
Firmware <=> ACPI <=> Kernel

## Table

### RSDP = Root System Description Pointer
Il permet de trouver les table ACPI.
Le RSDP peut etre vue comme un paneau qui indique où commencer à chercher.

### RSDT = Root System Description Table
Une table qui contient des adresses vers d'autre table

### XSDT = Extended System Description Table
la même chose que RSDT mais qui utilise des adresses 64bit
`8bit = 0b00000000 = 0x00` et `64bit = 0b0000000000000000000000000000000000000000000000000000000000000000 = 0x0000000000000000`

### ACPI table
Une table ACPI est une structure de donnée fournie par le firmware

### FADT = Fixed ACPI Description Table
Contien des information sur les registres de gestion de puissance

### DSDT = Differentiated System Description Table
C'est **une table ACPI** qui contient une description de la machine et du code **ACPI**, notamment sous forme d'**AML**.

## PM = Power Management
Gestion de l'alimentation

S5 = etain
S3 = veille
S0 = ordinateur allumé

## AML = ACPI Machine Language
C'est le language utiliser dans les table ACPI
_S5 = Instruction pour entrer dans cette etat S5

## Adresse

### Adresse physique
Une adresse physique designe directement un emplacement dans l'espace mémoire physique de la machine

### MMIO = Memory-Mapped I/O
Certains péripheriques sont accesibles comme s'ils étaient de la memoire.
