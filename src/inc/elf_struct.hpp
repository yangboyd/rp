/*
    This file is part of rp++.

    Copyright (C) 2014, Axel "0vercl0k" Souchet <0vercl0k at tuxfamily.org>
    All rights reserved.

    rp++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    rp++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with rp++.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ELF_STRUCT_H
#define ELF_STRUCT_H

#include "platform.h"
#include "toolbox.hpp"
#include "coloshell.hpp"
#include "section.hpp"
#include "rpexception.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <array>

/* Information extracted from winnt.h ; a bit of template-kung-fu and here it goes ! */

#define EI_NIDENT       16

#ifdef WINDOWS
#pragma pack(push)
#pragma pack(1)
#endif

template<class T>
struct Elf_Ehdr
{
    uint8_t  e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    T        e_entry;  /* Entry point */
    T        e_phoff;
    T        e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;

    void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const
    {
        w_yel_lf("-> ELF_Ehdr:");

        display_short_hex_field_lf(e_phoff);
        display_short_hex_field_lf(e_shoff);

        display_short_hex_field_lf(e_flags);

        display_short_hex_2fields_lf(e_phentsize, e_phnum);
        display_short_hex_2fields_lf(e_shentsize, e_shnum);
        display_short_hex_2fields_lf(e_shstrndx, e_ehsize);
    }
}
#ifdef LINUX
__attribute__((packed))
#endif
;


using Elf32_Ehdr = Elf_Ehdr<x86Version>;
using Elf64_Ehdr = Elf_Ehdr<x64Version>;

#define EI_OSABI 7
#define EI_CLASS 4

#define ELFCLASS32 1
#define ELFCLASS64 2

template<class T>
struct Elf_Phdr
{}
#ifdef LINUX
__attribute__((packed))
#endif
;

std::string type_to_str(const uint32_t p_type);

std::string flags_to_str(const uint32_t p_flags);

template<>
struct Elf_Phdr<x86Version>
{
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;

    explicit Elf_Phdr()
    {}

    void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const
    {
        w_yel_lf("-> Elf_Phdr32: ");
        std::cout << "    " << type_to_str(p_type) << " " << flags_to_str(p_flags) << std::endl;

        if(lvl > VERBOSE_LEVEL_1)
        {
            display_hex_2fields_lf(p_vaddr, p_filesz);
        }

        if(lvl > VERBOSE_LEVEL_2)
        {
            display_hex_2fields_lf(p_align, p_flags);
        }

        display_hex_2fields_lf(p_offset, p_paddr);
    }
}
#ifdef LINUX
__attribute__((packed))
#endif
;

template<>
struct Elf_Phdr<x64Version>
{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;

    explicit Elf_Phdr()
    {}

    void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const
    {
        w_yel_lf("-> Elf_Phdr64:"); 
        std::cout << "    " << type_to_str(p_type) << " " << flags_to_str(p_flags) << std::endl;

        if(lvl > VERBOSE_LEVEL_1)
        {
            display_short_hex_2fields_lf(p_vaddr, p_filesz);
        }

        if(lvl > VERBOSE_LEVEL_2)
        {
            display_short_hex_2fields_lf(p_align, p_flags);
        }

        display_short_hex_2fields_lf(p_offset, p_paddr);
    }
}
#ifdef LINUX
__attribute__((packed))
#endif
;

typedef Elf_Phdr<x86Version> Elf32_Phdr;
typedef Elf_Phdr<x64Version> Elf64_Phdr;

template<class T>
struct Elf_Shdr
{
    uint32_t sh_name;
    uint32_t sh_type;
    T        sh_flags;
    T        sh_addr;
    T        sh_offset;
    T        sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    T        sh_addralign;
    T        sh_entsize;
}
#ifdef LINUX
__attribute__((packed))
#endif
;

typedef Elf_Shdr<x86Version> Elf32_Shdr;
typedef Elf_Shdr<x64Version> Elf64_Shdr;

#ifdef WINDOWS
#pragma pack(pop)
#endif

