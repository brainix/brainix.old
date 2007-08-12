/*----------------------------------------------------------------------------*\
 |	elf.h								      |
 |									      |
 |	Copyright © 2002-2007, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is free software: you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation, either version 3 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FS_ELF_H
#define _FS_ELF_H

/* 32-bit data types: */
typedef unsigned long Elf32_Addr;  /* Unsigned program address. */
typedef unsigned short Elf32_Half; /* Unsigned medium integer.  */
typedef unsigned long Elf32_Off;   /* Unsigned file offset.     */
typedef long Elf32_Sword;          /* Signed large integer.     */
typedef unsigned long Elf32_Word;  /* Unsigned large integer.   */

/* Size of e_ident[]: */
#define EI_NIDENT	16

/* ELF header: */
typedef struct
{
	unsigned char e_ident[EI_NIDENT]; /* ELF identification.              */
	Elf32_Half e_type;                /* Object file type.                */
	Elf32_Half e_machine;             /* Required architecture.           */
	Elf32_Word e_version;             /* Object file version.             */
	Elf32_Addr e_entry;               /* Entry point.                     */
	Elf32_Off e_phoff;                /* Program header table offset.     */
	Elf32_Off e_shoff;                /* Section header table offset.     */
	Elf32_Word e_flags;               /* Processor-specific flags.        */
	Elf32_Half e_ehsize;              /* ELF header size.                 */
	Elf32_Half e_phentsize;           /* Program header table entry size. */
	Elf32_Half e_phnum;               /* # program header table entries.  */
	Elf32_Half e_shentsize;           /* Section header table entry size. */
	Elf32_Half e_shnum;               /* # section header table entries.  */
	Elf32_Half e_shstrndx;            /* Section name string table index. */
} Elf32_Ehdr;

/* Values for e_type: */
#define ET_NONE		0      /* No file type.       */
#define ET_REL		1      /* Relocatable file.   */
#define ET_EXEC		2      /* Executable file.    */
#define ET_DYN		3      /* Shared object file. */
#define ET_CORE		4      /* Core file.          */
#define ET_LOPROC	0xFF00 /* Processor-specific. */
#define ET_HIPROC	0xFFFF /* Processor-specific. */

/* Values for e_machine: */
#define EM_NONE		0 /* No machine.     */
#define EM_M32		1 /* AT&T WE 32100.  */
#define EM_SPARC	2 /* SPARC.          */
#define EM_386		3 /* Intel 80386.    */
#define EM_68K		4 /* Motorola 68000. */
#define EM_88K		5 /* Motorola 88000. */
#define EM_860		7 /* Intel 80860.    */
#define EM_MIPS		8 /* MIPS RS3000.    */

/* Values for e_version: */
#define EV_NONE		0 /* Invalid version. */
#define EV_CURRENT	1 /* Current version. */

/* e_ident[] identification indexes: */
#define EI_MAG0		 0 /* File identification.    */
#define EI_MAG1		 1 /* File identification.    */
#define EI_MAG2		 2 /* File identification.    */
#define EI_MAG3		 3 /* File identification.    */
#define EI_CLASS	 4 /* File class.             */
#define EI_DATA		 5 /* Data encoding.          */
#define EI_VERSION	 6 /* File version.           */
#define EI_PAD		 7 /* Start of padding bytes. */
#define EI_NIDENT	16 /* Size of e_ident[].      */

/* Values for e_ident[EI_MAG0] to e_ident[EI_MAG3]: */
#define ELFMAG0		0x7F /* e_ident[EI_MAG0]. */
#define ELFMAG1		'E'  /* e_ident[EI_MAG1]. */
#define ELFMAG2		'L'  /* e_ident[EI_MAG2]. */
#define ELFMAG3		'F'  /* e_ident[EI_MAG3]. */

/* Values for e_ident[EI_CLASS]: */
#define ELFCLASSNONE	0 /* Invalid class.  */
#define ELFCLASS32	1 /* 32-bit objects. */
#define ELFCLASS64	2 /* 64-bit objects. */

/* Values for e_ident[EI_DATA]: */
#define ELFDATANONE	0 /* Invalid data encoding.                         */
#define ELFDATA2LSB	1 /* 2's compliment values, least significant byte. */
#define ELFDATA2MSB	2 /* 2's compliment values, most significant byte.  */

