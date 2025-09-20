OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
	ram	(rwx)	: ORIGIN = 0x10000000, LENGTH = 0x01E00000
}

EXTERN(crt_init);

SECTIONS
{
	/*--------------------------------------------------------------------*/
	/* Code and read-only segment                                         */
	/*--------------------------------------------------------------------*/

	. = ORIGIN(ram);

	_ftext = .;
	PROVIDE( eprol = . );

	/* text: Program code section */
	.text : 
	{
		*(.text)
		*(.text.*)
		*(.gnu.linkonce.t.*)

		KEEP(*(.keep_rt_crt))
	}

	/* init: Code to execute before main (called by crt0.S) */
	.init : 
	{
		KEEP( *(.init) )
	}

	/* fini: Code to execute after main (called by crt0.S) */
	.fini : 
	{
		KEEP( *(.fini) )
	}

	/* rodata: Read-only data */
	.rodata : 
	{
		*(.rdata)
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r.*)
	}

	/* End of code and read-only segment */
	PROVIDE( etext = . );
	_etext = .;

	/*--------------------------------------------------------------------*/
	/* Global constructor/destructor segement                             */
	/*--------------------------------------------------------------------*/

	.preinit_array     :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	}

	.init_array     :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array ))
		PROVIDE_HIDDEN (__init_array_end = .);
	}

	.fini_array     :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array ))
		PROVIDE_HIDDEN (__fini_array_end = .);
	}

	/*--------------------------------------------------------------------*/
	/* Other misc gcc segments (this was in idt32.ld)                     */
	/*--------------------------------------------------------------------*/
	/* I am not quite sure about these sections but it seems they are for
		 C++ exception handling. I think .jcr is for "Java Class
		 Registration" but it seems to end up in C++ binaries as well. */

	.eh_frame_hdr     : { *(.eh_frame_hdr)     }
	.eh_frame         : { KEEP( *(.eh_frame) ) }
	.gcc_except_table : { *(.gcc_except_table) }
	.jcr              : { KEEP (*(.jcr))       } 

	/*--------------------------------------------------------------------*/
	/* Initialized data segment                                           */
	/*--------------------------------------------------------------------*/

	. = ALIGN(16);

	.data : 
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d.*)
	}

	.sdata : 
	{
		*(.sdata)
		*(.sdata.*)
		*(.srodata.*)
		*(.gnu.linkonce.s.*)
	}

	. = ALIGN(16);
	_gp = . + 0x800;
	__global_pointer$ = .;

		/* End of initialized data segment */
	PROVIDE( edata = . );
	_edata = .;

	/*--------------------------------------------------------------------*/
	/* Uninitialized data segment                                         */
	/*--------------------------------------------------------------------*/

	/* Start of uninitialized data segment */
	. = ALIGN(8);

	__bss_start = .;
	.bss : 
	{
		*(.bss)
		*(.bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)

		*(.sbss)
		*(.sbss.*)
		*(.gnu.linkonce.sb.*)
	}

	. = ALIGN(16);

	PROVIDE( end = . );
	_end = ALIGN(8);
}
