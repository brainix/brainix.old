/*----------------------------------------------------------------------------*\
 |	elf.c								      |
 |									      |
 |	Copyright © 2002-2006, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#include <fs/fs.h>

#define SHDR(img, ehdr, sec_num) \
	((Elf32_Shdr *) (img + ehdr->e_shoff + ehdr->e_shentsize * sec_num))

#define REL_SIZE(shdr) \
	(shdr->sh_type == SHT_RELA ? sizeof(Elf32_Rela) : sizeof(Elf32_Rel))

#define REL_ADDR(img, shdr, rel_size, rel_num) \
	((Elf32_Rela *) (img + shdr->sh_offset + rel_size * rel_num))

/*----------------------------------------------------------------------------*\
 |				  elf_verify()				      |
\*----------------------------------------------------------------------------*/
int elf_verify(char *img)
{

/* Given the memory image of a file, verify that it is executable by Brainix.
 * Return 0 on success, or a negative error number. */

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) img;

	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
	    ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
	    ehdr->e_ident[EI_MAG3] != ELFMAG3)
		/* Not ELF. */
		err_code = ENOEXEC;

	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32)
		/* Not 32-bit. */
		err_code = ENOEXEC;

	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
		/* Not little endian. */
		err_code = ENOEXEC;

	if (ehdr->e_type != ET_REL)
		/* Not relocatable. */
		err_code = ENOEXEC;

	if (ehdr->e_machine != EM_386)
		/* Not for IA-32 (x86). */
		err_code = ENOEXEC;

	if (ehdr->e_version != EV_CURRENT)
		/* Invalid version. */
		err_code = ENOEXEC;

	return -err_code;
}

/*----------------------------------------------------------------------------*\
 |				   elf_val()				      |
\*----------------------------------------------------------------------------*/
int elf_val(char *img, Elf32_Word sec_num, unsigned char sym_num,
	Elf32_Addr *sym_val)
{

/* Given the memory image of an ELF file, a section number, and a symbol number,
 * find the specified symbol's value.  Return 0 on success, or a negative error
 * number. */

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) img;
	Elf32_Shdr *shdr = SHDR(img, ehdr, sec_num);
	Elf32_Sym *sym = (Elf32_Sym *) (img + shdr->sh_offset) + sym_num;
	Elf32_Word shndx = sym->st_shndx == 0 ? shdr->sh_link : sym->st_shndx;
	Elf32_Addr addr;
	Elf32_Word *name;

	shdr = SHDR(img, ehdr, shndx);
	addr = (Elf32_Addr) img + shdr->sh_offset;

	if (sym->st_shndx == 0)
	{
		/* External symbol. */
		name = (Elf32_Word *) ((char *) addr + sym->st_name);
		err_code = ENOEXEC;
	}
	else
		/* Internal symbol. */
		*sym_val = addr + sym->st_value;

	return -err_code;
}

/*----------------------------------------------------------------------------*\
 |				   elf_rel()				      |
\*----------------------------------------------------------------------------*/
int elf_rel(char *img, Elf32_Shdr *shdr_1, Elf32_Rela *rela)
{

/* Given the memory image of an ELF file, a section header, and a relocation
 * entry, perform the specified relocation.  Return 0 on success, or a negative
 * error number. */

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) img;
	Elf32_Shdr *shdr_2 = SHDR(img, ehdr, shdr_1->sh_info);
	Elf32_Addr addr = (Elf32_Addr) img + shdr_2->sh_offset;
	Elf32_Addr *p = (Elf32_Addr *) (addr + rela->r_offset);
	Elf32_Word sec_num = shdr_1->sh_link;
	unsigned char sym_num = ELF32_R_SYM(rela->r_info);
	Elf32_Addr sym_val;

	elf_val(img, sec_num, sym_num, &sym_val);
	if (err_code)
		return -err_code;

	switch (ELF32_R_TYPE(rela->r_info))
	{
		case R_386_32   : *p = sym_val + *p;                  break;
		case R_386_PC32 : *p = sym_val + *p - (Elf32_Addr) p; break;
		default         : err_code = ENOEXEC;
	}

	return -err_code;
}

/*----------------------------------------------------------------------------*\
 |				   elf_load()				      |
\*----------------------------------------------------------------------------*/
int elf_load(char *img, Elf32_Addr *ent)
{

/*
 | Given the memory image of an ELF file, prepare it for execution:
 |	· Allocate memory for the BSS,
 |	· Perform relocations,
 |	· Find the entry point.
 | Return 0 on success, or a negative error number.
 */

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) img;
	Elf32_Half sec_num;
	Elf32_Shdr *shdr;
	char *bss;
	char rel_size;
	Elf32_Word rel_num;
	Elf32_Rela *rela;

	elf_verify(img);
	if (err_code)
		return -err_code;

	/* Allocate memory for the BSS. */
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num++)
	{
		shdr = SHDR(img, ehdr, sec_num);
		if (shdr->sh_type != SHT_NOBITS)
			continue;
		bss = do_malloc(shdr->sh_size);
		if (bss == NULL)
			return -(err_code = ENOMEM);
		shdr->sh_offset = bss - img;
		break;
	}

	/* Perform relocations. */
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num++)
	{
		shdr = SHDR(img, ehdr, sec_num);
		if (shdr->sh_type != SHT_RELA && shdr->sh_type != SHT_REL)
			continue;
		rel_size = REL_SIZE(shdr);
		for (rel_num = 0; rel_num < shdr->sh_size / rel_size; rel_num++)
		{
			rela = REL_ADDR(img, shdr, rel_size, rel_num);
			elf_rel(img, shdr, rela);
			if (err_code)
				return -err_code;
		}
	}

	/* Find the entry point. */
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num++)
	{
		shdr = SHDR(img, ehdr, sec_num);
		if (shdr->sh_flags != SHF_EXECINSTR)
			continue;
		*ent = (Elf32_Addr) img + shdr->sh_offset;
		return 0;
	}

	return -(err_code = ENOEXEC);
}