/* Special section indexes: */
#define SHN_UNDEF	0      /* Undefined/missing/irrelevant/meaningless.   */
#define SHN_LORESERVE	0xFF00 /* Lower bound: reserved indexes.              */
#define SHN_LOPROC	0xFF00 /* Lower bound: processor-specific semantics.  */
#define SHN_HIPROC	0xFF1F /* Upper bound: processor-specific semantics.  */
#define SHN_ABS		0xFFF1 /* Absolute vals (not affected by relocation). */
#define SHN_COMMON	0xFFF2 /* Common symbols (unallocated C externs).     */
#define SHN_HIRESERVE	0xFFFF /* Upper bound: reserved indexes.              */

/* Section header: */
typedef struct
{
	Elf32_Word sh_name;      /* Section name (index into string table). */
	Elf32_Word sh_type;      /* Section contents and semantics.         */
	Elf32_Word sh_flags;     /* Miscellaneous attributes.               */
	Elf32_Addr sh_addr;      /* Section (memory) address.               */
	Elf32_Off sh_offset;     /* Section (file) offset.                  */
	Elf32_Word sh_size;      /* Section size (in file).                 */
	Elf32_Word sh_link;      /* Section header table index link.        */
	Elf32_Word sh_info;      /* Extra information.                      */
	Elf32_Word sh_addralign; /* Address alignment constraints.          */
	Elf32_Word sh_entsize;   /* Section entry size (for tables).        */
} Elf32_Shdr;

/* Values for sh_type: */
#define SHT_NULL	 0         /* Inactive; no associated section.        */
#define SHT_PROGBITS	 1         /* Program-defined information.            */
#define SHT_SYMTAB	 2         /* Symbols for link editing / dyn linking. */
#define SHT_STRTAB	 3         /* String table.                           */
#define SHT_RELA	 4         /* Relocation entries w explicit addends.  */
#define SHT_HASH	 5         /* Symbol hash table.                      */
#define SHT_DYNAMIC	 6         /* Information for dynamic linking.        */
#define SHT_NOTE	 7         /* Information that marks the file.        */
#define SHT_NOBITS	 8         /* No space, but resembles SHT_PROGBITS.   */
#define SHT_REL		 9         /* Relocation entries wo explicit addends. */
#define SHT_SHLIB	10         /* Reserved, but unspecified semantics.    */
#define SHT_DYNSYM	11         /* Minimal set of dynamic linking symbols. */
#define SHT_LOPROC	0x70000000 /* Lower bound: processor-specific.        */
#define SHT_HIPROC	0x7FFFFFFF /* Upper bound: processor-specific.        */
#define SHT_LOUSER	0x80000000 /* Lower bound: for application programs.  */
#define SHT_HIUSER	0xFFFFFFFF /* Upper bound: for application programs.  */

/* Values for sh_flags: */
#define SHF_WRITE	0x1        /* Data writable during execution.   */
#define SHF_ALLOC	0x2        /* Occupies memory during execution. */
#define SHF_EXECINSTR	0x4        /* Executable machine instructions.  */
#define SHF_MASKPROC	0xF0000000 /* Processor-specific semantics.     */

#define STN_UNDEF	0

/* Symbol table entry: */
typedef struct
{
	Elf32_Word st_name;     /* Symbol name (index into string table).    */
	Elf32_Addr st_value;    /* Symbol value (absolute value, addr, etc). */
	Elf32_Word st_size;     /* Symbol size (e.g. data object size).      */
	unsigned char st_info;  /* Symbol type and binding attributes.       */
	unsigned char st_other; /* No defined meaning.                       */
	Elf32_Half st_shndx;    /* Relevant section header table index.      */
} Elf32_Sym;

#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0xF)
#define ELF32_ST_INFO(b, t)	(((b) << 4) + ((t) & 0xF))

/* Relocation entry: */
typedef struct
{
	Elf32_Addr r_offset; /* Location to apply relocation action. */
	Elf32_Word r_info;   /* Symbol table index, relocation type. */
} Elf32_Rel;

/* Relocation entry: */
typedef struct
{
	Elf32_Addr r_offset;  /* Location to apply relocation action.        */
	Elf32_Word r_info;    /* Symbol table index, relocation type.        */
	Elf32_Sword r_addend; /* Addend to calc value for relocatable field. */
} Elf32_Rela;

#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((unsigned char) (i))
#define ELF32_R_INFO(s, t)	(((s) << 8) + (unsigned char) (t))