template<class T>
struct Elf_Shdr_Abstraction
{
    Elf_Shdr<T> header;
    std::string name;

    void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const
    {
        /* remove the warning C4100 with /W4 */
        lvl = VERBOSE_LEVEL_1;

        std::cout << "0x" << std::setw(15) << std::setfill(' ') << std::left << header.sh_addr;
        std::cout << "0x" << std::setw(15) << std::setfill(' ') << std::left << header.sh_size;
        std::cout << std::setw(30) << std::setfill(' ') << std::left << name << std::endl;
        
        /* 
        if(lvl > VERBOSE_LEVEL_1)
        {
            std::cout << std::hex << "\t sh_type: " << header.sh_type << std::endl;
            std::cout << std::hex << "\t sh_flags: " <<header.sh_flags << std::endl;
            std::cout << std::hex << "\t sh_offset: " << header.sh_offset << std::endl;
        }

        if(lvl > VERBOSE_LEVEL_2)
        {
            std::cout << std::hex << "\t sh_link: " << header.sh_link << std::endl;
            std::cout << std::hex << "\t sh_info: " << header.sh_info << std::endl;
            std::cout << std::hex << "\t sh_addralign: " << header.sh_addralign << std::endl;
            std::cout << std::hex << "\t sh_entsize: " << header.sh_entsize << std::endl;
        }
        */
    }
};

typedef Elf_Shdr_Abstraction<x86Version> Elf_Shdr32_Abstraction;
typedef Elf_Shdr_Abstraction<x64Version> Elf_Shdr64_Abstraction;

struct ExecutableLinkingFormatLayout
{
    virtual ~ExecutableLinkingFormatLayout(void)
    {}
    
    virtual void fill_structures(std::ifstream &file) = 0;
    virtual void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const = 0;
    virtual uint64_t get_image_base_address(void) = 0;
    virtual std::vector<std::shared_ptr<Section>> get_executable_section(std::ifstream &file) const = 0;
	virtual uint16_t get_cpu(void) = 0;
};

#define SHT_SYMTAB      2
#define SHT_STRTAB      3

#define RP_ELFEM_386    0x03
#define RP_ELFEM_X86_64 0x3e
#define RP_ELFEM_ARM    0x28

template<class T>
struct ELFLayout : public ExecutableLinkingFormatLayout
{
    Elf_Ehdr<T> elfHeader;
    std::vector<std::shared_ptr<Elf_Phdr<T>>> elfProgramHeaders;
    std::vector<std::shared_ptr<Elf_Shdr_Abstraction<T>>> elfSectionHeaders;
    T offset_string_table, size_string_table;
    uint64_t base;

    ELFLayout(void)
    : ExecutableLinkingFormatLayout(), base(0)
    {}

    ~ELFLayout(void)
    {
    }

    void display(VerbosityLevel lvl = VERBOSE_LEVEL_1) const
    {
        uint32_t i = 0;
        elfHeader.display(lvl);

        for(auto &programheader : elfProgramHeaders)
                programheader->display(lvl);

        w_yel_lf("-> Elf Headers:");
        std::cout << std::setw(12) << std::setfill(' ') << std::left;
        w_gre("id");
        std::cout << std::setw(17) << std::setfill(' ') << std::left;
        w_gre("addr");
        std::cout << std::setw(17) << std::setfill(' ') << std::left;
        w_gre("size");
        std::cout << std::setw(32) << std::setfill(' ') << std::left;
        w_gre("name");
        std::cout << std::endl << std::setw(70) << std::setfill('-') << "-" << std::endl;

        for(auto &sectionheader : elfSectionHeaders)
        {
            std::cout << "0x" << std::setw(10) << std::setfill(' ') << std::left << i++;
            sectionheader->display(lvl);
        }
    }

