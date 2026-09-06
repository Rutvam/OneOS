#include <elf.h>
#include <efi.h>
#include <efilib.h>
#include <string.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_BOOT_SERVICES *BS = SystemTable->BootServices;

	EFI_LOADED_IMAGE *LoadedImage = NULL;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
	EFI_FILE_PROTOCOL *Root = NULL;
	EFI_FILE_PROTOCOL *KernelFile = NULL;

	EFI_PHYSICAL_ADDRESS KernelAddress;
	UINTN Pages;

	EFI_FILE_INFO *FileInfo = NULL;
	VOID *KernelBuffer = NULL;

	EFI_STATUS status;
	UINTN FileInfoSize = 0;
	UINTN KernelSize;
	UINTN ReadSize;


	InitializeLib(ImageHandle, SystemTable);


	/* 1. Image UEFI */

	Print(L"[1/8] Chargement de l'image...\r\n");
	status = uefi_call_wrapper(BS->OpenProtocol, 6, ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	if (EFI_ERROR(status))
	{
		Print(L"      ERREUR : LoadedImage : %r\r\n", status);
		return status;
	}
	
	Print(L"      OK\r\n");

	/* 2. Systeme de fichiers */
	Print(L"[2/8] Acces au systeme de fichiers...\r\n");
	status = uefi_call_wrapper(BS->HandleProtocol, 3, LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystem);
	if (EFI_ERROR(status))
	{
		Print(L"      ERREUR : Filesystem : %r\r\n", status);
		return status;
	}

	Print(L"      OK\r\n");

    /* 3. Volume racine */
    Print(L"[3/8] Ouverture du volume...\r\n");
    status = uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Root);
    if (EFI_ERROR(status))
    {
        Print(L"      ERREUR : Volume : %r\r\n", status);
        return status;
    }

    Print(L"      OK\r\n");

    /* 4. Kernel */
    Print(L"[4/8] Ouverture de kernel.elf...\r\n");
    status = uefi_call_wrapper(Root->Open, 5, Root, &KernelFile, L"kernel.elf", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        Print(L"      ERREUR : kernel.elf : %r\r\n", status);
        return status;
    }

    Print(L"      OK\r\n");

    /* 5. Informations du fichier */
    Print(L"[5/8] Lecture des informations...\r\n");
    status = uefi_call_wrapper(KernelFile->GetInfo, 4, KernelFile, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL)
    {
        Print(L"      ERREUR : GetInfo : %r\r\n", status);
        return status;
    }

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, FileInfoSize, (VOID **)&FileInfo);
    if (EFI_ERROR(status))
    {
        Print(L"      ERREUR : Allocation : %r\r\n", status);
        return status;
    }

    status = uefi_call_wrapper(KernelFile->GetInfo, 4, KernelFile, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR(status)) {
        Print(L"      ERREUR : GetInfo : %r\r\n", status);
        uefi_call_wrapper(BS->FreePool, 1, FileInfo);
        return status;
    }

    KernelSize = FileInfo->FileSize;
    Print(L"      Taille : %lu octets\r\n", KernelSize);
    uefi_call_wrapper(BS->FreePool, 1, FileInfo);
    FileInfo = NULL;

    /* 6. Allocation RAM */
    Print(L"[6/8] Allocation de %lu octets...\r\n", KernelSize);
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, KernelSize, &KernelBuffer);
    if (EFI_ERROR(status)) {
        Print(L"      ERREUR : RAM : %r\r\n", status);
        return status;
    }

    Print(L"      OK\r\n");

    /* 7. Lecture du kernel */
    Print(L"[7/8] Chargement du kernel...\r\n");
    ReadSize = KernelSize;
    status = uefi_call_wrapper(KernelFile->Read, 3, KernelFile, &ReadSize, KernelBuffer);
    if (EFI_ERROR(status)) {
        Print(L"      ERREUR : Lecture : %r\r\n", status);
        uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);
        return status;
    }

    if (ReadSize != KernelSize) {
        Print(L"      ERREUR : Lecture incomplete (%lu/%lu)\r\n", ReadSize, KernelSize);
        uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);
        return EFI_LOAD_ERROR;
    }
	Print(L"      OK : %lu octets charges en RAM\r\n", ReadSize);


    Print(L"[8/8] Verification du kernel...\r\n");

	Elf64_Ehdr *Header = (Elf64_Ehdr *)KernelBuffer;

	if (Header->e_ident[EI_MAG0] != ELFMAG0 || Header->e_ident[EI_MAG1] != ELFMAG1 || Header->e_ident[EI_MAG2] != ELFMAG2 || Header->e_ident[EI_MAG3] != ELFMAG3)
	{
		Print(L"      ERREUR : fichier ELF invalide\r\n");
		uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);
		return EFI_LOAD_ERROR;
	}

	if (Header->e_ident[EI_CLASS] != ELFCLASS64)
	{
		Print(L"      ERREUR : ELF non 64 bits\r\n");
		uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);
		return EFI_LOAD_ERROR;
	}

	if (Header->e_machine != EM_X86_64)
	{
		Print(L"      ERREUR : architecture non x86-64\r\n");
		uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);
		return EFI_LOAD_ERROR;
	}

	Print(L"      ELF64 x86-64 valide !\r\n");

	Print(L"\r\n");
	Print(L"--- ELF ---\r\n");
	
	Print(L"Entry      : 0x%lx\r\n", Header->e_entry);
	Print(L"PH offset  : 0x%lx\r\n", Header->e_phoff);
	Print(L"PH size    : %u\r\n", Header->e_phentsize);
	Print(L"PH count   : %u\r\n", Header->e_phnum);
	
	Elf64_Phdr *ProgramHeaders =
	    (Elf64_Phdr *)((UINT8 *)KernelBuffer + Header->e_phoff);
	
	for (UINTN i = 0; i < Header->e_phnum; i++)
	{
	    Elf64_Phdr *ph = &ProgramHeaders[i];
	
	    if (ph->p_type == PT_LOAD)
		{
	    	Print(L"\r\nPT_LOAD %lu\r\n", i);
	
	    	Print(L"  Offset : 0x%lx\r\n", ph->p_offset);
	    	Print(L"  Vaddr  : 0x%lx\r\n", ph->p_vaddr);
	    	Print(L"  Filesz : 0x%lx\r\n", ph->p_filesz);
	    	Print(L"  Memsz  : 0x%lx\r\n", ph->p_memsz);
	
	    	/*
	    	 * Nombre de pages necessaires
	    	 */
	    	Pages = (ph->p_memsz + 0xFFF) / 0x1000;
	
	    	KernelAddress = ph->p_vaddr;
	
	    	Print(L"  Pages  : %lu\r\n", Pages);
	    	Print(L"  Adresse demandee : 0x%lx\r\n", KernelAddress);
	
	    	/*
	    	 * Reservation de la memoire du segment
	    	 */
	    	status = uefi_call_wrapper(
	    	    BS->AllocatePages,
	    	    4,
	    	    AllocateAddress,
	    	    EfiLoaderData,
	    	    Pages,
	    	    &KernelAddress
	    	);

		    if (EFI_ERROR(status))
		    {
	    	    Print(L"  ERREUR : AllocatePages : %r\r\n", status);

		        uefi_call_wrapper(BS->FreePool, 1, KernelBuffer);

		        return status;
		    }
	
	    	Print(L"  OK : memoire kernel reservee\r\n");
	
	    	/*
	    	 * Copie de la partie presente dans le fichier ELF
	    	 */
	    	Print(L"  Copie du segment...\r\n");
	
	    	CopyMem(
	    	    (VOID *)(UINTN)ph->p_vaddr,
	    	    (UINT8 *)KernelBuffer + ph->p_offset,
	    	    ph->p_filesz
	    	);
	
	    	Print(L"  OK : %lu octets copies\r\n", ph->p_filesz);
	
	    	/*
	    	 * Le reste du segment correspond notamment au .bss.
	    	 * Il doit etre initialise a zero.
	    	 */
	    	if (ph->p_memsz > ph->p_filesz)
	    	{
	    	    UINTN BssSize = ph->p_memsz - ph->p_filesz;
		
		        Print(L"  Initialisation BSS : %lu octets\r\n", BssSize);
	
		        SetMem(
		            (UINT8 *)(UINTN)(ph->p_vaddr + ph->p_filesz),
		            BssSize,
		            0
		        );
	
		        Print(L"  OK : BSS initialise\r\n");
		    }
		}
	}

	Print(L"Passage au kernel\r\n");

	UINTN MemoryMapSize = 0;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;

	Print(L"GetMemoryMap ...\r\n");

	status = uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, NULL, &MapKey, &DescriptorSize, &DescriptorVersion);

	if (status != EFI_BUFFER_TOO_SMALL)
	{
		Print(L"ERREUR GetMemoryMap : %r\r\n", status);
		return status;
	}

	Print(L"allocation MemoryMap ...\r\n");

	MemoryMapSize += 2 * DescriptorSize;

	status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MemoryMapSize, (VOID **)&MemoryMap);

	if (EFI_ERROR(status))
	{
		Print(L"ERREUR allocation MemoryMap : %r\r\n", status);
		return status;
	}

	Print(L"\r\n");

	status = uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
	
	if (EFI_ERROR(status))
	{
	    Print(L"ERREUR GetMemoryMap : %r\r\n", status);
	    return status;
	}


	Print(L"Warning: Les fonction UEFI sera desactiver dans un instant pour donner le control entier au kernel");
	status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, MapKey);
	
	if (EFI_ERROR(status))
	{
	    // Ici il faut récupérer une nouvelle MapKey et réessayer.
	    // Mais surtout : ne pas continuer comme si la transition avait réussi.
	}


	typedef void (*KernelEntry)(void);
	KernelEntry Entry = (KernelEntry)(UINTN)Header->e_entry;
	Entry();	

	while (1) {
		__asm__ volatile ("hlt");
	}

	return EFI_SUCCESS;
}