#define R_386_NONE	 0 /* */
#define R_386_32	 1 /* */
#define R_386_PC32	 2 /* */
#define R_386_GOT32	 3 /* */
#define R_386_PLT32	 4 /* */
#define R_386_COPY	 5 /* */
#define R_386_GLOB_DAT	 6 /* */
#define R_386_JMP_SLOT	 7 /* */
#define R_386_RELATIVE	 8 /* */
#define R_386_GOTOFF	 9 /* */
#define R_386_GOTPC	10 /* */

/* Program header: */
typedef struct
{
	Elf32_Word p_type;   /* How to interpret array element's information. */
	Elf32_Off p_offset;  /* Offset from beginning of file to segment.     */
	Elf32_Addr p_vaddr;  /* Virtual address of segment in memory.         */
	Elf32_Addr p_paddr;  /* Physical address of segment in memory.        */
	Elf32_Word p_filesz; /* Size of file image of segment; may be zero.   */
	Elf32_Word p_memsz;  /* Size of memory image of segment; may be zero. */
	Elf32_Word p_flags;  /* Flags relevant to segment.                    */
	Elf32_Word p_align;  /* Val to which segs are aligned in mem & file.  */
} Elf32_Phdr;

/* Values for p_type: */
#define PT_NULL		0          /* Unused; other members' vals undefined.  */
#define PT_LOAD		1          /* Loadable segment.                       */
#define PT_DYNAMIC	2          /* Dynamic linking information.            */
#define PT_INTERP	3          /* Location/size of path to interpreter.   */
#define PT_NOTE		4          /* Location/size of auxiliary information. */
#define PT_SHLIB	5          /* Reserved; but unspecified semantics.    */
#define PT_PHDR		6          /* Location/size of program header table.  */
#define PT_LOPROC	0x70000000 /* Processor-specific semantics.           */
#define PT_HIPROC	0x7FFFFFFF /* Processor-specific semantics.           */

/* Dynamic structure: */
typedef struct
{
	Elf32_Sword d_tag;        /* Dynamic array tag.       */
	union
	{
		Elf32_Word d_val; /* Integer value.           */
		Elf32_Addr d_ptr; /* Program virtual address. */
	} d_un;
} Elf32_Dyn;

extern Elf32_Dyn _DYNAMIC[];

/* Values for d_tag: */
#define DT_NULL		 0         /* End of _DYNAMIC array.                  */
#define DT_NEEDED	 1         /* String table offset of library name.    */
#define DT_PLTRELSZ	 2         /* Size of relocation entries of PLT.      */
#define DT_PLTGOT	 3         /* Address of PLT/GOT.                     */
#define DT_HASH		 4         /* Address of symbol hash table.           */
#define DT_STRTAB	 5         /* Address of string table.                */
#define DT_SYMTAB	 6         /* Address of symbol table.                */
#define DT_RELA		 7         /* Address of relocation table.            */
#define DT_RELASZ	 8         /* Size of DT_RELA relocation table.       */
#define DT_RELAENT	 9         /* Size of DT_RELA relocation entry.       */
#define DT_STRSZ	10         /* Size of string table.                   */
#define DT_SYMENT	11         /* Size of symbol table entry.             */
#define DT_INIT		12         /* Address of initialization function.     */
#define DT_FINI		13         /* Address of termination function.        */
#define DT_SONAME	14         /* String table offset of shared object.   */
#define DT_RPATH	15         /* String table offset of lib search path. */
#define DT_SYMBOLIC	16         /* */
#define DT_REL		17         /* Like DT_RELA except explicit addends.   */
#define DT_RELSZ	18         /* Size of DT_REL relocation table.        */
#define DT_RELENT	19         /* Size of DT_REL relocation entry.        */
#define DT_PLTREL	20         /* Type of relocation entry of PLT.        */
#define DT_DEBUG	21         /* Debugging information.                  */
#define DT_TEXTREL	22         /* */
#define DT_JMPREL	23         /* */
#define DT_LOPROC	0x70000000 /* Processor-specific semantics.           */
#define DT_HIPROC	0x7FFFFFFF /* Processor-specific semantics.           */

extern Elf32_Addr _GLOBAL_OFFSET_TABLE_[];

/* Function prototypes: */
int elf_verify(char *img);
int elf_val(char *img, Elf32_Word sec_num, unsigned char sym_num,
	Elf32_Addr *sym_val);
int elf_rel(char *img, Elf32_Shdr *shdr_1, Elf32_Rela *rela);
int elf_load(char *img, Elf32_Addr *ent);

#endif