    T find_string_table(std::ifstream &file)
    {
        Elf_Shdr<T> elf_shdr;
        std::streampos off = file.tellg();

        file.seekg((std::streamoff)elfHeader.e_shoff, std::ios::beg);

        for(uint32_t i = 0; i < elfHeader.e_shnum; ++i)
        {
            file.read((char*)&elf_shdr, sizeof(Elf_Shdr<T>));
            if(elf_shdr.sh_addr == 0 && elf_shdr.sh_type == SHT_STRTAB)
            {
                offset_string_table = elf_shdr.sh_offset;
                size_string_table = elf_shdr.sh_size;
                break;
            }
        }

        file.seekg(off);
        return offset_string_table;
    }

    void fill_structures(std::ifstream &file)
    {
        /* Remember where the caller was in the file */
        std::streampos off = file.tellg();

        /* 1] Dump the Elf Header */
        file.seekg(0, std::ios::beg);
        file.read((char*)&elfHeader, sizeof(Elf_Ehdr<T>));

        /* 2] Goto the first Program Header, and dump them */
        file.seekg((std::streamoff)elfHeader.e_phoff, std::ios::beg);
        for(uint32_t i = 0; i < elfHeader.e_phnum; ++i)
        {
            std::shared_ptr<Elf_Phdr<T>> pElfProgramHeader = std::make_shared<Elf_Phdr<T>>();

            file.read((char*)pElfProgramHeader.get(), sizeof(Elf_Phdr<T>));
            elfProgramHeaders.push_back(pElfProgramHeader);

            //XXX: Here we assume that the first LOAD program header encountered will
            // hold the image base address and I guess this assumption is quite wrong
            // Fuck you ELF.
            // https://stackoverflow.com/questions/18296276/base-address-of-elf
            if(type_to_str(pElfProgramHeader->p_type) == "LOAD" && base == 0)
                base = pElfProgramHeader->p_vaddr;
        }

        /* 3.1] If we want to know the name of the different section, 
         *    we need to find the string table section 
         */
        find_string_table(file);

        /* 3.2] Keep the string table in memory */
        file.seekg((std::streamoff)offset_string_table, std::ios::beg);
        
        std::vector<char> string_table_section((uint32_t)size_string_table);

        file.read(string_table_section.data(), (std::streamsize)size_string_table);

        /* 3.3] Goto the first Section Header, and dump them !*/
        file.seekg((std::streamoff)elfHeader.e_shoff, std::ios::beg);
        for(uint32_t i = 0; i < elfHeader.e_shnum; ++i)
        {
            std::shared_ptr<Elf_Shdr_Abstraction<T>> pElfSectionHeader = std::make_shared<Elf_Shdr_Abstraction<T>>();
            
            file.read((char*)&pElfSectionHeader->header, sizeof(Elf_Shdr<T>));

            /* 3.4] Resolve the name of the section */
            if(pElfSectionHeader->header.sh_name < size_string_table)
            {
                /* Yeah we know where is the string */
                char *name_section = string_table_section.data() + pElfSectionHeader->header.sh_name;
                std::string s(name_section, std::strlen(name_section));
                pElfSectionHeader->name = (s == "") ? std::string("unknown section") : s;
            }

            elfSectionHeaders.push_back(pElfSectionHeader);
        }

        /* Set correctly the pointer */
        file.seekg(off);
    }

    std::vector<std::shared_ptr<Section>> get_executable_section(std::ifstream &file) const
    {
        std::vector<std::shared_ptr<Section>> exec_sections;

        for(auto &programheader : elfProgramHeaders)
        {
            if(programheader->p_flags & 1)
            {
				// XXX: g++ + std::make_shared + packed struct
                std::shared_ptr<Section> sec(new Section(
                    type_to_str(programheader->p_type).c_str(),
                    programheader->p_offset,
                    programheader->p_vaddr,
                    programheader->p_filesz
                ));

                if(sec == nullptr)
                    RAISE_EXCEPTION("Cannot alocate a section");
                
                sec->dump(file);
                sec->set_props(Section::Executable);

                exec_sections.push_back(sec);
            }
        }

        return exec_sections;
    }

    uint64_t get_image_base_address(void)
    {
        return base;
    }

	uint16_t get_cpu(void)
	{
		return elfHeader.e_machine;
	}
};

#endif
